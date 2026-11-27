//---- 自分のヘッダー ----
#include "PotionInventoryUI.h"												//インベントリ画面(ポーションタブ)

//---- シーン ----
#include"../../../../Scene/SceneManager.h"									//シーンの管理

//---- プレイヤー ----
#include"../../../Character/Player/PlayerBase.h"							//プレイヤー
#include"../../../Character/Player/PlayerInventory/PlayerInventory.h"		//プレイヤーの所持品

//---- ポーション ----
#include"../../../Potions/PotionsType.h"									//ポーションの種類
#include"../../../Potions/PotionUseController.h"							//ポーションの使用処理
#include"../../../Potions/PotionTexInfo/PotionTexInfo.h"					//アイテムの名前・画像情報

//---- 便利機能 ----
#include"../../../../Info/VOL/VOLInfo.h"									//音量設定
#include"../../../../Info/KeyInfo/KeyInfo.h"								//キー入力
#include"../../../../Info/MouseInfo/MouseInfo.h"							//マウス入力
#include"../../../../Info/NumDraw/NumDraw.h"

//=============================================
// 
//=============================================

void PotionInventoryUI::Init()
{
	m_iconDimensions = { 40,40 };

	//---------- スクロールバーの画像 ----------
	const std::string scrollPath = "Asset/Textures/GameUI/Item/PlayerInventory/Scroll/";

	auto loadTex = [](std::shared_ptr<KdTexture>& _spTex, const std::string& _path)
		{
			if (_spTex) { return; }	// 読み込み済みなら何もしない
			_spTex = std::make_shared<KdTexture>();
			_spTex->Load(_path);
		};

	loadTex(m_spScrollTrackTex, scrollPath + "ScrollTrack.png");

	loadTex(m_spScrollThumbTopTex, scrollPath + "ScrollThumbTop.png");
	loadTex(m_spScrollThumbMidTex, scrollPath + "ScrollThumbMid.png");
	loadTex(m_spScrollThumbBottomTex, scrollPath + "ScrollThumbBottom.png");
	loadTex(m_spScrollThumbGripTex, scrollPath + "ScrollThumbGrip.png");

	loadTex(m_spScrollThumbTopHoverTex, scrollPath + "ScrollThumbTop_Hover.png");
	loadTex(m_spScrollThumbMidHoverTex, scrollPath + "ScrollThumbMid_Hover.png");
	loadTex(m_spScrollThumbBottomHoverTex, scrollPath + "ScrollThumbBottom_Hover.png");
	loadTex(m_spScrollThumbGripHoverTex, scrollPath + "ScrollThumbGrip_Hover.png");

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

		loadTex(m_spCategoryTex[i], categoryPath + name + ".png");
		loadTex(m_spCategorySelectTex[i], categoryPath + name + "_Select.png");

		m_categorySize = { 96,34 };
	}

	for (int i = ItemCategory_Num - 1; i >= 0; i--)
	{
		// 右から何番目か（一番右 = 0）
		const int fromRight = (ItemCategory_Num - 1) - i;

		m_categoryPos[i].y = m_spContext->m_back1Tex2DPos.y + 150;
		m_categoryPos[i].x = m_spContext->m_back1Tex2DPos.x + m_spContext->m_back1TexWidth / 2;
		m_categoryPos[i].x += -m_categoryRightMargin - m_categorySize.x / 2;
		m_categoryPos[i].x -= fromRight * (m_categorySize.x + m_categorySpacing);
	}

	//---------- 溝（レール） ----------
	// 横幅は画像から、縦はマスの段数ぶん
	const float step = m_iconDimensions.m_h + m_slotSpacing;
	const float gridTop = m_slotBasePos.y + m_iconDimensions.m_h / 2;
	const float gridBottom = gridTop - (m_slotRows * step - m_slotSpacing);
	const float gridRight = m_slotBasePos.x - m_iconDimensions.m_w / 2 + m_slotCols * (m_iconDimensions.m_w + m_slotSpacing) - m_slotSpacing;
	const float panelInnerRight = m_spContext->m_back1Tex2DPos.x + m_spContext->m_back1TexWidth / 2 - 26.0f;	// 鋲の内側の右端

	m_scrollTrackSize.x = m_spScrollTrackTex ? (float)m_spScrollTrackTex->GetWidth() : 16.0f;
	m_scrollTrackSize.y = gridTop - gridBottom;

	// マスの右端と、鋲の内側の右端の真ん中に置く
	m_scrollTrackPos.x = (gridRight + panelInnerRight) / 2;
	m_scrollTrackPos.y = (gridTop + gridBottom) / 2;

	//---------- つまみ ----------
	// 横幅と上下の端の高さは画像から
	m_scrollThumbSize.x = m_spScrollThumbTopTex ? (float)m_spScrollThumbTopTex->GetWidth() : m_scrollTrackSize.x;
	m_scrollThumbCapH = m_spScrollThumbTopTex ? (float)m_spScrollThumbTopTex->GetHeight() : 6.0f;

	// 一番短くても「上の端＋3本線＋下の端」が入る長さ
	const float gripH = m_spScrollThumbGripTex ? (float)m_spScrollThumbGripTex->GetHeight() : 7.0f;
	m_scrollThumbMinH = m_scrollThumbCapH * 2 + gripH + 2.0f;

	// 最初は溝の内側いっぱい（ScrollBarCalcThumb で毎フレーム決め直す）
	m_scrollThumbPos = m_scrollTrackPos;
	m_scrollThumbSize.y = m_scrollTrackSize.y - m_scrollTrackInset * 2;

	//---------- スロット ----------
	loadTex(m_SlotRegisterTex, "Asset/Textures/GameUI/Item/PlayerInventory/SlotRegisterTex.png");
	loadTex(m_SlotUnregisterTex, "Asset/Textures/GameUI/Item/PlayerInventory/SlotUnregisterTex.png");
	loadTex(m_SlotFullTex, "Asset/Textures/GameUI/Item/PlayerInventory/SlotFullTex.png");
	loadTex(m_spNumSlashTex, "Asset/Textures/Num/NumSlash.png");


	m_SlotTexPos = m_spContext->m_useTex2DPos;
	m_SlotTexPos.x += 20;
	m_SlotTexPos.y += 80;
}

void PotionInventoryUI::OnSelect()
{
	if (m_itemIconInfo.size() != 0)
	{
		m_spContext->m_spSelectExplanationTex = m_itemIconInfo[0].m_spExplanationTex;
		m_spContext->m_spSelectNameTex = m_itemIconInfo[0].m_spNameTex;
		m_selectPotionID = m_itemIconInfo[0].m_itemID;
	}
	else
	{
		m_selectPotionID = -999;
		m_spContext->m_spSelectExplanationTex = m_spContext->m_spNotSelectExplanationTex;
		m_spContext->m_spSelectNameTex = m_spContext->m_spNotSelectNameTex;
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

	PotionUse();


	std::shared_ptr<PlayerBase>spPlayer = m_spContext->m_wpPlayer.lock();
	if (!spPlayer) { return; }

	std::shared_ptr<PlayerInventory> spPlayerInventory = spPlayer->GetPlayerInventory();
	if (!spPlayerInventory) { return; }

	//クイックスロットに登録or解除
	if (KeyInfo::Instance().GetValidKeyPush(m_SlotSetKey, true))
	{
		spPlayerInventory->AddQuickSlot(m_selectPotionID);
	}
}

void PotionInventoryUI::DrawSprite()
{

	CategoryDraw();

	const float step = m_iconDimensions.m_w + m_slotSpacing;       // アイコン ＋ すき間
	Math::Color slotCol = { 0, 0, 0, 0.25f };         // うっすら暗いマス

	for (int y = 0; y < m_slotRows; y++)
	{
		for (int x = 0; x < m_slotCols; x++)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(
				m_spContext->m_spPotionSelectTex,
				m_slotBasePos.x + x * step, m_slotBasePos.y - y * step,
				m_iconDimensions.m_w, m_iconDimensions.m_h, nullptr, &slotCol);
		}
	}

	// 0→1→0 と山なりに変化する値（演出の強さ）
	float t = m_spContext->UseEffectT();

	for (auto& tex : m_itemIconInfo)
	{
		// スクロールで見えない段のアイコンは描かない
		if (!IsInSlotView(tex.m_2DPos.y)) { continue; }

		float w = m_iconDimensions.m_w;
		float h = m_iconDimensions.m_h;

		bool isUseEffect = (t > 0.0f && tex.m_itemID == m_spContext->m_useEffectItemID);
		if (isUseEffect)
		{
			w *= 1.0f + 0.3f * t;
			h *= 1.0f + 0.3f * t;
		}

		KdShaderManager::Instance().m_spriteShader.DrawTex(tex.m_spIconTex, tex.m_2DPos.x, tex.m_2DPos.y, w, h);

		// 白く光らせる（同じアイコンを加算で重ねる）
		if (isUseEffect)
		{
			Math::Color flash = { 1,1,1,t * 0.8f };
			KdShaderManager::Instance().ChangeBlendState(KdBlendState::Add);
			KdShaderManager::Instance().m_spriteShader.DrawTex(tex.m_spIconTex, tex.m_2DPos.x, tex.m_2DPos.y, w, h, nullptr, &flash);
			KdShaderManager::Instance().ChangeBlendState(KdBlendState::Alpha);
		}

		if (tex.m_itemID == m_selectPotionID)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_spContext->m_spPotionSelectTex, tex.m_2DPos.x, tex.m_2DPos.y, w, h);
		}
	}

	//スクロールバーの描画
	ScrollBarDraw();

	//消耗品のときだけ
	if (m_nowCategory == ItemCategory_Consumable)
	{
		if (m_spContext->m_spUseTex)
		{
			//使用ボタン
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_spContext->m_spUseTex, m_spContext->m_useTex2DPos.x, m_spContext->m_useTex2DPos.y);
		}

		//クイックスロット
		if (m_selectPotionID == -999) { return; }
		std::shared_ptr<PlayerBase>spPlayer = m_spContext->m_wpPlayer.lock();
		if (!spPlayer) { return; }

		std::shared_ptr<PlayerInventory> spPlayerInventory = spPlayer->GetPlayerInventory();
		if (!spPlayerInventory) { return; }

		std::shared_ptr<KdTexture>spTex;

		if (spPlayerInventory->GetQuickSlotFlg(m_selectPotionID))
		{
			spTex = m_SlotUnregisterTex;
		}
		else
		{
			//上限かどうか
			if (spPlayerInventory->IsQuickSlotFull())
			{
				spTex = m_SlotFullTex;
			}
			else
			{
				spTex = m_SlotRegisterTex;
			}
		}


		if (spTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(spTex, m_SlotTexPos.x, m_SlotTexPos.y);
		}

		if (!spPlayerInventory->IsQuickSlotFull() || spPlayerInventory->GetQuickSlotFlg(m_selectPotionID))
		{
			Math::Vector2 keyPos = m_SlotTexPos;
			keyPos.x -= 60;
			Math::Vector2 keySiz = { 30,30 };
			KeyInfo::Instance().Draw2Dkey(m_SlotSetKey, keyPos, keySiz);
		}

		//数を出す
		const int listSiz = spPlayerInventory->GetPpotionsItemBarInventoryListSiz();
		const int listMaxSiz = spPlayerInventory->GetQUICK_SLOT_MAX();

		Math::Vector2 listSizPos = m_spContext->m_useTex2DPos;
		Math::Vector2 listMaxPos = m_spContext->m_useTex2DPos;

		listSizPos += {-10, 45};
		listMaxPos += {10, 41};
		float numSIz = 1.5;
		NumDraw::GetInstance().Draw(listSiz, RAligned, listSizPos, kWhiteColor, numSIz);

		//「/」
		if (m_spNumSlashTex)
		{
			Math::Rectangle rec = { 0,0,9,14 };
			float SlashSiz = numSIz;
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_spNumSlashTex, (int)m_spContext->m_useTex2DPos.x, (int)m_spContext->m_useTex2DPos.y + 43, (int)(9 * SlashSiz), (int)(14 * SlashSiz),&rec);
		}

		NumDraw::GetInstance().Draw(listMaxSiz, LAligned, listMaxPos, kWhiteColor, numSIz);

	}

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
		m_spContext->m_spSelectExplanationTex = m_itemIconInfo[0].m_spExplanationTex;
		m_spContext->m_spSelectNameTex = m_itemIconInfo[0].m_spNameTex;
		m_selectPotionID = m_itemIconInfo[0].m_itemID;
		return true;
	}

	m_spContext->m_spSelectNameTex = m_spContext->m_spNotSelectNameTex;
	return false;
}

void PotionInventoryUI::AddPotionTexInfo()
{
	std::shared_ptr<PlayerBase >spPlayer = m_spContext->m_wpPlayer.lock();
	if (!spPlayer) { return; }

	std::shared_ptr<PlayerInventory> spPlayerInventory = spPlayer->GetPlayerInventory();
	if (!spPlayerInventory) { return; }

	std::shared_ptr<PotionTexInfo> spTexInfo = m_spContext->m_wpPotionTexInfo.lock();

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
			itemIconInfo.m_itemID = item.m_ID;

			if (i >= m_slotCols)
			{
				j++;
				i = 0;
			}

			float x = m_slotBasePos.x + i * m_iconDimensions.m_w + i * m_slotSpacing;
			// スクロールした段数(m_scrollRow)だけ上にずらす
			float y = m_slotBasePos.y - (j - m_scrollRow) * (m_iconDimensions.m_h + m_slotSpacing);
			itemIconInfo.m_2DPos = { x,y };
			itemIconInfo.m_num = item.m_num;
			itemIconInfo.m_name = item.m_name;

			//---------- 種類ごとの画像 ----------
			if (spTexInfo)
			{
				switch (m_nowCategory)
				{
				case ItemCategory_Consumable:
					itemIconInfo.m_spIconTex = spTexInfo->GetPotionIcon(item.m_ID);
					itemIconInfo.m_spExplanationTex = spTexInfo->GetPotionExplanation(item.m_ID);
					itemIconInfo.m_spNameTex = spTexInfo->GetPotionName(item.m_ID);
					break;
				case ItemCategory_Material:
					itemIconInfo.m_spIconTex = spTexInfo->GetWeaponsStrengtheningItemIcon(item.m_ID);
					itemIconInfo.m_spExplanationTex = spTexInfo->GetWeaponsStrengtheningItemExplanation(item.m_ID);
					itemIconInfo.m_spNameTex = spTexInfo->GetWeaponsStrengtheningItemName(item.m_ID);
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
		if (m_selectPotionID == icon.m_itemID)
		{
			flg = true;
		}
	}

	if (!flg && m_itemIconInfo.size() != 0)
	{
		m_spContext->m_spSelectExplanationTex = m_itemIconInfo[0].m_spExplanationTex;
		m_selectPotionID = m_itemIconInfo[0].m_itemID;
		m_spContext->m_spSelectNameTex = m_itemIconInfo[0].m_spNameTex;
	}
}

void PotionInventoryUI::IconHit()
{
	if (m_itemIconInfo.size() <= 0)
	{
		m_spContext->m_spSelectExplanationTex = m_spContext->m_spNotSelectExplanationTex;
		m_spContext->m_spSelectNameTex = m_spContext->m_spNotSelectNameTex;
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

		float left = potion.m_2DPos.x - m_iconDimensions.m_w / 2;
		float right = potion.m_2DPos.x + m_iconDimensions.m_w / 2;
		float top = potion.m_2DPos.y + m_iconDimensions.m_h / 2;
		float bot = potion.m_2DPos.y - m_iconDimensions.m_h / 2;


		if (mousePos.x >= left && mousePos.x <= right &&
			mousePos.y >= bot && mousePos.y <= top)
		{
			MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
			potion.m_hit = true;
			if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
			{
				m_spContext->m_spSelectExplanationTex = potion.m_spExplanationTex;
				m_selectPotionID = potion.m_itemID;
				m_spContext->m_spSelectNameTex = potion.m_spNameTex;
			}
		}
		else
		{
			potion.m_hit = false;
		}
	}
}

void PotionInventoryUI::PotionUse()
{
	std::shared_ptr<PlayerBase>spPlayer = m_spContext->m_wpPlayer.lock();
	if (!spPlayer) { return; }

	std::shared_ptr<PlayerInventory> spPlayerInventory = spPlayer->GetPlayerInventory();
	if (!spPlayerInventory) { return; }

	//---------- 右の枠に出す個数（どの種類でも入れる） ----------
	switch (m_nowCategory)
	{
	case ItemCategory_Consumable:
		m_spContext->m_num = spPlayerInventory->GetPotionsInventoryNum(m_selectPotionID);
		break;
	case ItemCategory_Material:
		m_spContext->m_num = spPlayerInventory->GetWeaponsStrengtheningInventoryNum(m_selectPotionID);
		break;
	default:
		break;
	}

	//---------- 使用ボタンは消耗品のときだけ ----------
	if (m_nowCategory != ItemCategory_Consumable) { return; }

	const std::shared_ptr<KdTexture>& spUseTex = m_spContext->m_spUseTex;
	if (!spUseTex) { return; }
	const Math::Vector2& useTex2DPos = m_spContext->m_useTex2DPos;

	POINT mousePos = MouseInfo::Instance().m_windowPos;

	float left = useTex2DPos.x - spUseTex->GetWidth() / 2;
	float right = useTex2DPos.x + spUseTex->GetWidth() / 2;
	float top = useTex2DPos.y + spUseTex->GetHeight() / 2;
	float bot = useTex2DPos.y - spUseTex->GetHeight() / 2;

	if (mousePos.x >= left && mousePos.x <= right &&
		mousePos.y >= bot && mousePos.y <= top)
	{
		MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
		if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
		{
			if (m_itemIconInfo.size() <= 0 || m_selectPotionID == -999) { return; }
			std::shared_ptr<PotionUseController >spPotionUseController = m_spContext->m_wpPotionUseController.lock();
			if (!spPotionUseController) { return; }

			spPotionUseController->SetPlayer(spPlayer);
			spPotionUseController->PotionUse(m_selectPotionID);
			spPlayerInventory->UsePotionsInventory(m_selectPotionID);

			m_spContext->m_useEffectType = UseEffectType::Bounce;
			m_spContext->m_useEffectTimer = m_spContext->m_useEffectTime;
			m_spContext->m_useEffectItemID = m_selectPotionID;

			// 使った後の数にすぐ合わせる
			m_spContext->m_num = spPlayerInventory->GetPotionsInventoryNum(m_selectPotionID);
		}
	}
}

//=============================================
// 種類の切り替えボタン（消耗品／素材）
//=============================================

void PotionInventoryUI::CategoryUpdate()
{
	POINT mousePos = MouseInfo::Instance().m_windowPos;

	for (int i = 0; i < ItemCategory_Num; i++)
	{
		// ボタンの四角（m_categoryPos[i] が中心）
		float left = m_categoryPos[i].x - m_categorySize.x / 2;
		float right = m_categoryPos[i].x + m_categorySize.x / 2;
		float top = m_categoryPos[i].y + m_categorySize.y / 2;
		float bot = m_categoryPos[i].y - m_categorySize.y / 2;

		// マウスがボタンの上にあるか
		bool isOnButton = (mousePos.x >= left && mousePos.x <= right &&
			mousePos.y >= bot && mousePos.y <= top);

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
						std::shared_ptr<KdSoundInstance> spSE = KdAudioManager::Instance().Play("Asset/Sound/SE/UI/PageTurn.wav", false);
						if (spSE) { spSE->SetVolume(VOLInfo::Instance().GetMSSEVol() * 1.0f); }
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
		const std::shared_ptr<KdTexture>& spTex = (i == m_nowCategory) ? m_spCategorySelectTex[i] : m_spCategoryTex[i];
		if (!spTex) { continue; }

		KdShaderManager::Instance().m_spriteShader.DrawTex(spTex, m_categoryPos[i].x, m_categoryPos[i].y);
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
		m_selectPotionID = m_itemIconInfo[0].m_itemID;
		m_spContext->m_spSelectExplanationTex = m_itemIconInfo[0].m_spExplanationTex;
		m_spContext->m_spSelectNameTex = m_itemIconInfo[0].m_spNameTex;
	}
	else
	{
		// 何も持っていなければ「選択なし」の表示
		m_selectPotionID = -999;
		m_spContext->m_spSelectExplanationTex = m_spContext->m_spNotSelectExplanationTex;
		m_spContext->m_spSelectNameTex = m_spContext->m_spNotSelectNameTex;
	}
}

//=============================================
// スクロールバー
//=============================================

void PotionInventoryUI::ScrollBarDraw()
{
	// ドット絵がにじまないよう、ポイントフィルタで描く（NumDraw と同じ）
	KdShaderManager::Instance().m_spriteShader.Begin(false);

	//溝
	if (m_spScrollTrackTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spScrollTrackTex, m_scrollTrackPos.x, m_scrollTrackPos.y, m_scrollTrackSize.x, m_scrollTrackSize.y);
	}

	// マウスが乗っているときは明るい画像を使う
	const std::shared_ptr<KdTexture>& spTopTex = m_scrollThumbHoverFlg ? m_spScrollThumbTopHoverTex : m_spScrollThumbTopTex;
	const std::shared_ptr<KdTexture>& spMidTex = m_scrollThumbHoverFlg ? m_spScrollThumbMidHoverTex : m_spScrollThumbMidTex;
	const std::shared_ptr<KdTexture>& spBottomTex = m_scrollThumbHoverFlg ? m_spScrollThumbBottomHoverTex : m_spScrollThumbBottomTex;
	const std::shared_ptr<KdTexture>& spGripTex = m_scrollThumbHoverFlg ? m_spScrollThumbGripHoverTex : m_spScrollThumbGripTex;

	// つまみの上端・下端の y（y は上がプラス）
	const float thumbTop = m_scrollThumbPos.y + m_scrollThumbSize.y / 2;
	const float thumbBottom = m_scrollThumbPos.y - m_scrollThumbSize.y / 2;

	//つまみ(上の端)
	if (spTopTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(spTopTex, m_scrollThumbPos.x, thumbTop - m_scrollThumbCapH / 2, m_scrollThumbSize.x, m_scrollThumbCapH);
	}

	//つまみ(真ん中) … 上の端と下端を除いた長さに伸ばす
	const float midH = m_scrollThumbSize.y - m_scrollThumbCapH * 2;
	if (spMidTex && midH > 0.0f)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(spMidTex, m_scrollThumbPos.x, m_scrollThumbPos.y, m_scrollThumbSize.x, midH);
	}

	//つまみ(下の端)
	if (spBottomTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(spBottomTex, m_scrollThumbPos.x, thumbBottom + m_scrollThumbCapH / 2, m_scrollThumbSize.x, m_scrollThumbCapH);
	}

	//つまみ(中央の3本線) … 画像そのままの大きさで真ん中に
	if (spGripTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(spGripTex, m_scrollThumbPos.x, m_scrollThumbPos.y, spGripTex->GetWidth(), spGripTex->GetHeight());
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
	float thumbLeft = m_scrollThumbPos.x - m_scrollThumbSize.x / 2;
	float thumbRight = m_scrollThumbPos.x + m_scrollThumbSize.x / 2;
	float thumbTop = m_scrollThumbPos.y + m_scrollThumbSize.y / 2;
	float thumbBot = m_scrollThumbPos.y - m_scrollThumbSize.y / 2;

	// 溝の四角
	float trackLeft = m_scrollTrackPos.x - m_scrollTrackSize.x / 2;
	float trackRight = m_scrollTrackPos.x + m_scrollTrackSize.x / 2;


	float trackTop = m_scrollTrackPos.y + m_scrollTrackSize.y / 2 - m_scrollTrackInset;
	float trackBot = m_scrollTrackPos.y - m_scrollTrackSize.y / 2 + m_scrollTrackInset;

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
			float thumbMoveTop = trackTop - m_scrollThumbSize.y / 2;	// 一番上までスクロールしたときの中心のy
			float thumbMoveBot = trackBot + m_scrollThumbSize.y / 2;	// 一番下までスクロールしたときの中心のy

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
	const float panelLeft = m_spContext->m_back1Tex2DPos.x - m_spContext->m_back1TexWidth / 2;
	const float panelRight = m_spContext->m_back1Tex2DPos.x + m_spContext->m_back1TexWidth / 2;
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
	m_scrollThumbSize.x = m_scrollTrackSize.x;

	// スクロールの必要がなければ、つまみは溝いっぱい
	if (maxRow == 0)
	{
		m_scrollThumbSize.y = m_scrollTrackSize.y - m_scrollTrackInset * 2;
		m_scrollThumbPos.y = m_scrollTrackPos.y;
		return;
	}



	//---------- つまみの長さ ----------
	// 溝の高さ × 見えている段数 ÷ 全部の段数（短すぎるとつかめないので下限あり）
	float innerH = m_scrollTrackSize.y - m_scrollTrackInset * 2;
	float thumbH = innerH * (float)m_slotRows / (float)totalRows;
	m_scrollThumbSize.y = std::clamp(thumbH, m_scrollThumbMinH, innerH);

	//---------- つまみの位置 ----------
	// つまみの中心が動ける範囲（溝からはみ出さないように、長さの半分だけ内側）
	float trackTop = m_scrollTrackPos.y + m_scrollTrackSize.y / 2 - m_scrollTrackInset;
	float trackBot = m_scrollTrackPos.y - m_scrollTrackSize.y / 2 + m_scrollTrackInset;

	float thumbMoveTop = trackTop - m_scrollThumbSize.y / 2;
	float thumbMoveBot = trackBot + m_scrollThumbSize.y / 2;

	// 今の段が、一番上(0)〜一番下(maxRow)のどこか → 0〜1 の割合
	float ratio = (float)m_scrollRow / (float)maxRow;

	// 割合の位置につまみを置く（0 = 一番上、1 = 一番下）
	m_scrollThumbPos.y = thumbMoveTop - (thumbMoveTop - thumbMoveBot) * ratio;
}

//=============================================
// スクロールの状態
//=============================================

bool PotionInventoryUI::IsInSlotView(float _y) const
{
	// 見えている一番上の段・一番下の段の中心y（少し余裕を持たせて判定）
	const float step = m_iconDimensions.m_h + m_slotSpacing;
	const float topY = m_slotBasePos.y + step * 0.5f;
	const float botY = m_slotBasePos.y - (m_slotRows - 1) * step - step * 0.5f;
	return (_y <= topY && _y >= botY);
}
