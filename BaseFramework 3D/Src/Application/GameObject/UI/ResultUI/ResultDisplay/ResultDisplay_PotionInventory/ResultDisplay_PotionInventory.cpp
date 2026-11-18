#include "ResultDisplay_PotionInventory.h"
#include"../../../../../Info/MouseInfo/MouseInfo.h"
#include"../../../../../Info/KeyInfo/KeyInfo.h"
#include"../../../../Character/Player/PlayerInventory/PlayerInventory.h"
#include"../../../../Potions/PotionTexInfo/PotionTexInfo.h"
#include"../../../../../Info/NumDraw/NumDraw.h"

void ResultDisplay_PotionInventory::Init()
{
	m_iconDimensions = { 40,40 };

	auto loadTex = [](std::shared_ptr<KdTexture>& _spTex, const std::string& _path)
		{
			if (_spTex) { return; }	// 読み込み済みなら何もしない
			_spTex = std::make_shared<KdTexture>();
			_spTex->Load(_path);
		};

	//---------- 背景・タイトル ----------
	loadTex(m_spPotionSelectTex, "Asset/Textures/GameUI/Item/Potion/PotionSelect.png");
	loadTex(m_spPotionTitleTex, "Asset/Textures/GameUI/Result/ResultDisplay_PotionInventory/PotionTiyel.png");
	loadTex(m_spPotionBackTex, "Asset/Textures/GameUI/Result/ResultDisplay_PotionInventory/BagBackTex2.png");
	loadTex(m_spPotionBack2Tex, "Asset/Textures/GameUI/Result/ResultDisplay_PotionInventory/back.png");

	m_potionTitleTexPos = { 0,250 };
	m_potionBackTexPos = { 200,-70 };
	m_potionBack2TexPos = { -150,m_potionBackTexPos.y };

	//---------- スクロールバーの画像（インベントリと同じもの） ----------
	const std::string scrollPath = "Asset/Textures/GameUI/Item/PlayerInventory/Scroll/";

	loadTex(m_spScrollTrackTex, scrollPath + "ScrollTrack.png");

	loadTex(m_spScrollThumbTopTex, scrollPath + "ScrollThumbTop.png");
	loadTex(m_spScrollThumbMidTex, scrollPath + "ScrollThumbMid.png");
	loadTex(m_spScrollThumbBottomTex, scrollPath + "ScrollThumbBottom.png");
	loadTex(m_spScrollThumbGripTex, scrollPath + "ScrollThumbGrip.png");

	loadTex(m_spScrollThumbTopHoverTex, scrollPath + "ScrollThumbTop_Hover.png");
	loadTex(m_spScrollThumbMidHoverTex, scrollPath + "ScrollThumbMid_Hover.png");
	loadTex(m_spScrollThumbBottomHoverTex, scrollPath + "ScrollThumbBottom_Hover.png");
	loadTex(m_spScrollThumbGripHoverTex, scrollPath + "ScrollThumbGrip_Hover.png");

	//---------- 溝（レール） ----------
	// 横幅は画像から、縦はマスの段数ぶん
	const float step = m_iconDimensions.m_h + m_slotSpacing;
	const float gridTop = m_slotBasePos.y + m_iconDimensions.m_h / 2;
	const float gridBottom = gridTop - (m_slotRows * step - m_slotSpacing);
	const float gridRight = m_slotBasePos.x - m_iconDimensions.m_w / 2 + m_slotCols * (m_iconDimensions.m_w + m_slotSpacing) - m_slotSpacing;

	const float back2Width = m_spPotionBack2Tex ? (float)m_spPotionBack2Tex->GetWidth() : 360.0f;
	const float panelInnerRight = m_potionBack2TexPos.x + back2Width / 2 - 26.0f;	// 鋲の内側の右端

	m_scrollTrackSiz.x = m_spScrollTrackTex ? (float)m_spScrollTrackTex->GetWidth() : 16.0f;
	m_scrollTrackSiz.y = gridTop - gridBottom;

	// マスの右端と、鋲の内側の右端の真ん中に置く
	m_scrollTrackPos.x = (gridRight + panelInnerRight) / 2;
	m_scrollTrackPos.y = (gridTop + gridBottom) / 2;

	//---------- つまみ ----------
	m_scrollThumbSiz.x = m_spScrollThumbTopTex ? (float)m_spScrollThumbTopTex->GetWidth() : m_scrollTrackSiz.x;
	m_scrollThumbCapH = m_spScrollThumbTopTex ? (float)m_spScrollThumbTopTex->GetHeight() : 6.0f;

	// 一番短くても「上の端＋3本線＋下の端」が入る長さ
	const float gripH = m_spScrollThumbGripTex ? (float)m_spScrollThumbGripTex->GetHeight() : 7.0f;
	m_scrollThumbMinH = m_scrollThumbCapH * 2 + gripH + 2.0f;

	// 最初は溝の内側いっぱい（ScrollBarCalcThumb で毎フレーム決め直す）
	m_scrollThumbPos = m_scrollTrackPos;
	m_scrollThumbSiz.y = m_scrollTrackSiz.y - m_scrollTrackInset * 2;
}

void ResultDisplay_PotionInventory::Update()
{
	// ① マウスホイール・ドラッグで「何段目から表示するか」を決める
	ScrollBarUpdate();

	// ② その段数を使って、アイコンの位置を決める（スクロール分ずらす）
	UpdateIconPos();

	// ③ つまみの長さと位置を計算
	ScrollBarCalcThumb();

	// ④ スクロール後の位置で当たり判定
	IconHit();
}

void ResultDisplay_PotionInventory::PreDraw()
{}

void ResultDisplay_PotionInventory::DrawSprite()
{
	if (m_spPotionTitleTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spPotionTitleTex, m_potionTitleTexPos.x, m_potionTitleTexPos.y);
	}

	if (m_spPotionBackTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spPotionBackTex, m_potionBackTexPos.x, m_potionBackTexPos.y);
	}
	if (m_spPotionBack2Tex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spPotionBack2Tex, m_potionBack2TexPos.x, m_potionBack2TexPos.y);
	}

	//---------- 空きマス ----------
	if (m_spPotionSelectTex)
	{
		const float step = m_iconDimensions.m_w + m_slotSpacing;	// アイコン ＋ すき間
		Math::Color slotCol = { 0, 0, 0, 0.25f };				// うっすら暗いマス

		for (int y = 0; y < m_slotRows; y++)
		{
			for (int x = 0; x < m_slotCols; x++)
			{
				KdShaderManager::Instance().m_spriteShader.DrawTex(
					m_spPotionSelectTex,
					m_slotBasePos.x + x * step, m_slotBasePos.y - y * step,
					m_iconDimensions.m_w, m_iconDimensions.m_h, nullptr, &slotCol);
			}
		}
	}

	//---------- アイコン ----------
	for (const auto& tex : m_itemIconInfo)
	{
		// スクロールで見えない段のアイコンは描かない
		if (!IsInSlotView(tex.m_2DPos.y)) { continue; }

		KdShaderManager::Instance().m_spriteShader.DrawTex(tex.m_spIconTex, tex.m_2DPos.x, tex.m_2DPos.y, m_iconDimensions.m_w, m_iconDimensions.m_h);

		if (tex.m_itemID == m_selectPotionID && m_spPotionSelectTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_spPotionSelectTex, tex.m_2DPos.x, tex.m_2DPos.y, m_iconDimensions.m_w, m_iconDimensions.m_h);
		}
	}

	//---------- 右の枠（選択中の名前・説明・個数） ----------
	// スクロールで選択中のアイコンが見えなくても表示は残す
	for (const auto& tex : m_itemIconInfo)
	{
		if (tex.m_itemID != m_selectPotionID) { continue; }

		if (tex.m_spExplanationTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(tex.m_spExplanationTex, 200, -95);
		}
		if (tex.m_spNameTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(tex.m_spNameTex, 200, 85);
		}
		// 所持数（右の枠の上の暗い帯に収まる大きさ・位置）
		NumDraw::GetInstance().Draw(tex.m_num, RAligned, { 285,11 }, kWhiteColor, 2.5f, false, -1, { 170.0f, 0.0f });	// 右の枠の幅に収める
		break;
	}

	//スクロールバーの描画
	ScrollBarDraw();
}

void ResultDisplay_PotionInventory::AddPotionTexInfo(std::vector<Inventory> _potions)
{
	m_itemIconInfo.clear();

	// 新しく入れ直したら一番上から表示
	m_scrollRow = 0;
	m_scrollDragFlg = false;

	// 画像を取り出すだけなので1つ作って使い回す
	std::shared_ptr<PotionTexInfo>spPotionTexInfo = std::make_shared<PotionTexInfo>();

	int index = 0;
	for (const Inventory& potion : _potions)
	{
		if (potion.m_num <= 0) { continue; }

		ItemIconInfo itemIconInfo;
		itemIconInfo.m_itemID = potion.m_ID;
		itemIconInfo.m_num = potion.m_num;
		itemIconInfo.m_name = potion.m_name;
		itemIconInfo.m_index = index;

		if (spPotionTexInfo)
		{
			itemIconInfo.m_spIconTex = spPotionTexInfo->GetPotionIcon(itemIconInfo.m_itemID);
			itemIconInfo.m_spExplanationTex = spPotionTexInfo->GetPotionExplanation(itemIconInfo.m_itemID);
			itemIconInfo.m_spNameTex = spPotionTexInfo->GetPotionName(itemIconInfo.m_itemID);
		}

		m_itemIconInfo.push_back(itemIconInfo);
		index++;
	}

	// 位置を決める
	UpdateIconPos();

	if (m_itemIconInfo.size() > 0)
	{
		m_selectPotionID = m_itemIconInfo[0].m_itemID;
	}
	else
	{
		m_selectPotionID = -999;
	}
}

void ResultDisplay_PotionInventory::IconHit()
{
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
				m_selectPotionID = potion.m_itemID;
			}
		}
		else
		{
			potion.m_hit = false;
		}
	}
}

void ResultDisplay_PotionInventory::UpdateIconPos()
{
	const float step = m_iconDimensions.m_w + m_slotSpacing;

	for (auto& icon : m_itemIconInfo)
	{
		const int i = icon.m_index % m_slotCols;	// 横（何列目）
		const int j = icon.m_index / m_slotCols;	// 縦（何段目）

		float x = m_slotBasePos.x + i * step;
		// スクロールした段数(m_scrollRow)だけ上にずらす
		float y = m_slotBasePos.y - (j - m_scrollRow) * step;
		icon.m_2DPos = { x,y };
	}
}

void ResultDisplay_PotionInventory::ScrollBarDraw()
{
	// ドット絵がにじまないよう、ポイントフィルタで描く（NumDraw と同じ）
	KdShaderManager::Instance().m_spriteShader.Begin(false);

	//溝
	if (m_spScrollTrackTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spScrollTrackTex, m_scrollTrackPos.x, m_scrollTrackPos.y, m_scrollTrackSiz.x, m_scrollTrackSiz.y);
	}

	// マウスが乗っているときは明るい画像を使う
	const std::shared_ptr<KdTexture>& spTopTex = m_scrollThumbHoverFlg ? m_spScrollThumbTopHoverTex : m_spScrollThumbTopTex;
	const std::shared_ptr<KdTexture>& spMidTex = m_scrollThumbHoverFlg ? m_spScrollThumbMidHoverTex : m_spScrollThumbMidTex;
	const std::shared_ptr<KdTexture>& spBottomTex = m_scrollThumbHoverFlg ? m_spScrollThumbBottomHoverTex : m_spScrollThumbBottomTex;
	const std::shared_ptr<KdTexture>& spGripTex = m_scrollThumbHoverFlg ? m_spScrollThumbGripHoverTex : m_spScrollThumbGripTex;

	// つまみの上端・下端の y（y は上がプラス）
	const float thumbTop = m_scrollThumbPos.y + m_scrollThumbSiz.y / 2;
	const float thumbBottom = m_scrollThumbPos.y - m_scrollThumbSiz.y / 2;

	//つまみ(上の端)
	if (spTopTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(spTopTex, m_scrollThumbPos.x, thumbTop - m_scrollThumbCapH / 2, m_scrollThumbSiz.x, m_scrollThumbCapH);
	}

	//つまみ(真ん中) … 上の端と下端を除いた長さに伸ばす
	const float midH = m_scrollThumbSiz.y - m_scrollThumbCapH * 2;
	if (spMidTex && midH > 0.0f)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(spMidTex, m_scrollThumbPos.x, m_scrollThumbPos.y, m_scrollThumbSiz.x, midH);
	}

	//つまみ(下の端)
	if (spBottomTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(spBottomTex, m_scrollThumbPos.x, thumbBottom + m_scrollThumbCapH / 2, m_scrollThumbSiz.x, m_scrollThumbCapH);
	}

	//つまみ(中央の3本線) … 画像そのままの大きさで真ん中に
	if (spGripTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(spGripTex, m_scrollThumbPos.x, m_scrollThumbPos.y, spGripTex->GetWidth(), spGripTex->GetHeight());
	}

	KdShaderManager::Instance().m_spriteShader.End();
}

void ResultDisplay_PotionInventory::ScrollBarUpdate()
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
			float thumbMoveTop = trackTop - m_scrollThumbSiz.y / 2;
			float thumbMoveBot = trackBot + m_scrollThumbSiz.y / 2;

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
	// 左の枠（アイテム一覧）の上にマウスがあるときだけホイールを効かせる
	const float back2Width = m_spPotionBack2Tex ? (float)m_spPotionBack2Tex->GetWidth() : 360.0f;
	const float back2Height = m_spPotionBack2Tex ? (float)m_spPotionBack2Tex->GetHeight() : 368.0f;
	const float panelLeft = m_potionBack2TexPos.x - back2Width / 2;
	const float panelRight = m_potionBack2TexPos.x + back2Width / 2;
	const float panelTop = m_potionBack2TexPos.y + back2Height / 2;
	const float panelBot = m_potionBack2TexPos.y - back2Height / 2;
	bool isOnPanel = (mousePos.x >= panelLeft && mousePos.x <= panelRight &&
		mousePos.y >= panelBot && mousePos.y <= panelTop);

	if (isOnPanel && !m_scrollDragFlg)
	{
		int mouseWheelValue = MouseInfo::Instance().GetMouseWheelValue();
		if (mouseWheelValue >= 1) { m_scrollRow -= 1; }
		else if (mouseWheelValue <= -1) { m_scrollRow += 1; }
	}

	// つまみの色：マウスが乗っているか、ドラッグ中なら明るく
	m_scrollThumbHoverFlg = isOnThumb || m_scrollDragFlg;

	//----------範囲内に収める ----------
	m_scrollRow = std::clamp(m_scrollRow, 0, maxRow);
}

void ResultDisplay_PotionInventory::ScrollBarCalcThumb()
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
	float trackTop = m_scrollTrackPos.y + m_scrollTrackSiz.y / 2 - m_scrollTrackInset;
	float trackBot = m_scrollTrackPos.y - m_scrollTrackSiz.y / 2 + m_scrollTrackInset;

	float thumbMoveTop = trackTop - m_scrollThumbSiz.y / 2;
	float thumbMoveBot = trackBot + m_scrollThumbSiz.y / 2;

	// 今の段が、一番上(0)〜一番下(maxRow)のどこか → 0〜1 の割合
	float ratio = (float)m_scrollRow / (float)maxRow;

	// 割合の位置につまみを置く（0 = 一番上、1 = 一番下）
	m_scrollThumbPos.y = thumbMoveTop - (thumbMoveTop - thumbMoveBot) * ratio;
}

bool ResultDisplay_PotionInventory::IsInSlotView(float _y) const
{
	// 見えている一番上の段・一番下の段の中心y（少し余裕を持たせて判定）
	const float step = m_iconDimensions.m_h + m_slotSpacing;
	const float topY = m_slotBasePos.y + step * 0.5f;
	const float botY = m_slotBasePos.y - (m_slotRows - 1) * step - step * 0.5f;
	return (_y <= topY && _y >= botY);
}
