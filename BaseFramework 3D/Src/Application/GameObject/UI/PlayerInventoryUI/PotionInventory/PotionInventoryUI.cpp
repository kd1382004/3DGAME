#include "PotionInventoryUI.h"
#include"../../../../Info/VOL/VOLInfo.h"
#include"../../../../Scene/SceneManager.h"

#include"../../../../Info/KeyInfo/KeyInfo.h"
#include"../../../../Info/MouseInfo/MouseInfo.h"

#include"../../../Potions/PotionsType.h"
#include"../../../Potions/PotionUseController.h"

#include"../../../Character/Player/PlayerBase.h"
#include"../../../Character/Player/PlayerInventory/PlayerInventory.h"

#include"../../../Potions/PotionTexInfo/PotionTexInfo.h"

void PotionInventoryUI::Init()
{
	m_iconDimensions = { 40,40 };

	//---------- スクロールバーの画像 ----------
	const std::string scrollPath = "Asset/Textures/GameUI/Item/PlayerInventory/Scroll/";

	auto loadTex = [](std::shared_ptr<KdTexture>& _tex, const std::string& _path)
		{
			if (_tex) { return; }	// 読み込み済みなら何もしない
			_tex = std::make_shared<KdTexture>();
			_tex->Load(_path);
		};

	loadTex(m_scrollTrackTex, scrollPath + "ScrollTrack.png");

	loadTex(m_scrollThumbTopTex, scrollPath + "ScrollThumbTop.png");
	loadTex(m_scrollThumbMidTex, scrollPath + "ScrollThumbMid.png");
	loadTex(m_scrollThumbBottomTex, scrollPath + "ScrollThumbBottom.png");
	loadTex(m_scrollThumbGripTex, scrollPath + "ScrollThumbGrip.png");

	loadTex(m_scrollThumbTopHoverTex, scrollPath + "ScrollThumbTop_Hover.png");
	loadTex(m_scrollThumbMidHoverTex, scrollPath + "ScrollThumbMid_Hover.png");
	loadTex(m_scrollThumbBottomHoverTex, scrollPath + "ScrollThumbBottom_Hover.png");
	loadTex(m_scrollThumbGripHoverTex, scrollPath + "ScrollThumbGrip_Hover.png");

	//---------- 種類の切り替えボタンの画像 ----------
	const std::string categoryPath = "Asset/Textures/GameUI/Item/PlayerInventory/ItemCategory/";

	for (int i = 0; i < ItemCategory_Num; i++)
	{
		// 種類ごとのファイル名（通常は "名前.png"、選択中は "名前_Select.png"）
		std::string name;
		switch (i)
		{
		case ItemCategory_Consumable:	name = "Consumable";	break;
		case ItemCategory_Material:		name = "Material";		break;
		default:														break;
		}
		if (name.empty()) { continue; }

		loadTex(m_categoryTex[i], categoryPath + name + ".png");
		loadTex(m_categorySelectTex[i], categoryPath + name + "_Select.png");

		m_categorySiz = { 96,34 };
	}

	for (int i = ItemCategory_Num - 1; i >= 0; i--)
	{
		// 右から何番目か（一番右 = 0）
		const int fromRight = (ItemCategory_Num - 1) - i;

		m_categoryPos[i].y = m_spContext->back1Tex2DPos.y + 150;
		m_categoryPos[i].x = m_spContext->back1Tex2DPos.x + m_spContext->back1TexWidth / 2;
		m_categoryPos[i].x += -m_categoryRightMargin - m_categorySiz.x / 2;
		m_categoryPos[i].x -= fromRight * (m_categorySiz.x + m_categorySpacing);
	}

	//---------- 溝（レール） ----------
	// 横幅は画像から、縦はマスの段数ぶん
	const float step = m_iconDimensions.h + m_slotSpacing;
	const float gridTop = m_slotBasePos.y + m_iconDimensions.h / 2;
	const float gridBottom = gridTop - (m_slotRows * step - m_slotSpacing);
	const float gridRight = m_slotBasePos.x - m_iconDimensions.w / 2 + m_slotCols * (m_iconDimensions.w + m_slotSpacing) - m_slotSpacing;
	const float panelInnerRight = m_spContext->back1Tex2DPos.x + m_spContext->back1TexWidth / 2 - 26.0f;	// 鋲の内側の右端

	m_scrollTrackSiz.x = m_scrollTrackTex ? (float)m_scrollTrackTex->GetWidth() : 16.0f;
	m_scrollTrackSiz.y = gridTop - gridBottom;

	// マスの右端と、鋲の内側の右端の真ん中に置く
	m_scrollTrackPos.x = (gridRight + panelInnerRight) / 2;
	m_scrollTrackPos.y = (gridTop + gridBottom) / 2;

	//---------- つまみ ----------
	// 横幅と上下の端の高さは画像から
	m_scrollThumbSiz.x = m_scrollThumbTopTex ? (float)m_scrollThumbTopTex->GetWidth() : m_scrollTrackSiz.x;
	m_scrollThumbCapH = m_scrollThumbTopTex ? (float)m_scrollThumbTopTex->GetHeight() : 6.0f;

	// 一番短くても「上の端＋3本線＋下の端」が入る長さ
	const float gripH = m_scrollThumbGripTex ? (float)m_scrollThumbGripTex->GetHeight() : 7.0f;
	m_scrollThumbMinH = m_scrollThumbCapH * 2 + gripH + 2.0f;

	// 最初は溝の内側いっぱい（ScrollBarCalcThumb で毎フレーム決め直す）
	m_scrollThumbPos = m_scrollTrackPos;
	m_scrollThumbSiz.y = m_scrollTrackSiz.y - m_scrollTrackInset * 2;
}

bool PotionInventoryUI::OnInventoryOpen()
{
	// 開いたときは消耗品から
	m_nowCategory = ItemCategory_Consumable;

	// 開いたときは一番上から表示
	m_scrollRow = 0;
	m_scrollDragFlg = false;

	AddPotionTexInfo();
	if (m_itemIconInfo.size() > 0)
	{
		m_spContext->selectExplanationTex = m_itemIconInfo[0].m_ExplanationTex;
		m_spContext->selectNameTex = m_itemIconInfo[0].m_NameTex;
		m_selectPotionID = m_itemIconInfo[0].m_ItemID;
		return true;
	}

	m_spContext->selectNameTex = m_spContext->notSelectNameTex;
	return false;
}

void PotionInventoryUI::OnSelect()
{
	if (m_itemIconInfo.size() != 0)
	{
		m_spContext->selectExplanationTex = m_itemIconInfo[0].m_ExplanationTex;
		m_spContext->selectNameTex = m_itemIconInfo[0].m_NameTex;
		m_selectPotionID = m_itemIconInfo[0].m_ItemID;
	}
	else
	{
		m_selectPotionID = -999;
		m_spContext->selectExplanationTex = m_spContext->notSelectExplanationTex;
		m_spContext->selectNameTex = m_spContext->notSelectNameTex;
	}
}

void PotionInventoryUI::Update()
{
	CategoryUpdate();


	// ① マウスホイール・ドラッグで「何段目から表示するか」を決める
	ScrollBarUpdate();

	// ② その段数を使って、アイコンの位置を決める（スクロール分ずらす）
	AddPotionTexInfo();

	// ③ 最新のアイテム数で、つまみの長さと位置を計算
	ScrollBarCalcThumb();

	// ④ スクロール後の位置で当たり判定
	IconHit();

	PotionIUse();
}

void PotionInventoryUI::DrawSprite()
{

	CategoryDraw();

	const float step = m_iconDimensions.w + m_slotSpacing;       // アイコン ＋ すき間
	Math::Color slotCol = { 0, 0, 0, 0.25f };         // うっすら暗いマス

	for (int y = 0; y < m_slotRows; y++)
	{
		for (int x = 0; x < m_slotCols; x++)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(
				m_spContext->potionSelectTex,
				m_slotBasePos.x + x * step, m_slotBasePos.y - y * step,
				m_iconDimensions.w, m_iconDimensions.h, nullptr, &slotCol);
		}
	}

	// 0→1→0 と山なりに変化する値（演出の強さ）
	float t = m_spContext->UseEffectT();

	for (auto& tex : m_itemIconInfo)
	{
		// スクロールで見えない段のアイコンは描かない
		if (!IsInSlotView(tex.m_2DPos.y)) { continue; }

		float w = m_iconDimensions.w;
		float h = m_iconDimensions.h;

		bool isUseEffect = (t > 0.0f && tex.m_ItemID == m_spContext->useEffectItemID);
		if (isUseEffect)
		{
			w *= 1.0f + 0.3f * t;
			h *= 1.0f + 0.3f * t;
		}

		KdShaderManager::Instance().m_spriteShader.DrawTex(tex.m_IconTex, tex.m_2DPos.x, tex.m_2DPos.y, w, h);

		// 白く光らせる（同じアイコンを加算で重ねる）
		if (isUseEffect)
		{
			Math::Color flash = { 1,1,1,t * 0.8f };
			KdShaderManager::Instance().ChangeBlendState(KdBlendState::Add);
			KdShaderManager::Instance().m_spriteShader.DrawTex(tex.m_IconTex, tex.m_2DPos.x, tex.m_2DPos.y, w, h, nullptr, &flash);
			KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);
		}

		if (tex.m_ItemID == m_selectPotionID)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_spContext->potionSelectTex, tex.m_2DPos.x, tex.m_2DPos.y, w, h);
		}
	}

	//使用ボタン（消耗品のときだけ）
	if (m_spContext->useTex && m_nowCategory == ItemCategory_Consumable)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spContext->useTex, m_spContext->useTex2DPos.x, m_spContext->useTex2DPos.y);
	}

	//スクロールバーの描画
	ScrollBarDraw();
}

void PotionInventoryUI::AddPotionTexInfo()
{
	std::shared_ptr<PlayerBase >spPlayer = m_spContext->wpPlayer.lock();
	if (!spPlayer) { return; }

	std::shared_ptr<PlayerInventory> spPlayerInventory = spPlayer->GetPlayerInventory();
	if (!spPlayerInventory) { return; }

	std::shared_ptr<PotionTexInfo> spTexInfo = m_spContext->wpPotionTexInfo.lock();

	//---------- 種類ごとの元のリスト ----------
	std::vector<Inventory> list;
	switch (m_nowCategory)
	{
	case ItemCategory_Consumable:	list = spPlayerInventory->GetPotionsInventory();				break;	// 消耗品（ポーション）
	case ItemCategory_Material:		list = spPlayerInventory->GetWeaponsStrengtheningInventory();	break;	// 素材（武器強化アイテム）
	default:																						break;
	}

	m_itemIconInfo.clear();

	int i = 0;
	int j = 0;
	for (const Inventory& item : list)
	{
		if (item.m_num > 0)
		{
			ItemIconInfo itemIconInfo;
			itemIconInfo.m_ItemID = item.m_ID;

			if (i >= m_slotCols)
			{
				j++;
				i = 0;
			}

			float x = m_slotBasePos.x + i * m_iconDimensions.w + i * m_slotSpacing;
			// スクロールした段数(m_scrollRow)だけ上にずらす
			float y = m_slotBasePos.y - (j - m_scrollRow) * (m_iconDimensions.h + m_slotSpacing);
			itemIconInfo.m_2DPos = { x,y };
			itemIconInfo.m_num = item.m_num;
			itemIconInfo.m_name = item.m_name;

			//---------- 種類ごとの画像 ----------
			if (spTexInfo)
			{
				switch (m_nowCategory)
				{
				case ItemCategory_Consumable:
					itemIconInfo.m_IconTex = spTexInfo->GetPotionIcon(item.m_ID);
					itemIconInfo.m_ExplanationTex = spTexInfo->GetPotionExplanation(item.m_ID);
					itemIconInfo.m_NameTex = spTexInfo->GetPotionName(item.m_ID);
					break;
				case ItemCategory_Material:
					itemIconInfo.m_IconTex = spTexInfo->GetWeaponsStrengtheningItemIcon(item.m_ID);
					itemIconInfo.m_ExplanationTex = spTexInfo->GetWeaponsStrengtheningItemExplanation(item.m_ID);
					itemIconInfo.m_NameTex = spTexInfo->GetWeaponsStrengtheningItemName(item.m_ID);
					break;
				default:
					break;
				}
			}

			m_itemIconInfo.push_back(itemIconInfo);
			i++;
		}
		else
		{
			if (item.m_ID == m_selectPotionID)
			{
				m_selectPotionID = -999;
			}
		}
	}

	// 選んでいたアイテムが一覧に無くなっていたら、先頭を選び直す
	bool flg = false;
	for (auto& icon : m_itemIconInfo)
	{
		if (m_selectPotionID == icon.m_ItemID)
		{
			flg = true;
		}
	}

	if (!flg && m_itemIconInfo.size() != 0)
	{
		m_spContext->selectExplanationTex = m_itemIconInfo[0].m_ExplanationTex;
		m_selectPotionID = m_itemIconInfo[0].m_ItemID;
		m_spContext->selectNameTex = m_itemIconInfo[0].m_NameTex;
	}
}

void PotionInventoryUI::IconHit()
{
	if (m_itemIconInfo.size() <= 0)
	{
		m_spContext->selectExplanationTex = m_spContext->notSelectExplanationTex;
		m_spContext->selectNameTex = m_spContext->notSelectNameTex;
		return;
	}


	POINT mousePos = MouseInfo::Instance().m_windowPos;

	for (auto& potion : m_itemIconInfo)
	{
		// スクロールで見えない段のアイコンは選べない
		if (!IsInSlotView(potion.m_2DPos.y))
		{
			potion.m_hit = false;
			continue;
		}

		float Left = potion.m_2DPos.x - m_iconDimensions.w / 2;
		float Right = potion.m_2DPos.x + m_iconDimensions.w / 2;
		float Top = potion.m_2DPos.y + m_iconDimensions.h / 2;
		float Bot = potion.m_2DPos.y - m_iconDimensions.h / 2;


		if (mousePos.x >= Left && mousePos.x <= Right &&
			mousePos.y >= Bot && mousePos.y <= Top)
		{
			MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
			potion.m_hit = true;
			if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
			{
				m_spContext->selectExplanationTex = potion.m_ExplanationTex;
				m_selectPotionID = potion.m_ItemID;
				m_spContext->selectNameTex = potion.m_NameTex;
			}
		}
		else
		{
			potion.m_hit = false;
		}
	}
}

void PotionInventoryUI::PotionIUse()
{
	std::shared_ptr<PlayerBase>spPlayer = m_spContext->wpPlayer.lock();
	if (!spPlayer) { return; }

	std::shared_ptr<PlayerInventory> spPlayerInventory = spPlayer->GetPlayerInventory();
	if (!spPlayerInventory) { return; }

	//---------- 右の枠に出す個数（どの種類でも入れる） ----------
	switch (m_nowCategory)
	{
	case ItemCategory_Consumable:
		m_spContext->num = spPlayerInventory->GetPotionsInventoryNum(m_selectPotionID);
		break;
	case ItemCategory_Material:
		m_spContext->num = spPlayerInventory->GetWeaponsStrengtheningInventoryNum(m_selectPotionID);
		break;
	default:
		break;
	}

	//---------- 使用ボタンは消耗品のときだけ ----------
	if (m_nowCategory != ItemCategory_Consumable) { return; }

	const std::shared_ptr<KdTexture>& useTex = m_spContext->useTex;
	if (!useTex) { return; }
	const Math::Vector2& useTex2DPos = m_spContext->useTex2DPos;

	POINT mousePos = MouseInfo::Instance().m_windowPos;

	float Left = useTex2DPos.x - useTex->GetWidth() / 2;
	float Right = useTex2DPos.x + useTex->GetWidth() / 2;
	float Top = useTex2DPos.y + useTex->GetHeight() / 2;
	float Bot = useTex2DPos.y - useTex->GetHeight() / 2;

	if (mousePos.x >= Left && mousePos.x <= Right &&
		mousePos.y >= Bot && mousePos.y <= Top)
	{
		MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
		if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
		{
			if (m_itemIconInfo.size() <= 0 || m_selectPotionID == -999) { return; }
			std::shared_ptr<PotionUseController >spPotionUseController = m_spContext->wpPotionUseController.lock();
			if (!spPotionUseController) { return; }

			spPotionUseController->SetPlayer(spPlayer);
			spPotionUseController->PotionUse(m_selectPotionID);
			spPlayerInventory->UsePotionsInventory(m_selectPotionID);

			m_spContext->useEffectType = UseEffectType::Bounce;
			m_spContext->useEffectTimer = m_spContext->useEffectTime;
			m_spContext->useEffectItemID = m_selectPotionID;

			// 使った後の数にすぐ合わせる
			m_spContext->num = spPlayerInventory->GetPotionsInventoryNum(m_selectPotionID);
		}
	}
}

void PotionInventoryUI::CategoryUpdate()
{
	POINT mousePos = MouseInfo::Instance().m_windowPos;

	for (int i = 0; i < ItemCategory_Num; i++)
	{
		// ボタンの四角（m_categoryPos[i] が中心）
		float Left = m_categoryPos[i].x - m_categorySiz.x / 2;
		float Right = m_categoryPos[i].x + m_categorySiz.x / 2;
		float Top = m_categoryPos[i].y + m_categorySiz.y / 2;
		float Bot = m_categoryPos[i].y - m_categorySiz.y / 2;

		// マウスがボタンの上にあるか
		bool isOnButton = (mousePos.x >= Left && mousePos.x <= Right &&
			mousePos.y >= Bot && mousePos.y <= Top);

		if (isOnButton)
		{
			MouseInfo::Instance().SetMouseType(MouseInfo::MouseType_HIT);
			if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
			{
				// 今と違う種類のときだけ切り替える（同じボタンを押しても何もしない）
				if (m_nowCategory != (ItemCategory)i)
				{
					m_nowCategory = (ItemCategory)i;
					OnCategoryChange();

					// ページをめくる音
					{
						std::shared_ptr<KdSoundInstance> se = KdAudioManager::Instance().Play("Asset/Sound/SE/UI/PageTurn.wav", false);
						if (se) { se->SetVolume(VOLInfo::Instance().GetMSSEVol() * 1.0f); }
					}
				}
			}
		}
	}

}

void PotionInventoryUI::CategoryDraw()
{
	// ドット絵がにじまないよう、ポイントフィルタで描く（スクロールバーと同じ）
	KdShaderManager::Instance().m_spriteShader.Begin(false);

	for (int i = 0; i < ItemCategory_Num; i++)
	{
		// 選ばれている種類は明るい画像、それ以外は通常の画像
		const std::shared_ptr<KdTexture>& tex = (i == m_nowCategory) ? m_categorySelectTex[i] : m_categoryTex[i];
		if (!tex) { continue; }

		KdShaderManager::Instance().m_spriteShader.DrawTex(tex, m_categoryPos[i].x, m_categoryPos[i].y);
	}

	KdShaderManager::Instance().m_spriteShader.End();
}

void PotionInventoryUI::OnCategoryChange()
{
	// スクロールを一番上に戻す（ドラッグ中なら止める）
	m_scrollRow = 0;
	m_scrollDragFlg = false;

	// 今の種類で一覧を作り直す
	AddPotionTexInfo();

	if (m_itemIconInfo.size() > 0)
	{
		// 先頭のアイテムを選ぶ
		m_selectPotionID = m_itemIconInfo[0].m_ItemID;
		m_spContext->selectExplanationTex = m_itemIconInfo[0].m_ExplanationTex;
		m_spContext->selectNameTex = m_itemIconInfo[0].m_NameTex;
	}
	else
	{
		// 何も持っていなければ「選択なし」の表示
		m_selectPotionID = -999;
		m_spContext->selectExplanationTex = m_spContext->notSelectExplanationTex;
		m_spContext->selectNameTex = m_spContext->notSelectNameTex;
	}
}

void PotionInventoryUI::ScrollBarDraw()
{
	// ドット絵がにじまないよう、ポイントフィルタで描く（NumDraw と同じ）
	KdShaderManager::Instance().m_spriteShader.Begin(false);

	//溝
	if (m_scrollTrackTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_scrollTrackTex, m_scrollTrackPos.x, m_scrollTrackPos.y, m_scrollTrackSiz.x, m_scrollTrackSiz.y);
	}

	// マウスが乗っているときは明るい画像を使う
	const std::shared_ptr<KdTexture>& topTex = m_scrollThumbHoverFlg ? m_scrollThumbTopHoverTex : m_scrollThumbTopTex;
	const std::shared_ptr<KdTexture>& midTex = m_scrollThumbHoverFlg ? m_scrollThumbMidHoverTex : m_scrollThumbMidTex;
	const std::shared_ptr<KdTexture>& bottomTex = m_scrollThumbHoverFlg ? m_scrollThumbBottomHoverTex : m_scrollThumbBottomTex;
	const std::shared_ptr<KdTexture>& gripTex = m_scrollThumbHoverFlg ? m_scrollThumbGripHoverTex : m_scrollThumbGripTex;

	// つまみの上端・下端の y（y は上がプラス）
	const float thumbTop = m_scrollThumbPos.y + m_scrollThumbSiz.y / 2;
	const float thumbBottom = m_scrollThumbPos.y - m_scrollThumbSiz.y / 2;

	//つまみ(上の端)
	if (topTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(topTex, m_scrollThumbPos.x, thumbTop - m_scrollThumbCapH / 2, m_scrollThumbSiz.x, m_scrollThumbCapH);
	}

	//つまみ(真ん中) … 上の端と下端を除いた長さに伸ばす
	const float midH = m_scrollThumbSiz.y - m_scrollThumbCapH * 2;
	if (midTex && midH > 0.0f)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(midTex, m_scrollThumbPos.x, m_scrollThumbPos.y, m_scrollThumbSiz.x, midH);
	}

	//つまみ(下の端)
	if (bottomTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(bottomTex, m_scrollThumbPos.x, thumbBottom + m_scrollThumbCapH / 2, m_scrollThumbSiz.x, m_scrollThumbCapH);
	}

	//つまみ(中央の3本線) … 画像そのままの大きさで真ん中に
	if (gripTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(gripTex, m_scrollThumbPos.x, m_scrollThumbPos.y, gripTex->GetWidth(), gripTex->GetHeight());
	}

	KdShaderManager::Instance().m_spriteShader.End();
}

void PotionInventoryUI::ScrollBarUpdate()
{

	//---------- スクロールできるか ----------
	// 全部の段数（切り上げ）
	int totalRows = ((int)m_itemIconInfo.size() + m_slotCols - 1) / m_slotCols;

	// 一番下までスクロールしたときの段
	int maxRow = std::max(0, totalRows - m_slotRows);

	// スクロールの必要がなければ何もしない
	if (maxRow == 0)
	{
		m_scrollRow = 0;
		m_scrollDragFlg = false;
		m_scrollThumbHoverFlg = false;
		return;
	}

	//----------マウスと当たり判定の四角 ----------
	POINT mousePos = MouseInfo::Instance().m_windowPos;

	// つまみの四角
	float thumbLeft = m_scrollThumbPos.x - m_scrollThumbSiz.x / 2;
	float thumbRight = m_scrollThumbPos.x + m_scrollThumbSiz.x / 2;
	float thumbTop = m_scrollThumbPos.y + m_scrollThumbSiz.y / 2;
	float thumbBot = m_scrollThumbPos.y - m_scrollThumbSiz.y / 2;

	// 溝の四角
	float trackLeft = m_scrollTrackPos.x - m_scrollTrackSiz.x / 2;
	float trackRight = m_scrollTrackPos.x + m_scrollTrackSiz.x / 2;


	float trackTop = m_scrollTrackPos.y + m_scrollTrackSiz.y / 2 - m_scrollTrackInset;
	float trackBot = m_scrollTrackPos.y - m_scrollTrackSiz.y / 2 + m_scrollTrackInset;

	// マウスがつまみ・溝の上にあるか
	bool isOnThumb = (mousePos.x >= thumbLeft && mousePos.x <= thumbRight &&
		mousePos.y >= thumbBot && mousePos.y <= thumbTop);

	bool isOnTrack = (mousePos.x >= trackLeft && mousePos.x <= trackRight &&
		mousePos.y >= trackBot && mousePos.y <= trackTop);

	//----------ドラッグ開始（つまみの上で押した瞬間） ----------
	if (isOnThumb && !m_scrollDragFlg)
	{
		MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);

		if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
		{
			m_scrollDragFlg = true;
			// つかんだ位置とつまみの中心のずれ（ドラッグ中は変えない）
			m_scrollDragOffsetY = (float)mousePos.y - m_scrollThumbPos.y;
		}
	}

	//----------ドラッグ中 ----------
	if (m_scrollDragFlg)
	{
		// 左ボタンが押されたままか（離したらドラッグ終了）
		if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
		{
			m_scrollDragFlg = false;
		}
		else
		{
			MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);

			// つまみの中心が動ける範囲（つまみが溝からはみ出さないように、長さの半分だけ内側）
			float thumbMoveTop = trackTop - m_scrollThumbSiz.y / 2;	// 一番上までスクロールしたときの中心のy
			float thumbMoveBot = trackBot + m_scrollThumbSiz.y / 2;	// 一番下までスクロールしたときの中心のy

			// つまみの中心が来てほしいy（つかんだ位置のずれを引く）
			float wantY = (float)mousePos.y - m_scrollDragOffsetY;

			// 動ける範囲のどこにいるかを 0〜1 の割合にする（0 = 一番上、1 = 一番下）
			float moveRange = thumbMoveTop - thumbMoveBot;
			float ratio = 0.0f;
			if (moveRange > 0.0f)
			{
				ratio = (thumbMoveTop - wantY) / moveRange;
			}
			ratio = std::clamp(ratio, 0.0f, 1.0f);

			// 割合を段に直す（四捨五入）
			m_scrollRow = (int)std::round(ratio * maxRow);
		}
	}

	//----------溝をクリック（つまみ以外のところ）→ 1画面分動く ----------
	else if (isOnTrack && !isOnThumb)
	{
		MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);

		if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
		{
			if ((float)mousePos.y > m_scrollThumbPos.y)
			{
				m_scrollRow -= m_slotRows;	// つまみより上 → 上へ
			}
			else
			{
				m_scrollRow += m_slotRows;	// つまみより下 → 下へ
			}
		}
	}

	//---------- マウスホイール ----------

	// 左の大きい枠の上にマウスがあるときだけホイールを効かせる
	const float panelLeft = m_spContext->back1Tex2DPos.x - m_spContext->back1TexWidth / 2;
	const float panelRight = m_spContext->back1Tex2DPos.x + m_spContext->back1TexWidth / 2;
	bool isOnPanel = (mousePos.x >= panelLeft && mousePos.x <= panelRight);


	if (isOnPanel && !m_scrollDragFlg)
	{
		int mouseWheelValue = MouseInfo::Instance().GetMouseWheelValue();
		if (mouseWheelValue >= 1) { m_scrollRow -= 1; }
		else if (mouseWheelValue <= -1) { m_scrollRow += 1; }
	}


	// つまみの色：マウスが乗っているか、ドラッグ中なら明るく
	m_scrollThumbHoverFlg = isOnThumb || m_scrollDragFlg;


	//----------範囲内に収める ----------
	// ドラッグ・溝クリック・ホイールのどれで変わっても、0 〜 maxRow を超えないように
	m_scrollRow = std::clamp(m_scrollRow, 0, maxRow);
}

void PotionInventoryUI::ScrollBarCalcThumb()
{

	// 全部の段数（切り上げ）と、一番下までスクロールしたときの段
	int totalRows = ((int)m_itemIconInfo.size() + m_slotCols - 1) / m_slotCols;
	int maxRow = std::max(0, totalRows - m_slotRows);

	// 横の位置と幅は溝に合わせる
	m_scrollThumbPos.x = m_scrollTrackPos.x;
	m_scrollThumbSiz.x = m_scrollTrackSiz.x;

	// スクロールの必要がなければ、つまみは溝いっぱい
	if (maxRow == 0)
	{
		m_scrollThumbSiz.y = m_scrollTrackSiz.y - m_scrollTrackInset * 2;
		m_scrollThumbPos.y = m_scrollTrackPos.y;
		return;
	}



	//---------- つまみの長さ ----------
	// 溝の高さ × 見えている段数 ÷ 全部の段数（短すぎるとつかめないので下限あり）
	float innerH = m_scrollTrackSiz.y - m_scrollTrackInset * 2;
	float thumbH = innerH * (float)m_slotRows / (float)totalRows;
	m_scrollThumbSiz.y = std::clamp(thumbH, m_scrollThumbMinH, innerH);

	//---------- つまみの位置 ----------
	// つまみの中心が動ける範囲（溝からはみ出さないように、長さの半分だけ内側）
	float trackTop = m_scrollTrackPos.y + m_scrollTrackSiz.y / 2 - m_scrollTrackInset;
	float trackBot = m_scrollTrackPos.y - m_scrollTrackSiz.y / 2 + m_scrollTrackInset;

	float thumbMoveTop = trackTop - m_scrollThumbSiz.y / 2;
	float thumbMoveBot = trackBot + m_scrollThumbSiz.y / 2;

	// 今の段が、一番上(0)〜一番下(maxRow)のどこか → 0〜1 の割合
	float ratio = (float)m_scrollRow / (float)maxRow;

	// 割合の位置につまみを置く（0 = 一番上、1 = 一番下）
	m_scrollThumbPos.y = thumbMoveTop - (thumbMoveTop - thumbMoveBot) * ratio;
}

bool PotionInventoryUI::IsInSlotView(float _y) const
{
	// 見えている一番上の段・一番下の段の中心y（少し余裕を持たせて判定）
	const float step = m_iconDimensions.h + m_slotSpacing;
	const float topY = m_slotBasePos.y + step * 0.5f;
	const float botY = m_slotBasePos.y - (m_slotRows - 1) * step - step * 0.5f;
	return (_y <= topY && _y >= botY);
}