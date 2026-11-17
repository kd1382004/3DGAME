#include "EquipDisplay.h"
#include"../../../../Info/MouseInfo/MouseInfo.h"
#include"../../../../Info/KeyInfo/KeyInfo.h"
#include"../../../../Scene/SceneManager.h"

namespace
{
	// 画面サイズ(座標変換用)
	constexpr float kScreenW = 1280.0f;
	constexpr float kScreenH = 720.0f;

	// レイアウト(画面左上が0,0・下がプラス)
	constexpr float kTitleX = 548.0f, kTitleY = 30.0f;		// 「装備」
	constexpr float kListX = 550.0f, kListY = 110.0f;		// 武器一覧パネルの左上
	constexpr float kDetailX = 550.0f, kDetailY = 325.0f;	// 武器詳細パネルの左上

	// 武器一覧
	constexpr int   kVisibleNum = 8;		// 一度に見える武器の数
	constexpr float kSlotSiz = 64.0f;		// アイコンの大きさ
	constexpr float kSlotSpace = 80.0f;		// アイコン同士の間隔
	constexpr float kSlotOffsetX = 34.0f;	// パネル左上から1つ目のアイコンまで
	constexpr float kSlotOffsetY = 40.0f;

	// スクロールバー
	constexpr float kTrackOffsetX = 30.0f;	// パネル左上からレールまで
	constexpr float kTrackOffsetY = 162.0f;
	constexpr float kTrackW = 640.0f;		// レールの長さ
	constexpr float kTrackH = 16.0f;		// レールの高さ
	constexpr float kTrackInset = 2.0f;		// レールのふちの太さ(つまみが動けるのはこの分だけ内側)

	// 武器一覧パネルの大きさ(ホイールを効かせる範囲)
	constexpr float kListW = 700.0f;
	constexpr float kListH = 200.0f;
}

void EquipDisplay::Init()
{
	LoadTex();

	KeyInfo::Instance().SetKeyValid(VK_LBUTTON);

	// つまみの最小の長さ = 左端 + 持ち手 + 右端
	const auto& spL = m_spTex[static_cast<int>(EquipTex::ThumbLeft)];
	const auto& spR = m_spTex[static_cast<int>(EquipTex::ThumbRight)];
	const auto& spG = m_spTex[static_cast<int>(EquipTex::ThumbGrip)];
	if (spL && spR && spG)
	{
		m_scrollThumbMinW = static_cast<float>(spL->GetWidth() + spG->GetWidth() + spR->GetWidth());
	}

	m_scrollIndex = 0;
	ScrollBarCalcThumb();
}

void EquipDisplay::SetOwnedWeapons(const std::vector<bool>& _owned)
{
	m_ownedList.clear();
	for (int id = 0; id < PlayerWeaponInfoSize; id++)
	{
		if (id < static_cast<int>(_owned.size()) && _owned[id])
		{
			m_ownedList.push_back(id);
		}
	}

	// 装備中の武器を持っていなければ、持っている最初の武器に替える
	// (持っていない武器でダンジョンに入れないようにする)
	const int equip = SceneManager::Instance().GetPlayerWeaponInfo();
	if (std::find(m_ownedList.begin(), m_ownedList.end(), equip) == m_ownedList.end() &&
		!m_ownedList.empty())
	{
		SceneManager::Instance().SetPlayerWeaponInfo(static_cast<PlayerWeaponInfo>(m_ownedList.front()));
	}

	m_scrollIndex = 0;
}

void EquipDisplay::Update()
{
	// 開いているタブでなければ何もしない
	if (!m_activeFlg) { return; }

	// スクロールバー(先に処理して、ドラッグ中は武器を選ばないようにする)
	ScrollBarCalcThumb();
	ScrollBarUpdate();
	ScrollBarCalcThumb();

	// カーソルが乗っている武器を探す
	m_hoverIndex = -1;
	for (int slot = 0; slot < kVisibleNum && !m_scrollDragFlg; slot++)
	{
		int listIndex = m_scrollIndex + slot;
		if (listIndex >= GetWeaponNum()) { break; }	// 武器が無い枠は選べない
		int weapon = m_ownedList[listIndex];

		Math::Vector2 tl = GetSlotTopLeft(slot);

		// 左上基準(下がプラス) → 中心基準(上がプラス)に変換して判定
		Math::Vector2 center = { tl.x + kSlotSiz / 2.0f - kScreenW / 2.0f,
								 kScreenH / 2.0f - (tl.y + kSlotSiz / 2.0f) };

		if (MouseHit(center, kSlotSiz, kSlotSiz))
		{
			m_hoverIndex = weapon;

			if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
			{
				SceneManager::Instance().SetPlayerWeaponInfo(static_cast<PlayerWeaponInfo>(weapon));
			}

			break;
		}
	}

}

void EquipDisplay::DrawSprite()
{
	// 開いているタブでなければ何もしない
	if (!m_activeFlg) { return; }

	DrawTL(m_spTex[static_cast<int>(EquipTex::Title)], kTitleX, kTitleY);

	DrawList();
	DrawScrollBar();
	DrawDetail();
}

bool EquipDisplay::MouseHit(Math::Vector2 _pos, float _w, float _h)
{
	// _pos … 中心座標(画面中央が0,0・上がプラス)
	float left   = _pos.x - _w / 2.0f;
	float right  = _pos.x + _w / 2.0f;
	float top    = _pos.y - _h / 2.0f;
	float bottom = _pos.y + _h / 2.0f;

	float mx = static_cast<float>(MouseInfo::Instance().m_windowPos.x);
	float my = static_cast<float>(MouseInfo::Instance().m_windowPos.y);

	if (left <= mx && mx <= right &&
		top <= my && my <= bottom)
	{
		MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
		return true;
	}

	return false;
}

void EquipDisplay::LoadTex()
{
	for (int i = 0; i < static_cast<int>(EquipTex::TexSize); i++)
	{
		LoadOneTex(m_spTex[i], m_texPath[i]);
	}

	for (int w = 0; w < PlayerWeaponInfoSize; w++)
	{
		for (int i = 0; i < static_cast<int>(WeaponTex::WeaponTexSize); i++)
		{
			LoadOneTex(m_spWeaponTex[w][i], m_weaponTexPath[w][i]);
		}
	}
}

void EquipDisplay::LoadOneTex(std::shared_ptr<KdTexture>& _spTex, const std::string& _path)
{
	if (_spTex) { return; }

	_spTex = std::make_shared<KdTexture>();
	if (!_spTex->Load(_path))
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] EquipDisplayの画像が読み込めません : %s\n"), _path.c_str());
	}
}

Math::Vector2 EquipDisplay::GetSlotTopLeft(int _slot) const
{
	return { kListX + kSlotOffsetX + kSlotSpace * _slot,
			 kListY + kSlotOffsetY };
}

void EquipDisplay::DrawTL(const std::shared_ptr<KdTexture>& _spTex, float _x, float _y, float _scale, const Math::Color& _color)
{
	if (!_spTex) { return; }

	float w = _spTex->GetWidth() * _scale;
	float h = _spTex->GetHeight() * _scale;

	// 左上基準(下がプラス) → 中心基準(上がプラス)
	float cx = _x + w / 2.0f - kScreenW / 2.0f;
	float cy = kScreenH / 2.0f - (_y + h / 2.0f);

	KdShaderManager::Instance().m_spriteShader.DrawTex(
		_spTex.get(),
		static_cast<int>(cx), static_cast<int>(cy),
		static_cast<int>(w), static_cast<int>(h),
		nullptr, &_color);
}

void EquipDisplay::DrawTLSize(const std::shared_ptr<KdTexture>& _spTex, float _x, float _y, float _w, float _h)
{
	if (!_spTex) { return; }

	// 左上基準(下がプラス) → 中心基準(上がプラス)
	float cx = _x + _w / 2.0f - kScreenW / 2.0f;
	float cy = kScreenH / 2.0f - (_y + _h / 2.0f);

	KdShaderManager::Instance().m_spriteShader.DrawTex(
		_spTex.get(),
		static_cast<int>(cx), static_cast<int>(cy),
		static_cast<int>(_w), static_cast<int>(_h),
		nullptr, &kWhiteColor);
}

void EquipDisplay::DrawList()
{
	DrawTL(m_spTex[static_cast<int>(EquipTex::ListPanel)], kListX, kListY);

	const int equipWeapon = SceneManager::Instance().GetPlayerWeaponInfo();

	for (int slot = 0; slot < kVisibleNum; slot++)
	{
		// 持っている武器だけを出す(足りない枠は何も描かない)
		int listIndex = m_scrollIndex + slot;
		if (listIndex >= GetWeaponNum()) { break; }

		int weapon = m_ownedList[listIndex];
		Math::Vector2 tl = GetSlotTopLeft(slot);

		DrawTL(m_spWeaponTex[weapon][static_cast<int>(WeaponTex::Icon)], tl.x, tl.y);

		// 装備中 → 金の枠 + 「装備中」
		if (weapon == equipWeapon)
		{
			DrawTL(m_spTex[static_cast<int>(EquipTex::SelectFrame)], tl.x - 2.0f, tl.y - 2.0f);

			const auto& spLabel = m_spTex[static_cast<int>(EquipTex::Equipped)];
			if (spLabel)
			{
				DrawTL(spLabel, tl.x + kSlotSiz / 2.0f - spLabel->GetWidth() / 2.0f, tl.y + kSlotSiz + 2.0f);
			}
		}
		// カーソルが乗っている → 銀の枠
		else if (weapon == m_hoverIndex)
		{
			DrawTL(m_spTex[static_cast<int>(EquipTex::HoverFrame)], tl.x - 2.0f, tl.y - 2.0f);
		}
	}
}

void EquipDisplay::DrawScrollBar()
{
	// ドット絵がにじまないよう、ポイントフィルタで描く(PotionInventoryUIと同じ)
	KdShaderManager::Instance().m_spriteShader.Begin(false);

	const float trackX = kListX + kTrackOffsetX;
	const float trackY = kListY + kTrackOffsetY;

	// レール
	DrawTLSize(m_spTex[static_cast<int>(EquipTex::ScrollTrack)], trackX, trackY, kTrackW, kTrackH);

	// マウスが乗っているときは明るい画像を使う
	const bool hover = m_scrollThumbHoverFlg;
	const auto& spL = m_spTex[static_cast<int>(hover ? EquipTex::ThumbLeftHover  : EquipTex::ThumbLeft)];
	const auto& spM = m_spTex[static_cast<int>(hover ? EquipTex::ThumbMidHover   : EquipTex::ThumbMid)];
	const auto& spR = m_spTex[static_cast<int>(hover ? EquipTex::ThumbRightHover : EquipTex::ThumbRight)];
	const auto& spG = m_spTex[static_cast<int>(hover ? EquipTex::ThumbGripHover  : EquipTex::ThumbGrip)];

	if (spL && spM && spR && spG)
	{
		const float capLW = static_cast<float>(spL->GetWidth());
		const float capRW = static_cast<float>(spR->GetWidth());
		const float thumbY = trackY;

		// 左端
		DrawTLSize(spL, m_scrollThumbX, thumbY, capLW, kTrackH);

		// 中央 … 左端と右端を除いた長さに伸ばす
		const float midW = m_scrollThumbW - capLW - capRW;
		if (midW > 0.0f)
		{
			DrawTLSize(spM, m_scrollThumbX + capLW, thumbY, midW, kTrackH);
		}

		// 右端
		DrawTLSize(spR, m_scrollThumbX + m_scrollThumbW - capRW, thumbY, capRW, kTrackH);

		// 持ち手 … 画像そのままの大きさで真ん中に
		DrawTL(spG, m_scrollThumbX + m_scrollThumbW / 2.0f - spG->GetWidth() / 2.0f, thumbY);
	}

	KdShaderManager::Instance().m_spriteShader.End();
}

int EquipDisplay::GetMaxScrollIndex() const
{
	return std::max(0, GetWeaponNum() - kVisibleNum);
}

Math::Vector2 EquipDisplay::GetMouseTL() const
{
	// 画面中央が0,0・上がプラス → 画面左上が0,0・下がプラス
	const POINT mousePos = MouseInfo::Instance().m_windowPos;
	return { static_cast<float>(mousePos.x) + kScreenW / 2.0f,
			 kScreenH / 2.0f - static_cast<float>(mousePos.y) };
}

void EquipDisplay::ScrollBarCalcThumb()
{
	const int   weaponNum = GetWeaponNum();
	const int   maxIndex  = GetMaxScrollIndex();
	const float trackX    = kListX + kTrackOffsetX;

	// つまみが動ける範囲(レールのふちの内側)
	const float innerX = trackX + kTrackInset;
	const float innerW = kTrackW - kTrackInset * 2.0f;

	// スクロールの必要がなければ、つまみはレールいっぱい
	if (maxIndex == 0)
	{
		m_scrollThumbX = innerX;
		m_scrollThumbW = innerW;
		return;
	}

	//---------- つまみの長さ ----------
	// レールの長さ × 見えている数 ÷ 全部の数(短すぎるとつかめないので下限あり)
	const float thumbW = innerW * static_cast<float>(kVisibleNum) / static_cast<float>(weaponNum);
	m_scrollThumbW = std::clamp(thumbW, m_scrollThumbMinW, innerW);

	//---------- つまみの位置 ----------
	// 今の番号が、一番左(0)〜一番右(maxIndex)のどこか → 0〜1の割合
	const float ratio = static_cast<float>(m_scrollIndex) / static_cast<float>(maxIndex);

	// 左端が動ける範囲(innerX 〜 innerX + innerW - つまみの長さ)
	m_scrollThumbX = innerX + (innerW - m_scrollThumbW) * ratio;
}

void EquipDisplay::ScrollBarUpdate()
{
	//---------- スクロールできるか ----------
	const int maxIndex = GetMaxScrollIndex();

	// スクロールの必要がなければ何もしない
	if (maxIndex == 0)
	{
		m_scrollIndex = 0;
		m_scrollDragFlg = false;
		m_scrollThumbHoverFlg = false;
		return;
	}

	//---------- マウスと当たり判定の四角(画面左上が0,0・下がプラス) ----------
	const Math::Vector2 mouse = GetMouseTL();

	const float trackX = kListX + kTrackOffsetX;
	const float trackY = kListY + kTrackOffsetY;

	// レールの四角(ふちの内側)
	const float trackLeft  = trackX + kTrackInset;
	const float trackRight = trackX + kTrackW - kTrackInset;
	const float trackTop   = trackY;
	const float trackBot   = trackY + kTrackH;

	// つまみの四角
	const float thumbLeft  = m_scrollThumbX;
	const float thumbRight = m_scrollThumbX + m_scrollThumbW;

	// マウスがつまみ・レールの上にあるか
	const bool isOnTrack = (mouse.x >= trackLeft && mouse.x <= trackRight &&
							 mouse.y >= trackTop  && mouse.y <= trackBot);

	const bool isOnThumb = (isOnTrack && mouse.x >= thumbLeft && mouse.x <= thumbRight);

	//---------- ドラッグ開始(つまみの上で押した瞬間) ----------
	if (isOnThumb && !m_scrollDragFlg)
	{
		MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);

		if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
		{
			m_scrollDragFlg = true;
			// つかんだ位置とつまみの左端のずれ(ドラッグ中は変えない)
			m_scrollDragOffsetX = mouse.x - m_scrollThumbX;
		}
	}

	//---------- ドラッグ中 ----------
	if (m_scrollDragFlg)
	{
		// 左ボタンが押されたままか(離したらドラッグ終了)
		if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
		{
			m_scrollDragFlg = false;
		}
		else
		{
			MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);

			// つまみの左端が動ける範囲
			const float moveLeft  = trackLeft;
			const float moveRight = trackRight - m_scrollThumbW;

			// つまみの左端が来てほしいx(つかんだ位置のずれを引く)
			const float wantX = mouse.x - m_scrollDragOffsetX;

			// 動ける範囲のどこにいるかを0〜1の割合にする(0 = 一番左、1 = 一番右)
			const float moveRange = moveRight - moveLeft;
			float ratio = 0.0f;
			if (moveRange > 0.0f)
			{
				ratio = (wantX - moveLeft) / moveRange;
			}
			ratio = std::clamp(ratio, 0.0f, 1.0f);

			// 割合を番号に直す(四捨五入)
			m_scrollIndex = static_cast<int>(std::round(ratio * maxIndex));
		}
	}

	//---------- レールをクリック(つまみ以外のところ) → 1画面分動く ----------
	else if (isOnTrack && !isOnThumb)
	{
		MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);

		if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
		{
			if (mouse.x < m_scrollThumbX)
			{
				m_scrollIndex -= kVisibleNum;	// つまみより左 → 左へ
			}
			else
			{
				m_scrollIndex += kVisibleNum;	// つまみより右 → 右へ
			}
		}
	}

	//---------- マウスホイール ----------
	// 武器一覧パネルの上にマウスがあるときだけ効かせる
	const bool isOnPanel = (mouse.x >= kListX && mouse.x <= kListX + kListW &&
							 mouse.y >= kListY && mouse.y <= kListY + kListH);

	if (isOnPanel && !m_scrollDragFlg)
	{
		int mouseWheelValue = MouseInfo::Instance().GetMouseWheelValue();
		if (mouseWheelValue >= 1) { m_scrollIndex -= 1; }		// 奥に回す → 左へ
		else if (mouseWheelValue <= -1) { m_scrollIndex += 1; }	// 手前に回す → 右へ
	}

	// つまみの色:マウスが乗っているか、ドラッグ中なら明るく
	m_scrollThumbHoverFlg = isOnThumb || m_scrollDragFlg;

	//---------- 範囲内に収める ----------
	m_scrollIndex = std::clamp(m_scrollIndex, 0, maxIndex);
}

void EquipDisplay::DrawDetail()
{
	DrawTL(m_spTex[static_cast<int>(EquipTex::DetailPanel)], kDetailX, kDetailY);

	// カーソルが乗っている武器、無ければ装備中の武器を出す
	int weapon = (m_hoverIndex >= 0) ? m_hoverIndex : SceneManager::Instance().GetPlayerWeaponInfo();
	if (weapon < 0 || weapon >= PlayerWeaponInfoSize) { return; }

	DrawTL(m_spWeaponTex[weapon][static_cast<int>(WeaponTex::Icon)],        kDetailX + 40.0f,  kDetailY + 40.0f, 2.0f);
	DrawTL(m_spWeaponTex[weapon][static_cast<int>(WeaponTex::Name)],        kDetailX + 30.0f,  kDetailY + 188.0f);
	DrawTL(m_spWeaponTex[weapon][static_cast<int>(WeaponTex::Explanation)], kDetailX + 220.0f, kDetailY + 50.0f, 2.0f);

	// 操作説明(パネルの下)
	DrawTL(m_spTex[static_cast<int>(EquipTex::ClickEquip)], kDetailX + 10.0f, kDetailY + 340.0f);
}
