#include "ShopDisplay.h"
#include"../../../../Info/MouseInfo/MouseInfo.h"
#include"../../../../Info/KeyInfo/KeyInfo.h"
#include"../../../../Scene/SceneManager.h"
#include"../../../../Scene/PrepareScene/PrepareScene.h"

namespace
{
	/////////////////////////////////////////////////////////////////
	// レイアウト
	// 座標はフレームワークと同じ(画面の真ん中が0,0・右と上がプラス)
	// 特に書いていなければ、画像の「真ん中」の位置
	/////////////////////////////////////////////////////////////////

	//---------- 見出し・所持金 ----------
	const Math::Vector2 kTitlePos = { 38.0f, 314.0f };	// 「ショップ」(260×80)
	const Math::Vector2 kMoneyPanelPos = { 485.0f, 314.0f };	// 所持金パネル(250×64)
	const Math::Vector2 kMoneyLabelPos = { 430.0f, 314.0f };	// 「所持金」(116×56)
	const Math::Vector2 kMoneyNumPos = { 490.0f, 314.0f };	// 所持金の数字(左端)。「G」はこの右

	//---------- タブ(150×58) ----------
	const Math::Vector2 kTabPos = { -15.0f, 245.0f };	// 1つ目のタブ
	constexpr float kTabSpace = 158.0f;					// 右へずらす量(150 + すき間8)
	constexpr float kTabW = 150.0f, kTabH = 58.0f;

	//---------- 商品一覧(700×200) ----------
	const Math::Vector2 kListPos = { 260.0f, 110.0f };
	constexpr float kListW = 700.0f, kListH = 200.0f;
	const Math::Vector2 kSlotPos = { -24.0f, 138.0f };	// 1つ目の枠(64×64)
	constexpr float kSlotSiz = 64.0f, kSlotSpace = 96.0f;
	constexpr int   kListVisibleNum = 7;
	constexpr float kSlotUnderY = 84.0f;				// 枠の下の文字(売切れ・所持済み・未入荷)の真ん中のy
	constexpr float kSlotPriceY = 82.0f;				// 枠の下の値段の真ん中のy

	//---------- 商品詳細(700×240) ----------
	const Math::Vector2 kDetailPos = { 260.0f, -118.0f };
	const Math::Vector2 kDetailIconPos = { 4.0f,  -88.0f };	// 大きいアイコン(真ん中)
	const Math::Vector2 kDetailNamePos = { -70.0f, -158.0f };	// 名前(左上)
	const Math::Vector2 kDetailExplPos = { 90.0f,  -34.0f };	// 説明(左上)
	const Math::Vector2 kPivotLeftTop = { 0.0f, 1.0f };		// 左上を基準に描く

	//---------- 所持・値段の欄(334×206) ----------
	const Math::Vector2 kInfoPos = { 433.0f, -118.0f };
	constexpr float kInfoLabelX = 318.0f;		// 項目(「所持」など 76×52)の真ん中のx
	constexpr float kInfoValueX = 362.0f;		// 値の左端のx
	constexpr float kInfoButtonX = 536.0f;		// 買う・売るボタン(92×38)の真ん中のx
	constexpr float kInfoRow0Y = -49.0f, kInfoRowSpace = 44.0f;	// 1行目の真ん中のy と、下の行へずらす量
	constexpr float kButtonW = 92.0f, kButtonH = 38.0f, kSmallSiz = 36.0f;

	constexpr float kHaveTextX = 400.0f;	// 「あり」「なし」(76×48)の真ん中のx
	constexpr float kDashX = 394.0f;	// 「ー」(48×34)の真ん中のx

	// 1行目の右側の「在庫 〇」
	constexpr float kStockLabelX     = 494.0f;	// 「在庫」(76×52)の真ん中のx
	constexpr float kStockNumRightX  = 582.0f;	// 在庫の数の右端(買う・売るボタンの右端にそろえる)
	constexpr float kStockNumMaxW    = 50.0f;	// 在庫の数の最大幅(「在庫」の右 〜 欄の右端)
	constexpr float kInfoHaveNumMaxW = 54.0f;	// 1行目の所持の数の最大幅(「個」が「在庫」にかぶらないように。3桁まで)

	// 買値・売値の数字の右端(4桁 + カンマ「2,900」の右端にそろえる)。「G」はこのすぐ右
	constexpr float kInfoPriceRightX = 452.0f;
	constexpr float kInfoGoldX = 476.0f;	// 「G」(40×42)の真ん中のx

	// 数量の行の -・+ ボタン(36×36)の真ん中のx。数字は2つの間の真ん中
	constexpr float kCountMinusX = 374.0f;
	constexpr float kCountPlusX = 464.0f;
	constexpr float kCountNumX = 419.0f;

	//---------- 持ち物(700×104) ----------
	const Math::Vector2 kHavePos = { 260.0f, -298.0f };
	constexpr float kHaveW = 700.0f, kHaveH = 104.0f;
	const Math::Vector2 kHaveLabelPos = { -22.0f, -286.0f };	// 「持ち物」(104×52)
	constexpr float kHaveItemX = 60.0f, kHaveItemY = -284.0f;	// 1つ目のアイコン(40×40)の真ん中
	constexpr float kHaveSpace = 130.0f;
	constexpr int   kHaveVisibleNum = 4;

	//---------- スクロールバー ----------
	constexpr float kTrackH = 16.0f;
	constexpr float kTrackInset = 2.0f;

	//---------- 数字が超えてはいけない横幅(桁が増えても隣にかぶらないように) ----------
	constexpr float kMoneyNumMaxW = 72.0f;	// 所持金(「所持金」と「G」の間)
	constexpr float kListPriceMaxW = 70.0f;	// 一覧のアイコンの下の値段(枠の間隔96 - 「G」)
	constexpr float kInfoValueMaxW = 96.0f;	// 右の欄の値
	constexpr float kCountNumMaxW = 40.0f;	// 数量(-と+の間)
	constexpr float kHaveNumMaxW = 44.0f;	// 持ち物の個数(「×」の右 〜 次の持ち物)

	// 数字1文字の大きさ(NumDrawの画像)
	constexpr float kNumGlyphW = 9.0f;

	// 1行目から _i 行下の行の真ん中のy
	float InfoRowY(int _i) { return kInfoRow0Y - kInfoRowSpace * _i; }

	// ポーションの画像フォルダ名(PotionsTypeの順)
	const std::string kPotionName[PotionsTypeMax] =
	{
		"InstantHealPotion", "AttackBaseUPPotion", "AttackBoostPotion", "InstantStaminaHealPotion",
		"StaminaMaxUPPotion", "StaminaRegenBaseUPPotion", "StaminaRegenBoostPotion",
	};

	// 武器の画像フォルダ名(PlayerWeaponInfoの順)
	const std::string kWeaponName[PlayerWeaponInfoSize] = { "Dagger", "Sword" };
}

/////////////////////////////////////////////////////////////////////
// 初期化・データ
/////////////////////////////////////////////////////////////////////
void ShopDisplay::Init()
{
	LoadTex();
	LoadShopData();

	KeyInfo::Instance().SetKeyValid(VK_LBUTTON);

	// つまみの最小の長さ = 左端 + 持ち手 + 右端
	const auto& spL = m_spTex[static_cast<int>(ShopTex::ThumbLeft)];
	const auto& spR = m_spTex[static_cast<int>(ShopTex::ThumbRight)];
	const auto& spG = m_spTex[static_cast<int>(ShopTex::ThumbGrip)];
	if (spL && spR && spG)
	{
		m_thumbMinW = static_cast<float>(spL->GetWidth() + spG->GetWidth() + spR->GetWidth());
	}

	// スクロールの場所(レールの左端のx・真ん中のy・長さ)
	m_listScroll.trackLeft = -60.0f; m_listScroll.trackY = 40.0f; m_listScroll.trackW = 640.0f;
	m_listScroll.visibleNum = kListVisibleNum;
	m_haveScroll.trackLeft = 40.0f; m_haveScroll.trackY = -324.0f; m_haveScroll.trackW = 520.0f;
	m_haveScroll.visibleNum = kHaveVisibleNum;
}

void ShopDisplay::LoadShopData()
{
	auto readJson = [](const std::string& _path) -> nlohmann::json
		{
			std::ifstream file(_path);
			if (!file)
			{
				KdDebugGUI::Instance().AddLog(U8("[Error] ShopDisplayのファイルが見つかりません : %s\n"), _path.c_str());
				return nlohmann::json::object();
			}
			nlohmann::json data = nlohmann::json::parse(file, nullptr, false);
			if (data.is_discarded()) { return nlohmann::json::object(); }
			return data;
		};

	const nlohmann::json master = readJson(m_shopMasterPath);
	const nlohmann::json stock = readJson(m_shopStockPath);
	const nlohmann::json inventory = readJson(m_inventoryPath);

	// 値段(マスター)
	auto readMaster = [&](const char* _key, ShopItem* _items, int _num)
		{
			if (!master.contains(_key)) { return; }
			for (const auto& item : master[_key])
			{
				int id = item.value("id", -1);
				if (id < 0 || id >= _num) { continue; }
				_items[id].buyPrice = item.value("buyPrice", 0);
				_items[id].sellPrice = item.value("sellPrice", 0);
			}
		};
	readMaster("potions", m_potionItem, PotionsTypeMax);
	readMaster("materials", m_materialItem, kMaterialNum);
	readMaster("weapons", m_weaponItem, PlayerWeaponInfoSize);

	// 在庫
	auto readStock = [&](const char* _key, ShopItem* _items, int _num)
		{
			if (!stock.contains(_key)) { return; }
			for (const auto& item : stock[_key])
			{
				int id = item.value("id", -1);
				if (id < 0 || id >= _num) { continue; }
				_items[id].stock = item.value("stock", 0);
			}
		};
	readStock("potions", m_potionItem, PotionsTypeMax);
	readStock("materials", m_materialItem, kMaterialNum);
	readStock("weapons", m_weaponItem, PlayerWeaponInfoSize);

	// 持ち物
	auto readHave = [&](const char* _key, int* _have, int _num)
		{
			for (int i = 0; i < _num; i++) { _have[i] = 0; }
			if (!inventory.contains(_key)) { return; }
			for (const auto& item : inventory[_key])
			{
				int id = item.value("id", -1);
				if (id < 0 || id >= _num) { continue; }
				_have[id] = item.value("num", 0);
			}
		};
	readHave("potions", m_potionHave, PotionsTypeMax);
	readHave("weaponStrengtheningMaterials", m_materialHave, kMaterialNum);

	// 武器(無ければダガー(最初の武器)だけ持っている)
	for (int i = 0; i < PlayerWeaponInfoSize; i++) { m_weaponOwned[i] = false; }
	m_weaponOwned[PlayerWeaponInfo_Dagger] = true;
	if (inventory.contains("weapons"))
	{
		for (const auto& item : inventory["weapons"])
		{
			int id = item.value("id", -1);
			if (id < 0 || id >= PlayerWeaponInfoSize) { continue; }
			m_weaponOwned[id] = item.value("owned", false);
		}
	}

	m_money = inventory.value("money", 0L);
}

int ShopDisplay::GetListNum() const
{
	switch (m_tab)
	{
	case ShopTab::Consumable: return PotionsTypeMax;
	case ShopTab::Material:   return kMaterialNum;
	case ShopTab::Weapon:     return PlayerWeaponInfoSize;
	default:                  return 0;
	}
}

std::vector<ShopDisplay::HaveItem> ShopDisplay::GetHaveList() const
{
	std::vector<HaveItem> list;
	switch (m_tab)
	{
	case ShopTab::Consumable:
		for (int i = 0; i < PotionsTypeMax; i++)
		{
			if (m_potionHave[i] > 0) { list.push_back({ m_spPotionTex[i][static_cast<int>(ItemTex::Icon)], m_potionHave[i] }); }
		}
		break;
	case ShopTab::Weapon:
		for (int i = 0; i < PlayerWeaponInfoSize; i++)
		{
			if (m_weaponOwned[i]) { list.push_back({ m_spWeaponTex[i][static_cast<int>(ItemTex::Icon)], 1 }); }
		}
		break;
	case ShopTab::Material:
		for (int i = 0; i < kMaterialNum; i++)
		{
			if (m_materialHave[i] > 0) { list.push_back({ m_spMaterialTex[i][static_cast<int>(ItemTex::Icon)], m_materialHave[i] }); }
		}
		break;
	default:
		break;
	}
	return list;
}

const ShopDisplay::ShopItem& ShopDisplay::GetStackItem(int _index) const
{
	return (m_tab == ShopTab::Material) ? m_materialItem[_index] : m_potionItem[_index];
}

int ShopDisplay::GetStackHave(int _index) const
{
	return (m_tab == ShopTab::Material) ? m_materialHave[_index] : m_potionHave[_index];
}

const std::shared_ptr<KdTexture>& ShopDisplay::GetStackTex(int _index, ItemTex _tex) const
{
	return (m_tab == ShopTab::Material) ? m_spMaterialTex[_index][static_cast<int>(_tex)]
		: m_spPotionTex[_index][static_cast<int>(_tex)];
}

/////////////////////////////////////////////////////////////////////
// 更新
/////////////////////////////////////////////////////////////////////
void ShopDisplay::Update()
{
	// 開いているタブでなければ何もしない
	if (!m_activeFlg) { return; }

	// 左クリックはこのフレームで1回だけ取る(タブ・一覧・スクロールで取り合わないように)
	m_clickFlg = KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true);

	//---------- タブ ----------
	for (int t = 0; t < static_cast<int>(ShopTab::TabSize); t++)
	{
		const Math::Vector2 tabPos = { kTabPos.x + kTabSpace * t, kTabPos.y };
		if (MouseHit(tabPos, kTabW, kTabH))
		{
			MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
			if (m_clickFlg && m_tab != static_cast<ShopTab>(t))
			{
				m_tab = static_cast<ShopTab>(t);
				m_selectIndex = 0;
				m_count = 1;
				m_listScroll.index = 0;
				m_haveScroll.index = 0;
				m_clickFlg = false;
			}
		}
	}

	//---------- スクロール ----------
	const int listNum = GetListNum();
	const int haveNum = static_cast<int>(GetHaveList().size());

	ScrollCalc(m_listScroll, listNum);
	ScrollUpdate(m_listScroll, listNum, kListPos, kListW, kListH);
	ScrollCalc(m_listScroll, listNum);

	ScrollCalc(m_haveScroll, haveNum);
	ScrollUpdate(m_haveScroll, haveNum, kHavePos, kHaveW, kHaveH);
	ScrollCalc(m_haveScroll, haveNum);

	//---------- 商品一覧(カーソル・選択) ----------
	m_hoverIndex = -1;
	if (!m_listScroll.dragFlg)
	{
		for (int slot = 0; slot < kListVisibleNum; slot++)
		{
			int index = m_listScroll.index + slot;
			if (index >= listNum) { break; }

			const Math::Vector2 slotPos = { kSlotPos.x + kSlotSpace * slot, kSlotPos.y };
			if (MouseHit(slotPos, kSlotSiz, kSlotSiz))
			{
				MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
				m_hoverIndex = index;
				if (m_clickFlg)
				{
					m_selectIndex = index;
					m_count = 1;
					m_clickFlg = false;
				}
				break;
			}
		}
	}

	//---------- 数量の＋／－(消耗品・素材だけ) ----------
	if (IsStackTab() && m_selectIndex < GetListNum())
	{
		const float rowY = InfoRowY(1);
		if (m_clickFlg && MouseHit({ kCountMinusX, rowY }, kSmallSiz, kSmallSiz)) { m_count--; m_clickFlg = false; }
		if (m_clickFlg && MouseHit({ kCountPlusX,  rowY }, kSmallSiz, kSmallSiz)) { m_count++; m_clickFlg = false; }
		m_count = std::clamp(m_count, 1, 99);
	}

	// 買う・売る
	BuySellUpdate();
}

bool ShopDisplay::MouseHit(const Math::Vector2& _pos, float _w, float _h) const
{
	// マウスもフレームワークと同じ座標(画面の真ん中が0,0・上がプラス)
	const POINT mousePos = MouseInfo::Instance().m_windowPos;
	const float mx = static_cast<float>(mousePos.x);
	const float my = static_cast<float>(mousePos.y);

	return (std::abs(mx - _pos.x) <= _w / 2.0f && std::abs(my - _pos.y) <= _h / 2.0f);
}

/////////////////////////////////////////////////////////////////////
// 横スクロール(EquipDisplayと同じ動き)
/////////////////////////////////////////////////////////////////////
void ShopDisplay::ScrollCalc(HScroll& _scroll, int _total)
{
	const int   maxIndex = std::max(0, _total - _scroll.visibleNum);
	const float innerX = _scroll.trackLeft + kTrackInset;
	const float innerW = _scroll.trackW - kTrackInset * 2.0f;

	// スクロールの必要がなければ、つまみはレールいっぱい
	if (maxIndex == 0)
	{
		_scroll.index = 0;
		_scroll.thumbX = innerX;
		_scroll.thumbW = innerW;
		return;
	}

	// 長さ = レールの長さ × 見えている数 ÷ 全部の数(下限あり)
	const float thumbW = innerW * static_cast<float>(_scroll.visibleNum) / static_cast<float>(_total);
	_scroll.thumbW = std::clamp(thumbW, m_thumbMinW, innerW);

	// 位置 = 今の番号の割合
	_scroll.index = std::clamp(_scroll.index, 0, maxIndex);
	const float ratio = static_cast<float>(_scroll.index) / static_cast<float>(maxIndex);
	_scroll.thumbX = innerX + (innerW - _scroll.thumbW) * ratio;
}

void ShopDisplay::ScrollUpdate(HScroll& _scroll, int _total, const Math::Vector2& _panelPos, float _panelW, float _panelH)
{
	const int maxIndex = std::max(0, _total - _scroll.visibleNum);

	// スクロールの必要がなければ何もしない
	if (maxIndex == 0)
	{
		_scroll.index = 0;
		_scroll.dragFlg = false;
		_scroll.hoverFlg = false;
		return;
	}

	const float mx = static_cast<float>(MouseInfo::Instance().m_windowPos.x);

	const float trackLeft = _scroll.trackLeft + kTrackInset;
	const float trackRight = _scroll.trackLeft + _scroll.trackW - kTrackInset;

	const bool isOnTrack = MouseHit({ (trackLeft + trackRight) / 2.0f, _scroll.trackY }, trackRight - trackLeft, kTrackH);
	const bool isOnThumb = isOnTrack && mx >= _scroll.thumbX && mx <= _scroll.thumbX + _scroll.thumbW;

	//---------- ドラッグ開始 ----------
	if (isOnThumb && !_scroll.dragFlg)
	{
		MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
		if (m_clickFlg)
		{
			_scroll.dragFlg = true;
			_scroll.dragOffsetX = mx - _scroll.thumbX;
			m_clickFlg = false;
		}
	}

	//---------- ドラッグ中 ----------
	if (_scroll.dragFlg)
	{
		if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
		{
			_scroll.dragFlg = false;
		}
		else
		{
			MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);

			const float moveRange = (trackRight - _scroll.thumbW) - trackLeft;
			float ratio = 0.0f;
			if (moveRange > 0.0f)
			{
				ratio = (mx - _scroll.dragOffsetX - trackLeft) / moveRange;
			}
			ratio = std::clamp(ratio, 0.0f, 1.0f);
			_scroll.index = static_cast<int>(std::round(ratio * maxIndex));
		}
	}
	//---------- レールをクリック → 1画面分動く ----------
	else if (isOnTrack && !isOnThumb)
	{
		MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
		if (m_clickFlg)
		{
			_scroll.index += (mx < _scroll.thumbX) ? -_scroll.visibleNum : _scroll.visibleNum;
			m_clickFlg = false;
		}
	}

	//---------- マウスホイール(そのパネルの上にあるときだけ) ----------
	if (!_scroll.dragFlg && MouseHit(_panelPos, _panelW, _panelH))
	{
		int wheel = MouseInfo::Instance().GetMouseWheelValue();
		if (wheel >= 1) { _scroll.index -= 1; }
		else if (wheel <= -1) { _scroll.index += 1; }
	}

	_scroll.hoverFlg = isOnThumb || _scroll.dragFlg;
	_scroll.index = std::clamp(_scroll.index, 0, maxIndex);
}

void ShopDisplay::ScrollDraw(const HScroll& _scroll, ShopTex _track)
{
	// 左端を基準に、横に並べて描く
	const Math::Vector2 pivotLeft = { 0.0f, 0.5f };

	DrawTexSize(m_spTex[static_cast<int>(_track)], { _scroll.trackLeft, _scroll.trackY }, _scroll.trackW, kTrackH, pivotLeft);

	const bool hover = _scroll.hoverFlg;
	const auto& spL = m_spTex[static_cast<int>(hover ? ShopTex::ThumbLeftHover : ShopTex::ThumbLeft)];
	const auto& spM = m_spTex[static_cast<int>(hover ? ShopTex::ThumbMidHover : ShopTex::ThumbMid)];
	const auto& spR = m_spTex[static_cast<int>(hover ? ShopTex::ThumbRightHover : ShopTex::ThumbRight)];
	const auto& spG = m_spTex[static_cast<int>(hover ? ShopTex::ThumbGripHover : ShopTex::ThumbGrip)];
	if (!spL || !spM || !spR || !spG) { return; }

	const float lw = static_cast<float>(spL->GetWidth());
	const float rw = static_cast<float>(spR->GetWidth());
	const float y = _scroll.trackY;

	DrawTexSize(spL, { _scroll.thumbX, y }, lw, kTrackH, pivotLeft);
	const float midW = _scroll.thumbW - lw - rw;
	if (midW > 0.0f) { DrawTexSize(spM, { _scroll.thumbX + lw, y }, midW, kTrackH, pivotLeft); }
	DrawTexSize(spR, { _scroll.thumbX + _scroll.thumbW - rw, y }, rw, kTrackH, pivotLeft);

	// 持ち手はつまみの真ん中
	DrawTex(spG, { _scroll.thumbX + _scroll.thumbW / 2.0f, y });
}

/////////////////////////////////////////////////////////////////////
// 描画
/////////////////////////////////////////////////////////////////////
void ShopDisplay::DrawSprite()
{
	// 開いているタブでなければ何もしない
	if (!m_activeFlg) { return; }

	// ドット絵がにじまないよう、ポイントフィルタで描く
	KdShaderManager::Instance().m_spriteShader.Begin(false);

	DrawHeader();
	DrawList();
	DrawDetail();
	DrawInventory();

	KdShaderManager::Instance().m_spriteShader.End();
}

void ShopDisplay::DrawHeader()
{
	DrawTex(m_spTex[static_cast<int>(ShopTex::Title)], kTitlePos);

	// 所持金(数字の右に「G」)
	DrawTex(m_spTex[static_cast<int>(ShopTex::MoneyPanel)], kMoneyPanelPos);
	DrawTex(m_spTex[static_cast<int>(ShopTex::MoneyLabel)], kMoneyLabelPos);
	DrawNum(m_money, LAligned, kMoneyNumPos, 2.0f, true, kMoneyNumMaxW);
	const float numW = GetNumWidth(m_money, 2.0f, true, kMoneyNumMaxW);
	DrawTex(m_spTex[static_cast<int>(ShopTex::Gold)], { kMoneyNumPos.x + numW + 24.0f, kMoneyNumPos.y - 1.0f });

	// タブ
	for (int t = 0; t < static_cast<int>(ShopTab::TabSize); t++)
	{
		const int select = (m_tab == static_cast<ShopTab>(t)) ? 1 : 0;
		DrawTex(m_spTabTex[t][select], { kTabPos.x + kTabSpace * t, kTabPos.y });
	}
}

void ShopDisplay::DrawList()
{
	DrawTex(m_spTex[static_cast<int>(ShopTex::ListPanel)], kListPos);

	// 値段 + G(半分の大きさ)を、枠の真ん中にそろえて描く
	auto drawPrice = [&](int _price, float _slotX)
		{
			const float numW = GetNumWidth(_price, 2.0f, false, kListPriceMaxW);
			const float left = _slotX - (numW + 24.0f) / 2.0f;
			DrawNum(_price, LAligned, { left, kSlotPriceY }, 2.0f, false, kListPriceMaxW);
			DrawTex(m_spTex[static_cast<int>(ShopTex::Gold)], { left + numW + 12.0f, kSlotPriceY + 2.0f }, 0.5f);
		};

	const int listNum = GetListNum();
	for (int slot = 0; slot < kListVisibleNum; slot++)
	{
		const int index = m_listScroll.index + slot;
		if (index >= listNum) { break; }

		const Math::Vector2 slotPos = { kSlotPos.x + kSlotSpace * slot, kSlotPos.y };
		const Math::Vector2 underPos = { slotPos.x, kSlotUnderY };

		if (IsStackTab())
		{
			// 消耗品・素材のアイコンは40px → 1.5倍(60px)
			DrawTex(GetStackTex(index, ItemTex::Icon), slotPos, 1.5f);

			if (GetStackItem(index).stock <= 0)
			{
				DrawTex(m_spTex[static_cast<int>(ShopTex::SoldOut)], underPos);
			}
			else
			{
				drawPrice(GetStackItem(index).buyPrice, slotPos.x);
			}
		}
		else if (m_tab == ShopTab::Weapon)
		{
			DrawTex(m_spWeaponTex[index][static_cast<int>(ItemTex::Icon)], slotPos);

			// 持っている → 所持済み / 在庫なし → 未入荷 / それ以外 → 値段
			if (m_weaponOwned[index] || m_weaponItem[index].stock <= 0)
			{
				DrawTex(m_spTex[static_cast<int>(m_weaponOwned[index] ? ShopTex::Owned : ShopTex::NotStocked)], underPos);
			}
			else
			{
				drawPrice(m_weaponItem[index].buyPrice, slotPos.x);
			}
		}

		// 枠(選択中は金、カーソルは銀)
		if (index == m_selectIndex)
		{
			DrawTex(m_spTex[static_cast<int>(ShopTex::SelectFrame)], slotPos);
		}
		else if (index == m_hoverIndex)
		{
			DrawTex(m_spTex[static_cast<int>(ShopTex::HoverFrame)], slotPos);
		}
	}

	ScrollDraw(m_listScroll, ShopTex::ListTrack);
}

void ShopDisplay::DrawDetail()
{
	DrawTex(m_spTex[static_cast<int>(ShopTex::DetailPanel)], kDetailPos);

	auto label = [&](ShopTex _tex, int _i) { DrawTex(m_spTex[static_cast<int>(_tex)], { kInfoLabelX, InfoRowY(_i) }); };

	if (IsStackTab() && m_selectIndex < GetListNum())
	{
		const int id = m_selectIndex;
		const ShopItem& item = GetStackItem(id);
		const int have = GetStackHave(id);

		// アイコン(40px → 3倍)・名前・説明(名前と説明は長さが違うので左上基準)
		DrawTex(GetStackTex(id, ItemTex::Icon), kDetailIconPos, 3.0f);
		DrawTex(GetStackTex(id, ItemTex::Name), kDetailNamePos, 1.0f, kPivotLeftTop);
		DrawTex(GetStackTex(id, ItemTex::Explanation), kDetailExplPos, 1.0f, kPivotLeftTop);

		DrawTex(m_spTex[static_cast<int>(ShopTex::PriceInfoPanel)], kInfoPos);

		// 所持 〇 個
		label(ShopTex::HaveLabel, 0);
		DrawNum(have, LAligned, { kInfoValueX, InfoRowY(0) }, 2.0f, false, kInfoHaveNumMaxW);
		const float haveW = GetNumWidth(have, 2.0f, false, kInfoHaveNumMaxW);
		DrawTex(m_spTex[static_cast<int>(ShopTex::Count)], { kInfoValueX + haveW + 28.0f, InfoRowY(0) + 1.0f });

		// 在庫 〇(同じ行の右側)
		DrawStock(item.stock);

		// 数量 － 〇 ＋(数字は-と+の真ん中)
		label(ShopTex::CountLabel, 1);
		DrawButton(ShopButton::Minus, { kCountMinusX, InfoRowY(1) }, m_count > 1);
		DrawNum(m_count, CAligned, { kCountNumX, InfoRowY(1) }, 2.0f, false, kCountNumMaxW);
		DrawButton(ShopButton::Plus, { kCountPlusX, InfoRowY(1) }, true);

		// 買値・売値 … 数量分の合計
		const long buyTotal = static_cast<long>(item.buyPrice) * m_count;
		const long sellTotal = static_cast<long>(item.sellPrice) * m_count;

		// 買える:在庫が数量以上 かつ お金が足りる / 売れる:持っている数が数量以上
		const bool canBuy = (item.stock >= m_count) && (m_money >= buyTotal);
		const bool canSell = (have >= m_count);

		// 買値 〇 G [買う]
		label(ShopTex::BuyPriceLabel, 2);
		DrawPrice(buyTotal, InfoRowY(2));
		DrawButton(ShopButton::Buy, { kInfoButtonX, InfoRowY(2) }, canBuy);

		// 売値 〇 G [売る]
		label(ShopTex::SellPriceLabel, 3);
		DrawPrice(sellTotal, InfoRowY(3));
		DrawButton(ShopButton::Sell, { kInfoButtonX, InfoRowY(3) }, canSell);
	}
	else if (m_tab == ShopTab::Weapon && m_selectIndex < PlayerWeaponInfoSize)
	{
		const int id = m_selectIndex;
		const ShopItem& item = m_weaponItem[id];
		const bool owned = m_weaponOwned[id];

		// アイコン(64px → 2倍)・名前・説明
		DrawTex(m_spWeaponTex[id][static_cast<int>(ItemTex::Icon)], kDetailIconPos, 2.0f);
		DrawTex(m_spWeaponTex[id][static_cast<int>(ItemTex::Name)], kDetailNamePos, 1.0f, kPivotLeftTop);
		DrawTex(m_spWeaponTex[id][static_cast<int>(ItemTex::Explanation)], kDetailExplPos, 1.0f, kPivotLeftTop);

		DrawTex(m_spTex[static_cast<int>(ShopTex::PriceInfoPanel)], kInfoPos);

		// 所持 あり/なし
		label(ShopTex::HaveLabel, 0);
		DrawTex(m_spTex[static_cast<int>(owned ? ShopTex::Have : ShopTex::NotHave)], { kHaveTextX, InfoRowY(0) });

		// 在庫 〇(同じ行の右側)
		DrawStock(item.stock);

		// 買値(持っていたら ー)
		label(ShopTex::BuyPriceLabel, 1);
		if (owned)
		{
			DrawTex(m_spTex[static_cast<int>(ShopTex::Dash)], { kDashX, InfoRowY(1) + 1.0f });
		}
		else
		{
			DrawPrice(item.buyPrice, InfoRowY(1));
		}

		// 武器は売れないので、売値の行と売るボタンは出さない

		// 買える:持っていない・在庫あり・お金が足りる
		const bool canBuy = !owned && item.stock > 0 && m_money >= item.buyPrice;
		DrawButton(ShopButton::Buy, { kInfoButtonX, InfoRowY(1) }, canBuy);
	}
}

void ShopDisplay::DrawInventory()
{
	DrawTex(m_spTex[static_cast<int>(ShopTex::InventoryPanel)], kHavePos);
	DrawTex(m_spTex[static_cast<int>(ShopTex::InventoryLabel)], kHaveLabelPos);

	const std::vector<HaveItem> list = GetHaveList();
	for (int slot = 0; slot < kHaveVisibleNum; slot++)
	{
		const int index = m_haveScroll.index + slot;
		if (index >= static_cast<int>(list.size())) { break; }

		// アイコン(40px) → 「×」 → 個数 の順に右へ並べる
		const float x = kHaveItemX + kHaveSpace * slot;
		const auto& spIcon = list[index].spIcon;
		if (spIcon)
		{
			// アイコンは40pxに揃える(武器は64px → 縮める)
			const float scale = 40.0f / static_cast<float>(spIcon->GetWidth());
			DrawTex(spIcon, { x, kHaveItemY }, scale);
		}
		DrawTex(m_spTex[static_cast<int>(ShopTex::Times)], { x + 44.0f, kHaveItemY });
		DrawNum(list[index].num, LAligned, { x + 60.0f, kHaveItemY }, 2.0f, false, kHaveNumMaxW);
	}

	ScrollDraw(m_haveScroll, ShopTex::InventoryTrack);
}

void ShopDisplay::BuySellUpdate()
{
	if (m_selectIndex >= GetListNum()) { return; }

	const int buyRow = IsStackTab() ? 2 : 1;
	const int sellRow = buyRow + 1;

	const Math::Vector2 buyPos = { kInfoButtonX, InfoRowY(buyRow) };
	const Math::Vector2 sellPos = { kInfoButtonX, InfoRowY(sellRow) };


	bool success = false;
	if (m_clickFlg && MouseHit(buyPos, kButtonW, kButtonH))
	{
		if (IsStackTab())
		{
			// 消耗品・素材のタブ → 選んでいる商品を、数量(m_count)の分だけ買う
			success = BuyStackItem(m_selectIndex, m_count);
		}
		else
		{
			// 武器のタブ → 選んでいる武器を1本買う(数量は無い)
			success = BuyWeapon(m_selectIndex);
		}

		m_clickFlg = false;
	}

	// 売るボタンは消耗品・素材のタブだけ(武器は売れない)
	if (IsStackTab() && m_clickFlg && MouseHit(sellPos, kButtonW, kButtonH))
	{
		// 選んでいる商品を、数量(m_count)の分だけ売る
		success = SellStackItem(m_selectIndex, m_count);

		m_clickFlg = false;
	}


	if (success)
	{
		if (auto spScene = m_wpPrepareScene.lock()) { spScene->OnShopDataChanged(); }
	}
}

bool ShopDisplay::BuyStackItem(int _index, int _count)
{
	//---------- 今のタブの配列を決める ----------
	// 消耗品と素材で配列が違うので、ポインタで同じように扱う
	ShopItem* items = nullptr;	// 商品(値段・在庫)
	int* have = nullptr;	// 持っている数
	int       num = 0;		// 商品の種類数


	if (m_tab == ShopTab::Consumable)
	{
		items = m_potionItem;
		have = m_potionHave;
		num = PotionsTypeMax;
	}
	else if (m_tab == ShopTab::Material)
	{
		items = m_materialItem;
		have = m_materialHave;
		num = kMaterialNum;
	}
	else
	{
		// 武器のタブはBuyWeaponで買う
		return false;
	}


	//----------  買えるか調べる ----------
	// 番号が範囲外
	if (_index < 0 || _index >= num) { return false; }

	// 0個以下は買えない
	if (_count <= 0) { return false; }

	ShopItem& item = items[_index];

	// 合計の値段
	const long total = static_cast<long>(item.buyPrice) * _count;


	// 在庫が足りない
	if (item.stock < _count) { return false; }

	// お金が足りない
	if (m_money < total) { return false; }


	//---------- 買う ----------
	m_money -= total;		// お金を払う
	item.stock -= _count;	// お店の在庫が減る
	have[_index] += _count;	// 自分の持ち物が増える

	return true;
}

bool ShopDisplay::SellStackItem(int _index, int _count)
{
	//---------- 今のタブの配列を決める ----------
	// 消耗品と素材で配列が違うので、ポインタで同じように扱う
	ShopItem* items = nullptr;	// 商品(値段・在庫)
	int* have = nullptr;	// 持っている数
	int       num = 0;		// 商品の種類数


	if (m_tab == ShopTab::Consumable)
	{
		items = m_potionItem;
		have = m_potionHave;
		num = PotionsTypeMax;
	}
	else if (m_tab == ShopTab::Material)
	{
		items = m_materialItem;
		have = m_materialHave;
		num = kMaterialNum;
	}
	else
	{
		// 武器のタブはBuyWeaponで買う
		return false;
	}


	//----------  買えるか調べる ----------
	// 番号が範囲外
	if (_index < 0 || _index >= num) { return false; }

	// 0個以下は買えない
	if (_count <= 0) { return false; }

	ShopItem& item = items[_index];

	// 合計の値段
	const long total = static_cast<long>(item.sellPrice) * _count;


	// 所持量が足りない
	if (have[_index] < _count) { return false; }


	//---------- 売る ----------
	m_money += total;		// お金をもらう
	item.stock += _count;	// お店の在庫が増える
	have[_index] -= _count;	// 自分の持ち物が減る

	return true;
}

bool ShopDisplay::BuyWeapon(int _id)
{
	//---------- 買えるか調べる ----------
	if (_id < 0 || _id >= PlayerWeaponInfoSize) { return false; }	// 番号が範囲外

	ShopItem& item = m_weaponItem[_id];		// 書き換えるので参照(&)で取る

	if (m_weaponOwned[_id]) { return false; }	// もう持っている(同じ武器は2本いらない)
	if (item.stock <= 0) { return false; }	// 在庫がない(未入荷)
	if (m_money < item.buyPrice) { return false; }	// お金が足りない

	//---------- 買う ----------
	m_money -= item.buyPrice;		// お金を払う
	item.stock -= 1;				// お店の在庫が減る
	m_weaponOwned[_id] = true;		// 持っている武器になる

	return true;
}

void ShopDisplay::SaveShopData()
{
	//---------- 読み込み・書き込み ----------
	auto readJson = [](const std::string& _path) -> nlohmann::json
		{
			std::ifstream file(_path);
			if (!file) { return nlohmann::json::object(); }
			nlohmann::json data = nlohmann::json::parse(file, nullptr, false);
			if (data.is_discarded()) { return nlohmann::json::object(); }
			return data;
		};

	auto writeJson = [](const std::string& _path, const nlohmann::json& _data)
		{
			std::ofstream file(_path);
			if (!file)
			{
				KdDebugGUI::Instance().AddLog(U8("[Error] ShopDisplayの保存に失敗しました : %s\n"), _path.c_str());
				return;
			}
			file << _data.dump(4);	// 4 … 字下げの幅(今のファイルと同じ見た目)
		};

	// 配列 _key の中で "id" が合う要素の _valueKey に、_values[id] を書く
	// (ファイルに無いidは書かない。順番が違っても id で合わせる)
	auto writeArray = [](nlohmann::json& _data, const char* _key, const char* _valueKey, auto _getValue, int _num)
		{
			if (!_data.contains(_key) || !_data[_key].is_array()) { return; }
			for (auto& item : _data[_key])
			{
				int id = item.value("id", -1);
				if (id < 0 || id >= _num) { continue; }
				item[_valueKey] = _getValue(id);
			}
		};

	//---------- お店の在庫(ShopStockData.json) ----------
	nlohmann::json stock = readJson(m_shopStockPath);
	writeArray(stock, "potions", "stock", [&](int _id) { return m_potionItem[_id].stock; }, PotionsTypeMax);
	writeArray(stock, "materials", "stock", [&](int _id) { return m_materialItem[_id].stock; }, kMaterialNum);
	writeArray(stock, "weapons", "stock", [&](int _id) { return m_weaponItem[_id].stock; }, PlayerWeaponInfoSize);
	writeJson(m_shopStockPath, stock);

	//---------- 持ち物とお金(PlayerInventoryData.json) ----------
	nlohmann::json inventory = readJson(m_inventoryPath);
	writeArray(inventory, "potions", "num", [&](int _id) { return m_potionHave[_id]; }, PotionsTypeMax);
	writeArray(inventory, "weaponStrengtheningMaterials", "num", [&](int _id) { return m_materialHave[_id]; }, kMaterialNum);
	writeArray(inventory, "weapons", "owned", [&](int _id) { return m_weaponOwned[_id]; }, PlayerWeaponInfoSize);
	inventory["money"] = m_money;
	writeJson(m_inventoryPath, inventory);
}

void ShopDisplay::DrawButton(ShopButton _button, const Math::Vector2& _pos, bool _enable)
{
	float w = kButtonW, h = kButtonH;
	if (_button == ShopButton::Plus || _button == ShopButton::Minus) { w = h = kSmallSiz; }

	ButtonState state = ButtonState::Disable;
	if (_enable)
	{
		state = MouseHit(_pos, w, h) ? ButtonState::Hover : ButtonState::Normal;
	}
	DrawTex(m_spButtonTex[static_cast<int>(_button)][static_cast<int>(state)], _pos);
}

void ShopDisplay::DrawTex(const std::shared_ptr<KdTexture>& _spTex, const Math::Vector2& _pos, float _scale, const Math::Vector2& _pivot)
{
	if (!_spTex) { return; }
	DrawTexSize(_spTex, _pos, _spTex->GetWidth() * _scale, _spTex->GetHeight() * _scale, _pivot);
}

void ShopDisplay::DrawTexSize(const std::shared_ptr<KdTexture>& _spTex, const Math::Vector2& _pos, float _w, float _h, const Math::Vector2& _pivot)
{
	if (!_spTex) { return; }

	KdShaderManager::Instance().m_spriteShader.DrawTex(
		_spTex.get(),
		static_cast<int>(_pos.x), static_cast<int>(_pos.y),
		static_cast<int>(_w), static_cast<int>(_h),
		nullptr, &kWhiteColor, _pivot);
}

void ShopDisplay::DrawStock(int _stock)
{
	// 「在庫」と、その右に数(右端を買う・売るボタンの右端にそろえる)
	DrawTex(m_spTex[static_cast<int>(ShopTex::StockLabel)], { kStockLabelX, InfoRowY(0) });
	DrawNum(std::max(0, _stock), RAligned, { kStockNumRightX, InfoRowY(0) }, 2.0f, false, kStockNumMaxW);
}

void ShopDisplay::DrawPrice(long _price, float _rowY)
{
	// 右端を kInfoPriceRightX にそろえる(桁が変わっても「G」の位置は動かない)
	DrawNum(_price, RAligned, { kInfoPriceRightX, _rowY }, 2.0f, true, kInfoValueMaxW);
	DrawTex(m_spTex[static_cast<int>(ShopTex::Gold)], { kInfoGoldX, _rowY - 1.0f });
}

void ShopDisplay::DrawNum(long _num, Aligned _aligned, const Math::Vector2& _pos, float _siz, bool _separator, float _maxW)
{
	NumDraw::GetInstance().Drow(_num, _aligned, _pos, Math::Color(1, 1, 1, 1), _siz, _separator, -1, { _maxW, 0.0f });

	// NumDrawの中で Begin/End しているので、続けて描けるよう開き直す
	KdShaderManager::Instance().m_spriteShader.Begin(false);
}

float ShopDisplay::GetNumWidth(long _num, float _siz, bool _separator, float _maxW) const
{
	// 桁数(3桁区切りのカンマも1文字分)
	const int digits = static_cast<int>(std::to_string(std::abs(_num)).size());
	const int commas = _separator ? (digits - 1) / 3 : 0;
	const float w = (digits + commas) * kNumGlyphW * _siz;

	// 超えてはいけない横幅があれば、その幅に縮む
	return (_maxW > 0.0f && w > _maxW) ? _maxW : w;
}

/////////////////////////////////////////////////////////////////////
// 画像の読み込み
/////////////////////////////////////////////////////////////////////
void ShopDisplay::LoadOneTex(std::shared_ptr<KdTexture>& _spTex, const std::string& _path)
{
	if (_spTex) { return; }

	_spTex = std::make_shared<KdTexture>();
	if (!_spTex->Load(_path))
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] ShopDisplayの画像が読み込めません : %s\n"), _path.c_str());
	}
}

void ShopDisplay::LoadTex()
{
	const std::string shop = "Asset/Textures/GameUI/Prepare/Shop/";
	const std::string equip = "Asset/Textures/GameUI/Prepare/Equip/";
	const std::string menu = "Asset/Textures/GameUI/Prepare/Menu/";
	const std::string item = "Asset/Textures/GameUI/Item/";

	// ShopTexの順
	const std::string path[static_cast<int>(ShopTex::TexSize)] =
	{
		shop + "Text/ShopTitle.png", shop + "Text/MoneyLabel.png", menu + "Text/Gold.png", menu + "Text/Count.png",
		shop + "Panel/MoneyPanel.png", equip + "Panel/WeaponListPanel.png", shop + "Panel/ShopDetailPanel.png",
		shop + "Panel/PriceInfoPanel.png", shop + "Panel/InventoryPanel.png",
		equip + "Frame/SelectFrame.png", equip + "Frame/HoverFrame.png", item + "PlayerInventory/WeponStrengtheningIcon/NotIcon.png",
		shop + "Text/Owned.png", shop + "Text/NotStocked.png", shop + "Text/SoldOut.png",
		shop + "Text/HaveLabel.png", shop + "Text/Have.png", shop + "Text/NotHave.png", shop + "Text/BuyPriceLabel.png",
		shop + "Text/SellPriceLabel.png", shop + "Text/CountLabel.png", shop + "Text/TotalLabel.png",
		shop + "Text/Inventory.png", shop + "Text/Times.png", shop + "Text/Dash.png",
		equip + "Scroll/ScrollTrackH.png", shop + "Panel/InventoryScrollTrack.png",
		equip + "Scroll/ScrollThumbLeftH.png", equip + "Scroll/ScrollThumbMidH.png", equip + "Scroll/ScrollThumbRightH.png", equip + "Scroll/ScrollThumbGripH.png",
		equip + "Scroll/ScrollThumbLeftH_Hover.png", equip + "Scroll/ScrollThumbMidH_Hover.png", equip + "Scroll/ScrollThumbRightH_Hover.png", equip + "Scroll/ScrollThumbGripH_Hover.png",
		shop + "Text/StockLabel.png",
	};
	for (int i = 0; i < static_cast<int>(ShopTex::TexSize); i++)
	{
		LoadOneTex(m_spTex[i], path[i]);
	}

	// タブ
	const std::string tabName[static_cast<int>(ShopTab::TabSize)] = { "Consumable", "Material", "Weapon" };
	for (int t = 0; t < static_cast<int>(ShopTab::TabSize); t++)
	{
		LoadOneTex(m_spTabTex[t][0], shop + "Tab/Tab" + tabName[t] + ".png");
		LoadOneTex(m_spTabTex[t][1], shop + "Tab/Tab" + tabName[t] + "_Select.png");
	}

	// ボタン
	const std::string buttonName[static_cast<int>(ShopButton::ButtonSize)] = { "Buy", "Sell", "Plus", "Minus" };
	const std::string stateName[static_cast<int>(ButtonState::StateSize)] = { "", "_Hover", "_Disable" };
	for (int b = 0; b < static_cast<int>(ShopButton::ButtonSize); b++)
	{
		for (int s = 0; s < static_cast<int>(ButtonState::StateSize); s++)
		{
			LoadOneTex(m_spButtonTex[b][s], shop + "Button/" + buttonName[b] + "Button" + stateName[s] + ".png");
		}
	}

	// ポーション(アイコン・名前・説明)
	for (int i = 0; i < PotionsTypeMax; i++)
	{
		const std::string dir = item + "Potion/" + kPotionName[i] + "/";
		LoadOneTex(m_spPotionTex[i][static_cast<int>(ItemTex::Icon)], dir + "Icon.png");
		LoadOneTex(m_spPotionTex[i][static_cast<int>(ItemTex::Name)], dir + "SelectNameBase.png");
		LoadOneTex(m_spPotionTex[i][static_cast<int>(ItemTex::Explanation)], dir + "SelectExplanationBase.png");
	}

	// 武器(アイコン・名前・説明)
	for (int i = 0; i < PlayerWeaponInfoSize; i++)
	{
		LoadOneTex(m_spWeaponTex[i][static_cast<int>(ItemTex::Icon)], item + "PlayerInventory/WeponStrengtheningIcon/" + kWeaponName[i] + "/WeponStrengtheningIcon8.png");
		LoadOneTex(m_spWeaponTex[i][static_cast<int>(ItemTex::Name)], item + "WeaponsStrengthening/" + kWeaponName[i] + "/Icon/SelectNameBase.png");
		LoadOneTex(m_spWeaponTex[i][static_cast<int>(ItemTex::Explanation)], item + "WeaponsStrengthening/" + kWeaponName[i] + "/Icon/SelectExplanationBase.png");
	}

	// 強化素材(アイコン・名前・説明)
	for (int i = 0; i < kMaterialNum; i++)
	{
		const std::string dir = item + "WeaponsStrengtheningItem/WeaponStrengthening_Crystal" + std::to_string(i + 1) + "/";
		LoadOneTex(m_spMaterialTex[i][static_cast<int>(ItemTex::Icon)], dir + "Icon.png");
		LoadOneTex(m_spMaterialTex[i][static_cast<int>(ItemTex::Name)], dir + "SelectNameBase.png");
		LoadOneTex(m_spMaterialTex[i][static_cast<int>(ItemTex::Explanation)], dir + "SelectExplanationBase.png");
	}
}
