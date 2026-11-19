#include "ShopDisplay.h"
#include"../../../../Info/MouseInfo/MouseInfo.h"
#include"../../../../Info/KeyInfo/KeyInfo.h"
#include"../../../../Scene/SceneManager.h"
#include"../../../../Scene/PrepareScene/PrepareScene.h"

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

	//商品一覧のスクロールの場所(レールの左端のx・真ん中のy・長さ)
	m_listScroll.m_trackLeft = -60.0f;
	m_listScroll.m_trackY = 40.0f;
	m_listScroll.m_trackW = 640.0f;
	m_listScroll.m_visibleNum = m_listVisibleNum;

	//持ち物のスクロールの場所
	m_haveScroll.m_trackLeft = 40.0f;
	m_haveScroll.m_trackY = -324.0f;
	m_haveScroll.m_trackW = 520.0f;
	m_haveScroll.m_visibleNum = m_haveVisibleNum;
}

void ShopDisplay::Update()
{
	// 開いているタブでなければ何もしない
	if (!m_activeFlg) { return; }

	// 左クリックはこのフレームで1回だけ取る(タブ・一覧・スクロールで取り合わないように)
	m_clickFlg = KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true);

	//---------- タブ ----------
	for (int t = 0; t < static_cast<int>(ShopTab::TabSize); t++)
	{
		const Math::Vector2 tabPos = { m_tabPos.x + m_tabSpace * t, m_tabPos.y };
		if (MouseHit(tabPos, m_tabW, m_tabH))
		{
			MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
			if (m_clickFlg && m_tab != static_cast<ShopTab>(t))
			{
				m_tab = static_cast<ShopTab>(t);
				m_selectIndex = 0;
				m_count = 1;
				m_listScroll.m_index = 0;
				m_haveScroll.m_index = 0;
				m_clickFlg = false;
			}
		}
	}

	//---------- スクロール ----------
	const int listNum = GetListNum();
	const int haveNum = static_cast<int>(GetHaveList().size());

	ScrollCalc(m_listScroll, listNum);
	ScrollUpdate(m_listScroll, listNum, m_listPos, m_listW, m_listH);
	ScrollCalc(m_listScroll, listNum);

	ScrollCalc(m_haveScroll, haveNum);
	ScrollUpdate(m_haveScroll, haveNum, m_havePos, m_haveW, m_haveH);
	ScrollCalc(m_haveScroll, haveNum);

	//---------- 商品一覧(カーソル・選択) ----------
	m_hoverIndex = -1;
	if (!m_listScroll.m_dragFlg)
	{
		for (int slot = 0; slot < m_listVisibleNum; slot++)
		{
			int index = m_listScroll.m_index + slot;
			if (index >= listNum) { break; }

			const Math::Vector2 slotPos = { m_slotPos.x + m_slotSpace * slot, m_slotPos.y };
			if (MouseHit(slotPos, m_slotSiz, m_slotSiz))
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
		const float rowY = GetInfoRowY(1);
		if (m_clickFlg && MouseHit({ m_countMinusX, rowY }, m_smallButtonSiz, m_smallButtonSiz))
		{
			m_count--;
			m_clickFlg = false;
		}
		if (m_clickFlg && MouseHit({ m_countPlusX, rowY }, m_smallButtonSiz, m_smallButtonSiz))
		{
			m_count++;
			m_clickFlg = false;
		}
		m_count = std::clamp(m_count, 1, 99);
	}

	// 買う・売る
	BuySellUpdate();
}

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

void ShopDisplay::LoadShopData()
{
	const nlohmann::json master = ReadJson(m_shopMasterPath, true);
	const nlohmann::json stock = ReadJson(m_shopStockPath, true);
	const nlohmann::json inventory = ReadJson(m_inventoryPath, true);

	//値段(マスター)
	ReadPrice(master, "potions", m_potionItem, PotionsTypeMax);
	ReadPrice(master, "materials", m_materialItem, MaterialTypeMax);
	ReadPrice(master, "weapons", m_weaponItem, PlayerWeaponInfoSize);

	//在庫
	ReadStock(stock, "potions", m_potionItem, PotionsTypeMax);
	ReadStock(stock, "materials", m_materialItem, MaterialTypeMax);
	ReadStock(stock, "weapons", m_weaponItem, PlayerWeaponInfoSize);

	//持ち物
	ReadHave(inventory, "potions", m_potionHave, PotionsTypeMax);
	ReadHave(inventory, "weaponStrengtheningMaterials", m_materialHave, MaterialTypeMax);

	//武器(無ければダガー(最初の武器)だけ持っている)
	for (int i = 0; i < PlayerWeaponInfoSize; i++)
	{
		m_weaponOwned[i] = false;
	}
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

void ShopDisplay::DrawNum(long _num, Aligned _aligned, const Math::Vector2& _pos, float _siz, bool _separator, float _maxW)
{
	NumDraw::GetInstance().Draw(_num, _aligned, _pos, Math::Color(1, 1, 1, 1), _siz, _separator, -1, { _maxW, 0.0f });

	// NumDrawの中で Begin/End しているので、続けて描けるよう開き直す
	KdShaderManager::Instance().m_spriteShader.Begin(false);
}

void ShopDisplay::DrawPrice(long _price, float _rowY)
{
	// 右端を m_infoPriceRightX にそろえる(桁が変わっても「G」の位置は動かない)
	DrawNum(_price, RAligned, { m_infoPriceRightX, _rowY }, 2.0f, true, m_infoValueMaxW);
	DrawTex(m_spTex[static_cast<int>(ShopTex::Gold)], { m_infoGoldX, _rowY - 1.0f });
}

void ShopDisplay::DrawStock(int _stock)
{
	// 「在庫」と、その右に数(右端を買う・売るボタンの右端にそろえる)
	DrawTex(m_spTex[static_cast<int>(ShopTex::StockLabel)], { m_stockLabelX, GetInfoRowY(0) });
	DrawNum(std::max(0, _stock), RAligned, { m_stockNumRightX, GetInfoRowY(0) }, 2.0f, false, m_stockNumMaxW);
}

void ShopDisplay::DrawButton(ShopButton _button, const Math::Vector2& _pos, bool _enable)
{
	float w = m_buttonW;
	float h = m_buttonH;

	//＋・－ボタンは小さい
	if (_button == ShopButton::Plus || _button == ShopButton::Minus)
	{
		w = m_smallButtonSiz;
		h = m_smallButtonSiz;
	}

	ButtonState state = ButtonState::Disable;
	if (_enable)
	{
		state = MouseHit(_pos, w, h) ? ButtonState::Hover : ButtonState::Normal;
	}
	DrawTex(m_spButtonTex[static_cast<int>(_button)][static_cast<int>(state)], _pos);
}

void ShopDisplay::DrawListPrice(int _price, float _slotX)
{
	const float numW = GetNumWidth(_price, 2.0f, false, m_listPriceMaxW);
	const float left = _slotX - (numW + 24.0f) / 2.0f;
	DrawNum(_price, LAligned, { left, m_slotPriceY }, 2.0f, false, m_listPriceMaxW);
	DrawTex(m_spTex[static_cast<int>(ShopTex::Gold)], { left + numW + 12.0f, m_slotPriceY + 2.0f }, 0.5f);
}

void ShopDisplay::DrawInfoLabel(ShopTex _tex, int _row)
{
	DrawTex(m_spTex[static_cast<int>(_tex)], { m_infoLabelX, GetInfoRowY(_row) });
}

void ShopDisplay::DrawHeader()
{
	DrawTex(m_spTex[static_cast<int>(ShopTex::Title)], m_titlePos);

	//所持金(数字は右揃え、「G」はいつも同じ位置)
	DrawTex(m_spTex[static_cast<int>(ShopTex::MoneyPanel)], m_moneyPanelPos);
	DrawTex(m_spTex[static_cast<int>(ShopTex::MoneyLabel)], m_moneyLabelPos);
	DrawNum(m_money, RAligned, { m_moneyNumRightX, m_moneyNumPos.y }, 2.0f, true, m_moneyNumMaxW);
	DrawTex(m_spTex[static_cast<int>(ShopTex::Gold)], { m_moneyGoldX, m_moneyNumPos.y - 1.0f });


	// タブ
	for (int t = 0; t < static_cast<int>(ShopTab::TabSize); t++)
	{
		const int select = (m_tab == static_cast<ShopTab>(t)) ? 1 : 0;
		DrawTex(m_spTabTex[t][select], { m_tabPos.x + m_tabSpace * t, m_tabPos.y });
	}
}

void ShopDisplay::DrawList()
{
	DrawTex(m_spTex[static_cast<int>(ShopTex::ListPanel)], m_listPos);

	const int listNum = GetListNum();
	for (int slot = 0; slot < m_listVisibleNum; slot++)
	{
		const int index = m_listScroll.m_index + slot;
		if (index >= listNum) { break; }

		const Math::Vector2 slotPos = { m_slotPos.x + m_slotSpace * slot, m_slotPos.y };
		const Math::Vector2 underPos = { slotPos.x, m_slotUnderY };

		if (IsStackTab())
		{
			// 消耗品・素材のアイコンは40px → 1.5倍(60px)
			DrawTex(GetStackTex(index, ItemTex::Icon), slotPos, 1.5f);

			if (GetStackItem(index).m_stock <= 0)
			{
				DrawTex(m_spTex[static_cast<int>(ShopTex::SoldOut)], underPos);
			}
			else
			{
				DrawListPrice(GetStackItem(index).m_buyPrice, slotPos.x);
			}
		}
		else if (m_tab == ShopTab::Weapon)
		{
			DrawTex(m_spWeaponTex[index][static_cast<int>(ItemTex::Icon)], slotPos);

			// 持っている → 所持済み / 在庫なし → 未入荷 / それ以外 → 値段
			if (m_weaponOwned[index] || m_weaponItem[index].m_stock <= 0)
			{
				DrawTex(m_spTex[static_cast<int>(m_weaponOwned[index] ? ShopTex::Owned : ShopTex::NotStocked)], underPos);
			}
			else
			{
				DrawListPrice(m_weaponItem[index].m_buyPrice, slotPos.x);
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
	DrawTex(m_spTex[static_cast<int>(ShopTex::DetailPanel)], m_detailPos);

	if (IsStackTab() && m_selectIndex < GetListNum())
	{
		const int id = m_selectIndex;
		const ShopItem& item = GetStackItem(id);
		const int have = GetStackHave(id);

		// アイコン(40px → 3倍)・名前・説明(名前と説明は長さが違うので左上基準)
		DrawTex(GetStackTex(id, ItemTex::Icon), m_detailIconPos, 3.0f);
		DrawTex(GetStackTex(id, ItemTex::Name), m_detailNamePos, 1.0f, m_pivotLeftTop);
		DrawTex(GetStackTex(id, ItemTex::Explanation), m_detailExplPos, 1.0f, m_pivotLeftTop);

		DrawTex(m_spTex[static_cast<int>(ShopTex::PriceInfoPanel)], m_infoPos);

		// 所持 〇 個
		DrawInfoLabel(ShopTex::HaveLabel, 0);
		DrawNum(have, LAligned, { m_infoValueX, GetInfoRowY(0) }, 2.0f, false, m_infoHaveNumMaxW);
		const float haveW = GetNumWidth(have, 2.0f, false, m_infoHaveNumMaxW);
		DrawTex(m_spTex[static_cast<int>(ShopTex::Count)], { m_infoValueX + haveW + 28.0f, GetInfoRowY(0) + 1.0f });

		// 在庫 〇(同じ行の右側)
		DrawStock(item.m_stock);

		// 数量 － 〇 ＋(数字は-と+の真ん中)
		DrawInfoLabel(ShopTex::CountLabel, 1);
		DrawButton(ShopButton::Minus, { m_countMinusX, GetInfoRowY(1) }, m_count > 1);
		DrawNum(m_count, CAligned, { m_countNumX, GetInfoRowY(1) }, 2.0f, false, m_countNumMaxW);
		DrawButton(ShopButton::Plus, { m_countPlusX, GetInfoRowY(1) }, true);

		// 買値・売値 … 数量分の合計
		const long buyTotal = static_cast<long>(item.m_buyPrice) * m_count;
		const long sellTotal = static_cast<long>(item.m_sellPrice) * m_count;

		// 買える:在庫が数量以上 かつ お金が足りる / 売れる:持っている数が数量以上
		const bool canBuy = (item.m_stock >= m_count) && (m_money >= buyTotal);
		const bool canSell = (have >= m_count);

		// 買値 〇 G [買う]
		DrawInfoLabel(ShopTex::BuyPriceLabel, 2);
		DrawPrice(buyTotal, GetInfoRowY(2));
		DrawButton(ShopButton::Buy, { m_infoButtonX, GetInfoRowY(2) }, canBuy);

		// 売値 〇 G [売る]
		DrawInfoLabel(ShopTex::SellPriceLabel, 3);
		DrawPrice(sellTotal, GetInfoRowY(3));
		DrawButton(ShopButton::Sell, { m_infoButtonX, GetInfoRowY(3) }, canSell);
	}
	else if (m_tab == ShopTab::Weapon && m_selectIndex < PlayerWeaponInfoSize)
	{
		const int id = m_selectIndex;
		const ShopItem& item = m_weaponItem[id];
		const bool owned = m_weaponOwned[id];

		// アイコン(64px → 2倍)・名前・説明
		DrawTex(m_spWeaponTex[id][static_cast<int>(ItemTex::Icon)], m_detailIconPos, 2.0f);
		DrawTex(m_spWeaponTex[id][static_cast<int>(ItemTex::Name)], m_detailNamePos, 1.0f, m_pivotLeftTop);
		DrawTex(m_spWeaponTex[id][static_cast<int>(ItemTex::Explanation)], m_detailExplPos, 1.0f, m_pivotLeftTop);

		DrawTex(m_spTex[static_cast<int>(ShopTex::PriceInfoPanel)], m_infoPos);

		// 所持 あり/なし
		DrawInfoLabel(ShopTex::HaveLabel, 0);
		DrawTex(m_spTex[static_cast<int>(owned ? ShopTex::Have : ShopTex::NotHave)], { m_haveTextX, GetInfoRowY(0) });

		// 在庫 〇(同じ行の右側)
		DrawStock(item.m_stock);

		// 買値(持っていたら ー)
		DrawInfoLabel(ShopTex::BuyPriceLabel, 1);
		if (owned)
		{
			DrawTex(m_spTex[static_cast<int>(ShopTex::Dash)], { m_dashX, GetInfoRowY(1) + 1.0f });
		}
		else
		{
			DrawPrice(item.m_buyPrice, GetInfoRowY(1));
		}

		// 武器は売れないので、売値の行と売るボタンは出さない

		// 買える:持っていない・在庫あり・お金が足りる
		const bool canBuy = !owned && item.m_stock > 0 && m_money >= item.m_buyPrice;
		DrawButton(ShopButton::Buy, { m_infoButtonX, GetInfoRowY(1) }, canBuy);
	}
}

void ShopDisplay::DrawInventory()
{
	DrawTex(m_spTex[static_cast<int>(ShopTex::InventoryPanel)], m_havePos);
	DrawTex(m_spTex[static_cast<int>(ShopTex::InventoryLabel)], m_haveLabelPos);

	const std::vector<HaveItem> list = GetHaveList();
	for (int slot = 0; slot < m_haveVisibleNum; slot++)
	{
		const int index = m_haveScroll.m_index + slot;
		if (index >= static_cast<int>(list.size())) { break; }

		// アイコン(40px) → 「×」 → 個数 の順に右へ並べる
		const float x = m_haveItemX + m_haveSpace * slot;
		const auto& spIcon = list[index].m_spIcon;
		if (spIcon)
		{
			// アイコンは40pxに揃える(武器は64px → 縮める)
			const float scale = 40.0f / static_cast<float>(spIcon->GetWidth());
			DrawTex(spIcon, { x, m_haveItemY }, scale);
		}
		DrawTex(m_spTex[static_cast<int>(ShopTex::Times)], { x + 44.0f, m_haveItemY });
		DrawNum(list[index].m_num, LAligned, { x + 60.0f, m_haveItemY }, 2.0f, false, m_haveNumMaxW);
	}

	ScrollDraw(m_haveScroll, ShopTex::InventoryTrack);
}

int ShopDisplay::GetListNum() const
{
	switch (m_tab)
	{
	case ShopTab::Consumable: return PotionsTypeMax;
	case ShopTab::Material:   return MaterialTypeMax;
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
		for (int i = 0; i < MaterialTypeMax; i++)
		{
			if (m_materialHave[i] > 0) { list.push_back({ m_spMaterialTex[i][static_cast<int>(ItemTex::Icon)], m_materialHave[i] }); }
		}
		break;
	default:
		break;
	}
	return list;
}

float ShopDisplay::GetNumWidth(long _num, float _siz, bool _separator, float _maxW) const
{
	// 桁数(3桁区切りのカンマも1文字分)
	const int digits = static_cast<int>(std::to_string(std::abs(_num)).size());
	const int commas = _separator ? (digits - 1) / 3 : 0;
	const float w = (digits + commas) * m_numGlyphW * _siz;

	// 超えてはいけない横幅があれば、その幅に縮む
	return (_maxW > 0.0f && w > _maxW) ? _maxW : w;
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

bool ShopDisplay::MouseHit(const Math::Vector2& _pos, float _w, float _h) const
{
	// マウスもフレームワークと同じ座標(画面の真ん中が0,0・上がプラス)
	const POINT mousePos = MouseInfo::Instance().m_windowPos;
	const float mx = static_cast<float>(mousePos.x);
	const float my = static_cast<float>(mousePos.y);

	return (std::abs(mx - _pos.x) <= _w / 2.0f && std::abs(my - _pos.y) <= _h / 2.0f);
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
		const std::string dir = item + "Potion/" + m_potionName[i] + "/";
		LoadOneTex(m_spPotionTex[i][static_cast<int>(ItemTex::Icon)], dir + "Icon.png");
		LoadOneTex(m_spPotionTex[i][static_cast<int>(ItemTex::Name)], dir + "SelectNameBase.png");
		LoadOneTex(m_spPotionTex[i][static_cast<int>(ItemTex::Explanation)], dir + "SelectExplanationBase.png");
	}

	// 武器(アイコン・名前・説明)
	for (int i = 0; i < PlayerWeaponInfoSize; i++)
	{
		LoadOneTex(m_spWeaponTex[i][static_cast<int>(ItemTex::Icon)], item + "PlayerInventory/WeponStrengtheningIcon/" + m_weaponName[i] + "/WeponStrengtheningIcon8.png");
		LoadOneTex(m_spWeaponTex[i][static_cast<int>(ItemTex::Name)], item + "WeaponsStrengthening/" + m_weaponName[i] + "/Icon/SelectNameBase.png");
		LoadOneTex(m_spWeaponTex[i][static_cast<int>(ItemTex::Explanation)], item + "WeaponsStrengthening/" + m_weaponName[i] + "/Icon/SelectExplanationBase.png");
	}

	// 強化素材(アイコン・名前・説明)
	for (int i = 0; i < MaterialTypeMax; i++)
	{
		const std::string dir = item + "WeaponsStrengtheningItem/WeaponStrengthening_Crystal" + std::to_string(i + 1) + "/";
		LoadOneTex(m_spMaterialTex[i][static_cast<int>(ItemTex::Icon)], dir + "Icon.png");
		LoadOneTex(m_spMaterialTex[i][static_cast<int>(ItemTex::Name)], dir + "SelectNameBase.png");
		LoadOneTex(m_spMaterialTex[i][static_cast<int>(ItemTex::Explanation)], dir + "SelectExplanationBase.png");
	}
}

void ShopDisplay::LoadOneTex(std::shared_ptr<KdTexture>& _spTex, const std::string& _path)
{
	if (_spTex) { return; }

	_spTex = std::make_shared<KdTexture>();
	if (!_spTex->Load(_path))
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] ShopDisplayの画像が読み込めません : %s\n"), _path.c_str());
	}
}

void ShopDisplay::ScrollCalc(HScroll& _scroll, int _total)
{
	const int   maxIndex = std::max(0, _total - _scroll.m_visibleNum);
	const float innerX = _scroll.m_trackLeft + m_trackInset;
	const float innerW = _scroll.m_trackW - m_trackInset * 2.0f;

	// スクロールの必要がなければ、つまみはレールいっぱい
	if (maxIndex == 0)
	{
		_scroll.m_index = 0;
		_scroll.m_thumbX = innerX;
		_scroll.m_thumbW = innerW;
		return;
	}

	// 長さ = レールの長さ × 見えている数 ÷ 全部の数(下限あり)
	const float thumbW = innerW * static_cast<float>(_scroll.m_visibleNum) / static_cast<float>(_total);
	_scroll.m_thumbW = std::clamp(thumbW, m_thumbMinW, innerW);

	// 位置 = 今の番号の割合
	_scroll.m_index = std::clamp(_scroll.m_index, 0, maxIndex);
	const float ratio = static_cast<float>(_scroll.m_index) / static_cast<float>(maxIndex);
	_scroll.m_thumbX = innerX + (innerW - _scroll.m_thumbW) * ratio;
}

void ShopDisplay::ScrollUpdate(HScroll& _scroll, int _total, const Math::Vector2& _panelPos, float _panelW, float _panelH)
{
	const int maxIndex = std::max(0, _total - _scroll.m_visibleNum);

	// スクロールの必要がなければ何もしない
	if (maxIndex == 0)
	{
		_scroll.m_index = 0;
		_scroll.m_dragFlg = false;
		_scroll.m_hoverFlg = false;
		return;
	}

	const float mx = static_cast<float>(MouseInfo::Instance().m_windowPos.x);

	const float trackLeft = _scroll.m_trackLeft + m_trackInset;
	const float trackRight = _scroll.m_trackLeft + _scroll.m_trackW - m_trackInset;

	const bool isOnTrack = MouseHit({ (trackLeft + trackRight) / 2.0f, _scroll.m_trackY }, trackRight - trackLeft, m_trackH);
	const bool isOnThumb = isOnTrack && mx >= _scroll.m_thumbX && mx <= _scroll.m_thumbX + _scroll.m_thumbW;

	//---------- ドラッグ開始 ----------
	if (isOnThumb && !_scroll.m_dragFlg)
	{
		MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
		if (m_clickFlg)
		{
			_scroll.m_dragFlg = true;
			_scroll.m_dragOffsetX = mx - _scroll.m_thumbX;
			m_clickFlg = false;
		}
	}

	//---------- ドラッグ中 ----------
	if (_scroll.m_dragFlg)
	{
		if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
		{
			_scroll.m_dragFlg = false;
		}
		else
		{
			MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);

			const float moveRange = (trackRight - _scroll.m_thumbW) - trackLeft;
			float ratio = 0.0f;
			if (moveRange > 0.0f)
			{
				ratio = (mx - _scroll.m_dragOffsetX - trackLeft) / moveRange;
			}
			ratio = std::clamp(ratio, 0.0f, 1.0f);
			_scroll.m_index = static_cast<int>(std::round(ratio * maxIndex));
		}
	}
	//---------- レールをクリック → 1画面分動く ----------
	else if (isOnTrack && !isOnThumb)
	{
		MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
		if (m_clickFlg)
		{
			_scroll.m_index += (mx < _scroll.m_thumbX) ? -_scroll.m_visibleNum : _scroll.m_visibleNum;
			m_clickFlg = false;
		}
	}

	//---------- マウスホイール(そのパネルの上にあるときだけ) ----------
	if (!_scroll.m_dragFlg && MouseHit(_panelPos, _panelW, _panelH))
	{
		int wheel = MouseInfo::Instance().GetMouseWheelValue();
		//奥に回す → 左へ
		if (wheel >= 1)
		{
			_scroll.m_index -= 1;
		}
		//手前に回す → 右へ
		else if (wheel <= -1)
		{
			_scroll.m_index += 1;
		}
	}

	_scroll.m_hoverFlg = isOnThumb || _scroll.m_dragFlg;
	_scroll.m_index = std::clamp(_scroll.m_index, 0, maxIndex);
}

void ShopDisplay::ScrollDraw(const HScroll& _scroll, ShopTex _track)
{
	// 左端を基準に、横に並べて描く
	const Math::Vector2 pivotLeft = { 0.0f, 0.5f };

	DrawTexSize(m_spTex[static_cast<int>(_track)], { _scroll.m_trackLeft, _scroll.m_trackY }, _scroll.m_trackW, m_trackH, pivotLeft);

	const bool hover = _scroll.m_hoverFlg;
	const auto& spL = m_spTex[static_cast<int>(hover ? ShopTex::ThumbLeftHover : ShopTex::ThumbLeft)];
	const auto& spM = m_spTex[static_cast<int>(hover ? ShopTex::ThumbMidHover : ShopTex::ThumbMid)];
	const auto& spR = m_spTex[static_cast<int>(hover ? ShopTex::ThumbRightHover : ShopTex::ThumbRight)];
	const auto& spG = m_spTex[static_cast<int>(hover ? ShopTex::ThumbGripHover : ShopTex::ThumbGrip)];
	if (!spL || !spM || !spR || !spG) { return; }

	const float lw = static_cast<float>(spL->GetWidth());
	const float rw = static_cast<float>(spR->GetWidth());
	const float y = _scroll.m_trackY;

	DrawTexSize(spL, { _scroll.m_thumbX, y }, lw, m_trackH, pivotLeft);
	const float midW = _scroll.m_thumbW - lw - rw;
	if (midW > 0.0f)
	{
		DrawTexSize(spM, { _scroll.m_thumbX + lw, y }, midW, m_trackH, pivotLeft);
	}
	DrawTexSize(spR, { _scroll.m_thumbX + _scroll.m_thumbW - rw, y }, rw, m_trackH, pivotLeft);

	// 持ち手はつまみの真ん中
	DrawTex(spG, { _scroll.m_thumbX + _scroll.m_thumbW / 2.0f, y });
}

void ShopDisplay::BuySellUpdate()
{
	if (m_selectIndex >= GetListNum()) { return; }

	const int buyRow = IsStackTab() ? 2 : 1;
	const int sellRow = buyRow + 1;

	const Math::Vector2 buyPos = { m_infoButtonX, GetInfoRowY(buyRow) };
	const Math::Vector2 sellPos = { m_infoButtonX, GetInfoRowY(sellRow) };

	bool success = false;
	if (m_clickFlg && MouseHit(buyPos, m_buttonW, m_buttonH))
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
	if (IsStackTab() && m_clickFlg && MouseHit(sellPos, m_buttonW, m_buttonH))
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
	//商品(値段・在庫)
	ShopItem* items = nullptr;

	//持っている数
	int* have = nullptr;

	//商品の種類数
	int num = 0;

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
		num = MaterialTypeMax;
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
	const long total = static_cast<long>(item.m_buyPrice) * _count;

	// 在庫が足りない
	if (item.m_stock < _count) { return false; }

	// お金が足りない
	if (m_money < total) { return false; }

	//---------- 買う ----------
	//お金を払う
	m_money -= total;

	//お店の在庫が減る
	item.m_stock -= _count;

	//自分の持ち物が増える
	have[_index] += _count;

	return true;
}

bool ShopDisplay::SellStackItem(int _index, int _count)
{
	//---------- 今のタブの配列を決める ----------
	// 消耗品と素材で配列が違うので、ポインタで同じように扱う
	//商品(値段・在庫)
	ShopItem* items = nullptr;

	//持っている数
	int* have = nullptr;

	//商品の種類数
	int num = 0;

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
		num = MaterialTypeMax;
	}
	else
	{
		// 武器は売れない
		return false;
	}

	//----------  売れるか調べる ----------
	// 番号が範囲外
	if (_index < 0 || _index >= num) { return false; }

	// 0個以下は売れない
	if (_count <= 0) { return false; }

	ShopItem& item = items[_index];

	// 合計の値段
	const long total = static_cast<long>(item.m_sellPrice) * _count;

	// 所持量が足りない
	if (have[_index] < _count) { return false; }

	//---------- 売る ----------
	//お金をもらう
	m_money += total;

	//お店の在庫が増える
	item.m_stock += _count;

	//自分の持ち物が減る
	have[_index] -= _count;

	return true;
}

bool ShopDisplay::BuyWeapon(int _id)
{
	//---------- 買えるか調べる ----------
	//番号が範囲外
	if (_id < 0 || _id >= PlayerWeaponInfoSize) { return false; }

	//書き換えるので参照(&)で取る
	ShopItem& item = m_weaponItem[_id];

	//もう持っている(同じ武器は2本いらない)
	if (m_weaponOwned[_id]) { return false; }

	//在庫がない(未入荷)
	if (item.m_stock <= 0) { return false; }

	//お金が足りない
	if (m_money < item.m_buyPrice) { return false; }

	//---------- 買う ----------

	//お金を払う
	m_money -= item.m_buyPrice;

	//お店の在庫が減る
	item.m_stock -= 1;

	//持っている武器になる
	m_weaponOwned[_id] = true;

	return true;
}

void ShopDisplay::SaveShopData()
{
	//お店の在庫(ShopStockData.json)
	nlohmann::json stock = ReadJson(m_shopStockPath, false);
	WriteStock(stock, "potions", m_potionItem, PotionsTypeMax);
	WriteStock(stock, "materials", m_materialItem, MaterialTypeMax);
	WriteStock(stock, "weapons", m_weaponItem, PlayerWeaponInfoSize);
	WriteJson(m_shopStockPath, stock);

	//持ち物とお金(PlayerInventoryData.json)
	nlohmann::json inventory = ReadJson(m_inventoryPath, false);
	WriteHave(inventory, "potions", m_potionHave, PotionsTypeMax);
	WriteHave(inventory, "weaponStrengtheningMaterials", m_materialHave, MaterialTypeMax);
	WriteOwned(inventory, "weapons", m_weaponOwned, PlayerWeaponInfoSize);
	inventory["money"] = m_money;
	WriteJson(m_inventoryPath, inventory);
}

nlohmann::json ShopDisplay::ReadJson(const std::string& _path, bool _isLog)
{
	std::ifstream file(_path);
	if (!file)
	{
		if (_isLog)
		{
			KdDebugGUI::Instance().AddLog(U8("[Error] ShopDisplayのファイルが見つかりません : %s\n"), _path.c_str());
		}
		return nlohmann::json::object();
	}

	nlohmann::json data = nlohmann::json::parse(file, nullptr, false);
	if (data.is_discarded()) { return nlohmann::json::object(); }

	return data;
}

void ShopDisplay::WriteJson(const std::string& _path, const nlohmann::json& _data)
{
	std::ofstream file(_path);
	if (!file)
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] ShopDisplayの保存に失敗しました : %s\n"), _path.c_str());
		return;
	}

	//4 … 字下げの幅(今のファイルと同じ見た目)
	file << _data.dump(4);
}

void ShopDisplay::ReadPrice(const nlohmann::json& _master, const char* _key, ShopItem* _items, int _num)
{
	if (!_master.contains(_key)) { return; }

	for (const auto& item : _master[_key])
	{
		int id = item.value("id", -1);
		if (id < 0 || id >= _num) { continue; }
		_items[id].m_buyPrice = item.value("buyPrice", 0);
		_items[id].m_sellPrice = item.value("sellPrice", 0);
	}
}

void ShopDisplay::ReadStock(const nlohmann::json& _stock, const char* _key, ShopItem* _items, int _num)
{
	if (!_stock.contains(_key)) { return; }

	for (const auto& item : _stock[_key])
	{
		int id = item.value("id", -1);
		if (id < 0 || id >= _num) { continue; }
		_items[id].m_stock = item.value("stock", 0);
	}
}

void ShopDisplay::ReadHave(const nlohmann::json& _inventory, const char* _key, int* _have, int _num)
{
	for (int i = 0; i < _num; i++)
	{
		_have[i] = 0;
	}

	if (!_inventory.contains(_key)) { return; }

	for (const auto& item : _inventory[_key])
	{
		int id = item.value("id", -1);
		if (id < 0 || id >= _num) { continue; }
		_have[id] = item.value("num", 0);
	}
}

void ShopDisplay::WriteStock(nlohmann::json& _data, const char* _key, const ShopItem* _items, int _num)
{
	if (!_data.contains(_key) || !_data[_key].is_array()) { return; }

	for (auto& item : _data[_key])
	{
		int id = item.value("id", -1);
		if (id < 0 || id >= _num) { continue; }
		item["stock"] = _items[id].m_stock;
	}
}

void ShopDisplay::WriteHave(nlohmann::json& _data, const char* _key, const int* _have, int _num)
{
	if (!_data.contains(_key) || !_data[_key].is_array()) { return; }

	for (auto& item : _data[_key])
	{
		int id = item.value("id", -1);
		if (id < 0 || id >= _num) { continue; }
		item["num"] = _have[id];
	}
}

void ShopDisplay::WriteOwned(nlohmann::json& _data, const char* _key, const bool* _owned, int _num)
{
	if (!_data.contains(_key) || !_data[_key].is_array()) { return; }

	for (auto& item : _data[_key])
	{
		int id = item.value("id", -1);
		if (id < 0 || id >= _num) { continue; }
		item["owned"] = _owned[id];
	}
}
