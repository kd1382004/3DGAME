#include "EquipDisplay.h"
#include"../../../../Info/MouseInfo/MouseInfo.h"
#include"../../../../Info/KeyInfo/KeyInfo.h"
#include"../../../../Scene/SceneManager.h"

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
	for (int slot = 0; slot < m_visibleNum && !m_scrollDragFlg; slot++)
	{
		int listIndex = m_scrollIndex + slot;
		//武器が無い枠は選べない
		if (listIndex >= GetWeaponNum()) { break; }
		int weapon = m_ownedList[listIndex];

		if (MouseHit(GetSlotPos(slot), m_slotSiz, m_slotSiz))
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

	DrawTex(m_spTex[static_cast<int>(EquipTex::Title)], m_titlePos);

	DrawList();
	DrawScrollBar();
	DrawDetail();
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

	//---------- マウスと当たり判定の四角(画面の真ん中が0,0・上がプラス) ----------
	const POINT mousePos = MouseInfo::Instance().m_windowPos;
	const Math::Vector2 mouse = { static_cast<float>(mousePos.x), static_cast<float>(mousePos.y) };

	// レールの四角(ふちの内側)
	const float trackLeft  = m_trackLeft + m_trackInset;
	const float trackRight = m_trackLeft + m_trackW - m_trackInset;
	const float trackTop   = m_trackY + m_trackH / 2.0f;	// 上がプラスなので、上の辺の方が大きい
	const float trackBot   = m_trackY - m_trackH / 2.0f;

	// つまみの四角
	const float thumbLeft  = m_scrollThumbX;
	const float thumbRight = m_scrollThumbX + m_scrollThumbW;

	// マウスがつまみ・レールの上にあるか
	const bool isOnTrack = (mouse.x >= trackLeft && mouse.x <= trackRight &&
							 mouse.y <= trackTop  && mouse.y >= trackBot);

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
				//つまみより左 → 左へ
				m_scrollIndex -= m_visibleNum;
			}
			else
			{
				//つまみより右 → 右へ
				m_scrollIndex += m_visibleNum;
			}
		}
	}

	//---------- マウスホイール ----------
	// 武器一覧パネルの上にマウスがあるときだけ効かせる
	const bool isOnPanel = (std::abs(mouse.x - m_listPos.x) <= m_listW / 2.0f &&
							 std::abs(mouse.y - m_listPos.y) <= m_listH / 2.0f);

	if (isOnPanel && !m_scrollDragFlg)
	{
		int mouseWheelValue = MouseInfo::Instance().GetMouseWheelValue();
		//奥に回す → 左へ
		if (mouseWheelValue >= 1)
		{
			m_scrollIndex -= 1;
		}
		//手前に回す → 右へ
		else if (mouseWheelValue <= -1)
		{
			m_scrollIndex += 1;
		}
	}

	// つまみの色:マウスが乗っているか、ドラッグ中なら明るく
	m_scrollThumbHoverFlg = isOnThumb || m_scrollDragFlg;

	//---------- 範囲内に収める ----------
	m_scrollIndex = std::clamp(m_scrollIndex, 0, maxIndex);
}

void EquipDisplay::ScrollBarCalcThumb()
{
	const int   weaponNum = GetWeaponNum();
	const int   maxIndex  = GetMaxScrollIndex();
	// つまみが動ける範囲(レールのふちの内側)
	const float innerX = m_trackLeft + m_trackInset;
	const float innerW = m_trackW - m_trackInset * 2.0f;

	// スクロールの必要がなければ、つまみはレールいっぱい
	if (maxIndex == 0)
	{
		m_scrollThumbX = innerX;
		m_scrollThumbW = innerW;
		return;
	}

	//---------- つまみの長さ ----------
	// レールの長さ × 見えている数 ÷ 全部の数(短すぎるとつかめないので下限あり)
	const float thumbW = innerW * static_cast<float>(m_visibleNum) / static_cast<float>(weaponNum);
	m_scrollThumbW = std::clamp(thumbW, m_scrollThumbMinW, innerW);

	//---------- つまみの位置 ----------
	// 今の番号が、一番左(0)〜一番右(maxIndex)のどこか → 0〜1の割合
	const float ratio = static_cast<float>(m_scrollIndex) / static_cast<float>(maxIndex);

	// 左端が動ける範囲(innerX 〜 innerX + innerW - つまみの長さ)
	m_scrollThumbX = innerX + (innerW - m_scrollThumbW) * ratio;
}

int EquipDisplay::GetMaxScrollIndex() const
{
	return std::max(0, GetWeaponNum() - m_visibleNum);
}

Math::Vector2 EquipDisplay::GetSlotPos(int _slot) const
{
	return { m_slotPos.x + m_slotSpace * _slot, m_slotPos.y };
}

void EquipDisplay::DrawTex(const std::shared_ptr<KdTexture>& _spTex, const Math::Vector2& _pos, float _scale, const Math::Vector2& _pivot)
{
	if (!_spTex) { return; }
	DrawTexSize(_spTex, _pos, _spTex->GetWidth() * _scale, _spTex->GetHeight() * _scale, _pivot);
}

void EquipDisplay::DrawTexSize(const std::shared_ptr<KdTexture>& _spTex, const Math::Vector2& _pos, float _w, float _h, const Math::Vector2& _pivot)
{
	if (!_spTex) { return; }

	KdShaderManager::Instance().m_spriteShader.DrawTex(
		_spTex.get(),
		static_cast<int>(_pos.x), static_cast<int>(_pos.y),
		static_cast<int>(_w), static_cast<int>(_h),
		nullptr, &kWhiteColor, _pivot);
}

void EquipDisplay::DrawList()
{
	DrawTex(m_spTex[static_cast<int>(EquipTex::ListPanel)], m_listPos);

	const int equipWeapon = SceneManager::Instance().GetPlayerWeaponInfo();

	for (int slot = 0; slot < m_visibleNum; slot++)
	{
		// 持っている武器だけを出す(足りない枠は何も描かない)
		int listIndex = m_scrollIndex + slot;
		if (listIndex >= GetWeaponNum()) { break; }

		int weapon = m_ownedList[listIndex];
		const Math::Vector2 slotPos = GetSlotPos(slot);

		DrawTex(m_spWeaponTex[weapon][static_cast<int>(WeaponTex::Icon)], slotPos);

		// 装備中 → 金の枠 + 「装備中」
		if (weapon == equipWeapon)
		{
			DrawTex(m_spTex[static_cast<int>(EquipTex::SelectFrame)], slotPos);
			DrawTex(m_spTex[static_cast<int>(EquipTex::Equipped)], { slotPos.x, m_equippedY });
		}
		// カーソルが乗っている → 銀の枠
		else if (weapon == m_hoverIndex)
		{
			DrawTex(m_spTex[static_cast<int>(EquipTex::HoverFrame)], slotPos);
		}
	}
}

void EquipDisplay::DrawScrollBar()
{
	// ドット絵がにじまないよう、ポイントフィルタで描く(PotionInventoryUIと同じ)
	KdShaderManager::Instance().m_spriteShader.Begin(false);

	// レール(左端を基準に描く)
	DrawTexSize(m_spTex[static_cast<int>(EquipTex::ScrollTrack)], { m_trackLeft, m_trackY }, m_trackW, m_trackH, m_pivotLeft);

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
		const float thumbY = m_trackY;

		// 左端
		DrawTexSize(spL, { m_scrollThumbX, thumbY }, capLW, m_trackH, m_pivotLeft);

		// 中央 … 左端と右端を除いた長さに伸ばす
		const float midW = m_scrollThumbW - capLW - capRW;
		if (midW > 0.0f)
		{
			DrawTexSize(spM, { m_scrollThumbX + capLW, thumbY }, midW, m_trackH, m_pivotLeft);
		}

		// 右端
		DrawTexSize(spR, { m_scrollThumbX + m_scrollThumbW - capRW, thumbY }, capRW, m_trackH, m_pivotLeft);

		// 持ち手 … 画像そのままの大きさで真ん中に
		DrawTex(spG, { m_scrollThumbX + m_scrollThumbW / 2.0f, thumbY });
	}

	KdShaderManager::Instance().m_spriteShader.End();
}

void EquipDisplay::DrawDetail()
{
	DrawTex(m_spTex[static_cast<int>(EquipTex::DetailPanel)], m_detailPos);

	// カーソルが乗っている武器、無ければ装備中の武器を出す
	int weapon = (m_hoverIndex >= 0) ? m_hoverIndex : SceneManager::Instance().GetPlayerWeaponInfo();
	if (weapon < 0 || weapon >= PlayerWeaponInfoSize) { return; }

	// 名前と説明は武器ごとに大きさが違うので、左上を基準に描く
	DrawTex(m_spWeaponTex[weapon][static_cast<int>(WeaponTex::Icon)],        m_detailIconPos, 2.0f);
	DrawTex(m_spWeaponTex[weapon][static_cast<int>(WeaponTex::Name)],        m_detailNamePos, 1.0f, m_pivotLeftTop);
	DrawTex(m_spWeaponTex[weapon][static_cast<int>(WeaponTex::Explanation)], m_detailExplPos, 2.0f, m_pivotLeftTop);

	// 操作説明(パネルの下)
	DrawTex(m_spTex[static_cast<int>(EquipTex::ClickEquip)], m_clickEquipPos);
}
