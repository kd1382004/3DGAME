#include "ShopDisplay.h"
#include"../../../../Info/MouseInfo/MouseInfo.h"
#include"../../../../Info/KeyInfo/KeyInfo.h"
#include"../../../../Info/NumDraw/NumDraw.h"
#include"../../../../Scene/SceneManager.h"

namespace
{
	// 画面サイズ(座標変換用)
	constexpr float kScreenW = 1280.0f;
	constexpr float kScreenH = 720.0f;

	// レイアウト(画面左上が0,0・下がプラス。完成イメージと同じ数字)
	constexpr float kTitleX = 548.0f,  kTitleY = 6.0f;
	constexpr float kMoneyX = 1000.0f, kMoneyY = 14.0f;			// 所持金パネル
	constexpr float kTabX = 550.0f, kTabY = 86.0f, kTabSpace = 158.0f;	// タブ(150 + すき間8)
	constexpr float kTabW = 150.0f, kTabH = 58.0f;

	constexpr float kListX = 550.0f, kListY = 150.0f, kListW = 700.0f, kListH = 200.0f;		// 商品一覧
	constexpr float kSlotOffsetX = 34.0f, kSlotOffsetY = 40.0f;
	constexpr float kSlotSiz = 64.0f, kSlotSpace = 96.0f;
	constexpr int   kListVisibleNum = 7;

	constexpr float kDetailX = 550.0f, kDetailY = 358.0f;		// 商品詳細
	constexpr float kInfoX = kDetailX + 356.0f, kInfoY = kDetailY + 17.0f;	// 所持・値段の欄(334×206)
	constexpr float kInfoLabelX  = kInfoX + 14.0f;		// 欄の項目のx
	constexpr float kInfoValueX  = kInfoX + 96.0f;		// 欄の値のx
	constexpr float kInfoButtonX = kInfoX + 224.0f;		// 買う・売るボタンのx(値段の行の右)
	constexpr float kInfoRow0 = kInfoY + 8.0f, kInfoRowSpace = 44.0f;
	constexpr float kButtonW = 92.0f, kButtonH = 38.0f, kSmallSiz = 36.0f;

	constexpr float kHaveX = 550.0f, kHaveY = 606.0f, kHaveW = 700.0f, kHaveH = 104.0f;	// 持ち物
	constexpr float kHaveItemX = kHaveX + 130.0f, kHaveItemY = kHaveY + 18.0f, kHaveSpace = 130.0f;
	constexpr int   kHaveVisibleNum = 4;

	constexpr float kTrackH = 16.0f;
	constexpr float kTrackInset = 2.0f;

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

	// スクロールの場所
	m_listScroll.trackX = kListX + 30.0f;  m_listScroll.trackY = kListY + 162.0f; m_listScroll.trackW = 640.0f;
	m_listScroll.visibleNum = kListVisibleNum;
	m_haveScroll.trackX = kHaveItemX;      m_haveScroll.trackY = kHaveY + 70.0f;   m_haveScroll.trackW = 520.0f;
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

	const nlohmann::json master    = readJson(m_shopMasterPath);
	const nlohmann::json stock     = readJson(m_shopStockPath);
	const nlohmann::json inventory = readJson(m_inventoryPath);

	// 値段(マスター)
	auto readMaster = [&](const char* _key, ShopItem* _items, int _num)
		{
			if (!master.contains(_key)) { return; }
			for (const auto& item : master[_key])
			{
				int id = item.value("id", -1);
				if (id < 0 || id >= _num) { continue; }
				_items[id].buyPrice  = item.value("buyPrice", 0);
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

	// 武器(無ければソードだけ持っている)
	for (int i = 0; i < PlayerWeaponInfoSize; i++) { m_weaponOwned[i] = false; }
	m_weaponOwned[PlayerWeaponInfo_Sword] = true;
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
		if (MouseHit(kTabX + kTabSpace * t, kTabY, kTabW, kTabH))
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
	ScrollUpdate(m_listScroll, listNum, kListX, kListY, kListW, kListH);
	ScrollCalc(m_listScroll, listNum);

	ScrollCalc(m_haveScroll, haveNum);
	ScrollUpdate(m_haveScroll, haveNum, kHaveX, kHaveY, kHaveW, kHaveH);
	ScrollCalc(m_haveScroll, haveNum);

	//---------- 商品一覧(カーソル・選択) ----------
	m_hoverIndex = -1;
	if (!m_listScroll.dragFlg)
	{
		for (int slot = 0; slot < kListVisibleNum; slot++)
		{
			int index = m_listScroll.index + slot;
			if (index >= listNum) { break; }

			float x = kListX + kSlotOffsetX + kSlotSpace * slot;
			float y = kListY + kSlotOffsetY;
			if (MouseHit(x, y, kSlotSiz, kSlotSiz))
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
		const float rowY = kInfoRow0 + kInfoRowSpace;
		if (m_clickFlg && MouseHit(kInfoValueX - 6.0f,  rowY + 8.0f, kSmallSiz, kSmallSiz)) { m_count--; m_clickFlg = false; }
		if (m_clickFlg && MouseHit(kInfoValueX + 68.0f, rowY + 8.0f, kSmallSiz, kSmallSiz)) { m_count++; m_clickFlg = false; }
		m_count = std::clamp(m_count, 1, 99);
	}

	// TODO : 買う・売る(押したらJSONを書き換えて LoadShopData() で読み直す)
}

bool ShopDisplay::MouseHit(float _x, float _y, float _w, float _h) const
{
	// 画面中央が0,0・上がプラス → 画面左上が0,0・下がプラス
	const POINT mousePos = MouseInfo::Instance().m_windowPos;
	const float mx = static_cast<float>(mousePos.x) + kScreenW / 2.0f;
	const float my = kScreenH / 2.0f - static_cast<float>(mousePos.y);

	return (_x <= mx && mx <= _x + _w && _y <= my && my <= _y + _h);
}

/////////////////////////////////////////////////////////////////////
// 横スクロール(EquipDisplayと同じ動き)
/////////////////////////////////////////////////////////////////////
void ShopDisplay::ScrollCalc(HScroll& _scroll, int _total)
{
	const int   maxIndex = std::max(0, _total - _scroll.visibleNum);
	const float innerX = _scroll.trackX + kTrackInset;
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

void ShopDisplay::ScrollUpdate(HScroll& _scroll, int _total, float _panelX, float _panelY, float _panelW, float _panelH)
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

	const POINT mousePos = MouseInfo::Instance().m_windowPos;
	const float mx = static_cast<float>(mousePos.x) + kScreenW / 2.0f;

	const float trackLeft  = _scroll.trackX + kTrackInset;
	const float trackRight = _scroll.trackX + _scroll.trackW - kTrackInset;

	const bool isOnTrack = MouseHit(trackLeft, _scroll.trackY, trackRight - trackLeft, kTrackH);
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
	if (!_scroll.dragFlg && MouseHit(_panelX, _panelY, _panelW, _panelH))
	{
		int wheel = MouseInfo::Instance().GetMouseWheelValue();
		if (wheel >= 1)       { _scroll.index -= 1; }
		else if (wheel <= -1) { _scroll.index += 1; }
	}

	_scroll.hoverFlg = isOnThumb || _scroll.dragFlg;
	_scroll.index = std::clamp(_scroll.index, 0, maxIndex);
}

void ShopDisplay::ScrollDraw(const HScroll& _scroll, ShopTex _track)
{
	DrawTLSize(m_spTex[static_cast<int>(_track)], _scroll.trackX, _scroll.trackY, _scroll.trackW, kTrackH);

	const bool hover = _scroll.hoverFlg;
	const auto& spL = m_spTex[static_cast<int>(hover ? ShopTex::ThumbLeftHover  : ShopTex::ThumbLeft)];
	const auto& spM = m_spTex[static_cast<int>(hover ? ShopTex::ThumbMidHover   : ShopTex::ThumbMid)];
	const auto& spR = m_spTex[static_cast<int>(hover ? ShopTex::ThumbRightHover : ShopTex::ThumbRight)];
	const auto& spG = m_spTex[static_cast<int>(hover ? ShopTex::ThumbGripHover  : ShopTex::ThumbGrip)];
	if (!spL || !spM || !spR || !spG) { return; }

	const float lw = static_cast<float>(spL->GetWidth());
	const float rw = static_cast<float>(spR->GetWidth());

	DrawTLSize(spL, _scroll.thumbX, _scroll.trackY, lw, kTrackH);
	const float midW = _scroll.thumbW - lw - rw;
	if (midW > 0.0f) { DrawTLSize(spM, _scroll.thumbX + lw, _scroll.trackY, midW, kTrackH); }
	DrawTLSize(spR, _scroll.thumbX + _scroll.thumbW - rw, _scroll.trackY, rw, kTrackH);
	DrawTL(spG, _scroll.thumbX + _scroll.thumbW / 2.0f - spG->GetWidth() / 2.0f, _scroll.trackY);
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
	DrawTL(m_spTex[static_cast<int>(ShopTex::Title)], kTitleX, kTitleY);

	// 所持金
	DrawTL(m_spTex[static_cast<int>(ShopTex::MoneyPanel)], kMoneyX, kMoneyY);
	DrawTL(m_spTex[static_cast<int>(ShopTex::MoneyLabel)], kMoneyX + 12.0f, kMoneyY + 4.0f);
	float x = DrawNumTL(m_money, kMoneyX + 130.0f, kMoneyY + 18.0f, 2.0f, true);
	DrawTL(m_spTex[static_cast<int>(ShopTex::Gold)], x + 4.0f, kMoneyY + 12.0f);

	// タブ
	for (int t = 0; t < static_cast<int>(ShopTab::TabSize); t++)
	{
		const int select = (m_tab == static_cast<ShopTab>(t)) ? 1 : 0;
		DrawTL(m_spTabTex[t][select], kTabX + kTabSpace * t, kTabY);
	}
}

void ShopDisplay::DrawList()
{
	DrawTL(m_spTex[static_cast<int>(ShopTex::ListPanel)], kListX, kListY);

	const int listNum = GetListNum();
	for (int slot = 0; slot < kListVisibleNum; slot++)
	{
		const int index = m_listScroll.index + slot;
		if (index >= listNum) { break; }

		const float x = kListX + kSlotOffsetX + kSlotSpace * slot;
		const float y = kListY + kSlotOffsetY;
		const float underY = y + kSlotSiz;	// アイコンの下(値段・状態)

		if (IsStackTab())
		{
			// 消耗品・素材のアイコンは40px → 1.5倍(60px)で真ん中に
			DrawTL(GetStackTex(index, ItemTex::Icon), x + 2.0f, y + 2.0f, 1.5f);

			if (GetStackItem(index).stock <= 0)
			{
				const auto& sp = m_spTex[static_cast<int>(ShopTex::SoldOut)];
				if (sp) { DrawTL(sp, x + kSlotSiz / 2.0f - sp->GetWidth() / 2.0f, underY); }
			}
			else
			{
				// 値段 + G(半分の大きさ)を真ん中揃え
				const int   price = GetStackItem(index).buyPrice;
				const float numW  = static_cast<float>(std::to_string(price).size()) * 18.0f;
				const float startX = x + kSlotSiz / 2.0f - (numW + 24.0f) / 2.0f;
				float endX = DrawNumTL(price, startX, underY + 10.0f);
				DrawTL(m_spTex[static_cast<int>(ShopTex::Gold)], endX + 2.0f, underY + 12.0f, 0.5f);
			}
		}
		else if (m_tab == ShopTab::Weapon)
		{
			DrawTL(m_spWeaponTex[index][static_cast<int>(ItemTex::Icon)], x, y);

			// 持っている → 所持済み / 在庫なし → 未入荷 / それ以外 → 値段
			if (m_weaponOwned[index] || m_weaponItem[index].stock <= 0)
			{
				const auto& sp = m_spTex[static_cast<int>(m_weaponOwned[index] ? ShopTex::Owned : ShopTex::NotStocked)];
				if (sp) { DrawTL(sp, x + kSlotSiz / 2.0f - sp->GetWidth() / 2.0f, underY); }
			}
			else
			{
				const int   price = m_weaponItem[index].buyPrice;
				const float numW  = static_cast<float>(std::to_string(price).size()) * 18.0f;
				const float startX = x + kSlotSiz / 2.0f - (numW + 24.0f) / 2.0f;
				float endX = DrawNumTL(price, startX, underY + 10.0f);
				DrawTL(m_spTex[static_cast<int>(ShopTex::Gold)], endX + 2.0f, underY + 12.0f, 0.5f);
			}
		}

		// 枠(選択中は金、カーソルは銀)
		if (index == m_selectIndex)
		{
			DrawTL(m_spTex[static_cast<int>(ShopTex::SelectFrame)], x - 2.0f, y - 2.0f);
		}
		else if (index == m_hoverIndex)
		{
			DrawTL(m_spTex[static_cast<int>(ShopTex::HoverFrame)], x - 2.0f, y - 2.0f);
		}
	}

	ScrollDraw(m_listScroll, ShopTex::ListTrack);
}

void ShopDisplay::DrawDetail()
{
	DrawTL(m_spTex[static_cast<int>(ShopTex::DetailPanel)], kDetailX, kDetailY);

	auto row = [](int _i) { return kInfoRow0 + kInfoRowSpace * _i; };
	auto label = [&](ShopTex _tex, int _i) { DrawTL(m_spTex[static_cast<int>(_tex)], kInfoLabelX, row(_i)); };

	if (IsStackTab() && m_selectIndex < GetListNum())
	{
		const int id = m_selectIndex;
		const ShopItem& item = GetStackItem(id);
		const int have = GetStackHave(id);

		// アイコン(40px → 3倍)・名前・説明
		DrawTL(GetStackTex(id, ItemTex::Icon),        kDetailX + 34.0f,  kDetailY + 30.0f, 3.0f);
		DrawTL(GetStackTex(id, ItemTex::Name),        kDetailX + 20.0f,  kDetailY + 160.0f);
		DrawTL(GetStackTex(id, ItemTex::Explanation), kDetailX + 180.0f, kDetailY + 36.0f);

		DrawTL(m_spTex[static_cast<int>(ShopTex::PriceInfoPanel)], kInfoX, kInfoY);

		// 所持 〇 個
		label(ShopTex::HaveLabel, 0);
		float x = DrawNumTL(have, kInfoValueX, row(0) + 12.0f);
		DrawTL(m_spTex[static_cast<int>(ShopTex::Count)], x + 4.0f, row(0));

		// 数量 － 〇 ＋
		label(ShopTex::CountLabel, 1);
		DrawButton(ShopButton::Minus, kInfoValueX - 6.0f, row(1) + 8.0f, m_count > 1);
		DrawNumTL(m_count, kInfoValueX + 40.0f, row(1) + 12.0f);
		DrawButton(ShopButton::Plus, kInfoValueX + 68.0f, row(1) + 8.0f, true);

		// 買値 〇 G [買う] … 数量分の合計
		const long buyTotal  = static_cast<long>(item.buyPrice)  * m_count;
		const long sellTotal = static_cast<long>(item.sellPrice) * m_count;

		// 買える:在庫が数量以上 かつ お金が足りる / 売れる:持っている数が数量以上
		const bool canBuy  = (item.stock >= m_count) && (m_money >= buyTotal);
		const bool canSell = (have >= m_count);

		label(ShopTex::BuyPriceLabel, 2);
		x = DrawNumTL(buyTotal, kInfoValueX, row(2) + 12.0f, 2.0f, true);
		DrawTL(m_spTex[static_cast<int>(ShopTex::Gold)], x + 4.0f, row(2) + 6.0f);
		DrawButton(ShopButton::Buy, kInfoButtonX, row(2) + 7.0f, canBuy);

		// 売値 〇 G [売る] … 数量分の合計
		label(ShopTex::SellPriceLabel, 3);
		x = DrawNumTL(sellTotal, kInfoValueX, row(3) + 12.0f, 2.0f, true);
		DrawTL(m_spTex[static_cast<int>(ShopTex::Gold)], x + 4.0f, row(3) + 6.0f);
		DrawButton(ShopButton::Sell, kInfoButtonX, row(3) + 7.0f, canSell);
	}
	else if (m_tab == ShopTab::Weapon && m_selectIndex < PlayerWeaponInfoSize)
	{
		const int id = m_selectIndex;
		const ShopItem& item = m_weaponItem[id];
		const bool owned = m_weaponOwned[id];

		// アイコン(64px → 2倍)・名前・説明
		DrawTL(m_spWeaponTex[id][static_cast<int>(ItemTex::Icon)],        kDetailX + 30.0f,  kDetailY + 26.0f, 2.0f);
		DrawTL(m_spWeaponTex[id][static_cast<int>(ItemTex::Name)],        kDetailX + 20.0f,  kDetailY + 160.0f);
		DrawTL(m_spWeaponTex[id][static_cast<int>(ItemTex::Explanation)], kDetailX + 180.0f, kDetailY + 36.0f);

		DrawTL(m_spTex[static_cast<int>(ShopTex::PriceInfoPanel)], kInfoX, kInfoY);

		// 所持 あり/なし
		label(ShopTex::HaveLabel, 0);
		DrawTL(m_spTex[static_cast<int>(owned ? ShopTex::Have : ShopTex::NotHave)], kInfoValueX, row(0) + 2.0f);

		// 買値(持っていたら ー)
		label(ShopTex::BuyPriceLabel, 1);
		if (owned)
		{
			DrawTL(m_spTex[static_cast<int>(ShopTex::Dash)], kInfoValueX + 8.0f, row(1) + 8.0f);
		}
		else
		{
			float x = DrawNumTL(item.buyPrice, kInfoValueX, row(1) + 12.0f, 2.0f, true);
			DrawTL(m_spTex[static_cast<int>(ShopTex::Gold)], x + 4.0f, row(1) + 6.0f);
		}

		// 売値
		label(ShopTex::SellPriceLabel, 2);
		float x = DrawNumTL(item.sellPrice, kInfoValueX, row(2) + 12.0f, 2.0f, true);
		DrawTL(m_spTex[static_cast<int>(ShopTex::Gold)], x + 4.0f, row(2) + 6.0f);

		// 買える:持っていない・在庫あり・お金が足りる / 売れる:持っていて、装備中ではない
		const bool equipped = (SceneManager::Instance().GetPlayerWeaponInfo() == id);
		const bool canBuy  = !owned && item.stock > 0 && m_money >= item.buyPrice;
		const bool canSell = owned && !equipped;
		DrawButton(ShopButton::Buy,  kInfoButtonX, row(1) + 7.0f, canBuy);
		DrawButton(ShopButton::Sell, kInfoButtonX, row(2) + 7.0f, canSell);
	}
}

void ShopDisplay::DrawInventory()
{
	DrawTL(m_spTex[static_cast<int>(ShopTex::InventoryPanel)], kHaveX, kHaveY);
	DrawTL(m_spTex[static_cast<int>(ShopTex::InventoryLabel)], kHaveX + 16.0f, kHaveY + 14.0f);

	const std::vector<HaveItem> list = GetHaveList();
	for (int slot = 0; slot < kHaveVisibleNum; slot++)
	{
		const int index = m_haveScroll.index + slot;
		if (index >= static_cast<int>(list.size())) { break; }

		const float x = kHaveItemX + kHaveSpace * slot;
		const auto& spIcon = list[index].spIcon;
		if (spIcon)
		{
			// アイコンは40pxに揃える(武器は64px → 縮める)
			const float scale = 40.0f / static_cast<float>(spIcon->GetWidth());
			DrawTL(spIcon, x, kHaveItemY, scale);
		}
		DrawTL(m_spTex[static_cast<int>(ShopTex::Times)], x + 40.0f, kHaveItemY);
		DrawNumTL(list[index].num, x + 80.0f, kHaveItemY + 6.0f);
	}

	ScrollDraw(m_haveScroll, ShopTex::InventoryTrack);
}

void ShopDisplay::DrawButton(ShopButton _button, float _x, float _y, bool _enable)
{
	float w = kButtonW, h = kButtonH;
	if (_button == ShopButton::Plus || _button == ShopButton::Minus) { w = h = kSmallSiz; }

	ButtonState state = ButtonState::Disable;
	if (_enable)
	{
		state = MouseHit(_x, _y, w, h) ? ButtonState::Hover : ButtonState::Normal;
	}
	DrawTL(m_spButtonTex[static_cast<int>(_button)][static_cast<int>(state)], _x, _y);
}

void ShopDisplay::DrawTL(const std::shared_ptr<KdTexture>& _spTex, float _x, float _y, float _scale)
{
	if (!_spTex) { return; }
	DrawTLSize(_spTex, _x, _y, _spTex->GetWidth() * _scale, _spTex->GetHeight() * _scale);
}

void ShopDisplay::DrawTLSize(const std::shared_ptr<KdTexture>& _spTex, float _x, float _y, float _w, float _h)
{
	if (!_spTex) { return; }

	// 左上基準(下がプラス) → 中心基準(上がプラス)
	const float cx = _x + _w / 2.0f - kScreenW / 2.0f;
	const float cy = kScreenH / 2.0f - (_y + _h / 2.0f);

	KdShaderManager::Instance().m_spriteShader.DrawTex(
		_spTex.get(),
		static_cast<int>(cx), static_cast<int>(cy),
		static_cast<int>(_w), static_cast<int>(_h),
		nullptr, &kWhiteColor);
}

float ShopDisplay::DrawNumTL(long _num, float _x, float _y, float _siz, bool _separator)
{
	// 桁数(3桁区切りのカンマも1文字分)
	const int digits = static_cast<int>(std::to_string(std::abs(_num)).size());
	const int commas = _separator ? (digits - 1) / 3 : 0;
	const float w = (digits + commas) * 9.0f * _siz;
	const float h = 14.0f * _siz;

	// 左上基準 → NumDrawの左端・上下中央(中心基準・上がプラス)
	const Math::Vector2 pos = { _x - kScreenW / 2.0f, kScreenH / 2.0f - (_y + h / 2.0f) };
	NumDraw::GetInstance().Drow(_num, LAligned, pos, Math::Color(1, 1, 1, 1), _siz, _separator);

	// NumDrawの中で Begin/End しているので、続けて描けるよう開き直す
	KdShaderManager::Instance().m_spriteShader.Begin(false);

	return _x + w;
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
	const std::string shop  = "Asset/Textures/GameUI/Prepare/Shop/";
	const std::string equip = "Asset/Textures/GameUI/Prepare/Equip/";
	const std::string menu  = "Asset/Textures/GameUI/Prepare/Menu/";
	const std::string item  = "Asset/Textures/GameUI/Item/";

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
		LoadOneTex(m_spPotionTex[i][static_cast<int>(ItemTex::Icon)],        dir + "Icon.png");
		LoadOneTex(m_spPotionTex[i][static_cast<int>(ItemTex::Name)],        dir + "SelectNameBase.png");
		LoadOneTex(m_spPotionTex[i][static_cast<int>(ItemTex::Explanation)], dir + "SelectExplanationBase.png");
	}

	// 武器(アイコン・名前・説明)
	for (int i = 0; i < PlayerWeaponInfoSize; i++)
	{
		LoadOneTex(m_spWeaponTex[i][static_cast<int>(ItemTex::Icon)],        item + "PlayerInventory/WeponStrengtheningIcon/" + kWeaponName[i] + "/WeponStrengtheningIcon8.png");
		LoadOneTex(m_spWeaponTex[i][static_cast<int>(ItemTex::Name)],        item + "WeaponsStrengthening/" + kWeaponName[i] + "/Icon/SelectNameBase.png");
		LoadOneTex(m_spWeaponTex[i][static_cast<int>(ItemTex::Explanation)], item + "WeaponsStrengthening/" + kWeaponName[i] + "/Icon/SelectExplanationBase.png");
	}

	// 強化素材(アイコン・名前・説明)
	for (int i = 0; i < kMaterialNum; i++)
	{
		const std::string dir = item + "WeaponsStrengtheningItem/WeaponStrengthening_Crystal" + std::to_string(i + 1) + "/";
		LoadOneTex(m_spMaterialTex[i][static_cast<int>(ItemTex::Icon)],        dir + "Icon.png");
		LoadOneTex(m_spMaterialTex[i][static_cast<int>(ItemTex::Name)],        dir + "SelectNameBase.png");
		LoadOneTex(m_spMaterialTex[i][static_cast<int>(ItemTex::Explanation)], dir + "SelectExplanationBase.png");
	}
}
