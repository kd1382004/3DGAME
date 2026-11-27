//---- 自分のヘッダー ----
#include "PlayerInventory.h"										//プレイヤーの所持品

//---- ポーション ----
#include"../../../Potions/PotionsType.h"							//ポーションの種類

//---- UI ----
#include"../../../UI/ItemGetUIController/ItemGetUIController.h"		//アイテム入手の表示

//=============================================
// 基本の処理
//=============================================

void PlayerInventory::Init()
{
	{
		// ポーションの一覧は表(kPotionDefs)から作る
		std::vector<Inventory> potionInventoryList;
		for (int id = 0; id < PotionsTypeMax; id++)
		{
			Inventory inv;
			inv.m_ID = id;
			inv.m_name = GetPotionSTName(id);
			inv.m_num = 0;
			potionInventoryList.push_back(inv);
		}

		m_potionsInventory = potionInventoryList;

	}

	{
		std::vector<Inventory> weaponsStrengtheningInventoryList;

		std::ifstream ifs("Asset/Data/ObjeData/Item/WeaponsStrengtheningItem/WeaponsStrengtheningItemName.json");
		nlohmann::json jsonData = nlohmann::json::parse(ifs, nullptr, false);

		for (auto& item : jsonData["inventory"])
		{
			Inventory inv;
			inv.m_ID = item["id"].get<int>();
			inv.m_name = item["name"].get<std::string>();
			inv.m_num = 0;
			weaponsStrengtheningInventoryList.push_back(inv);
		}

		// ID順にソート
		std::sort(weaponsStrengtheningInventoryList.begin(), weaponsStrengtheningInventoryList.end(),
			[](const Inventory& _a, const Inventory& _b)
			{
				return _a.m_ID < _b.m_ID;
			});

		m_weaponsStrengtheningInventory = weaponsStrengtheningInventoryList;
	}
}

//=============================================
// 取得
//=============================================

int PlayerInventory::GetPotionsInventoryNum(int _potionsType)
{
	if (_potionsType<0 || _potionsType>m_potionsInventory.size() - 1) { return 0; }
	return m_potionsInventory[_potionsType].m_num;
}

int PlayerInventory::GetWeaponsStrengtheningInventoryNum(int _weaponsStrengtheningType)
{
	if (_weaponsStrengtheningType < 0 || _weaponsStrengtheningType >= m_weaponsStrengtheningInventory.size()) { return 0; }
	return m_weaponsStrengtheningInventory[_weaponsStrengtheningType].m_num;
}

//=============================================
// 設定
//=============================================

bool PlayerInventory::GetQuickSlotFlg(int _potionsType)
{
	if (_potionsType<0 || _potionsType>m_potionsInventory.size() - 1) { return false; }
	return m_potionsInventory[_potionsType].ItemBarFlg;
}

bool PlayerInventory::IsQuickSlotFull() const
{
	return (m_potionsItemBarInventory.size() >= QUICK_SLOT_MAX);
}

void PlayerInventory::AddPotionsInventory(int _potionsType)
{
	if (_potionsType<0 || _potionsType>m_potionsInventory.size() - 1) { return; }
	m_potionsInventory[_potionsType].m_num++;

	//入手アクションを起こす
	std::shared_ptr<ItemGetUIController>spItemGetUIController = m_wpItemGetUIController.lock();
	if (!spItemGetUIController) { return; }

	GetItem item;
	item.m_getNum = 1;
	item.m_ID = _potionsType;
	spItemGetUIController->AddGetItemList(item, ItemType::Potion);


	RefreshQuickSlot();
}

void PlayerInventory::AddQuickSlot(int _potionsType, bool _flg)
{
	if (_potionsType<0 || _potionsType>m_potionsInventory.size() - 1) { return; }

	//もしも登録なら上限チェック
	if (_flg == true && m_potionsItemBarInventory.size() >= QUICK_SLOT_MAX)
	{
		return;
	}

	m_potionsInventory[_potionsType].ItemBarFlg = _flg;

	RefreshQuickSlot();
}

void PlayerInventory::AddQuickSlot(int _potionsType)
{
	AddQuickSlot(_potionsType, !m_potionsInventory[_potionsType].ItemBarFlg);
}

void PlayerInventory::AddWeaponsStrengtheningInventory(int _weaponsStrengtheningType)
{
	if (_weaponsStrengtheningType < 0 || _weaponsStrengtheningType >= m_weaponsStrengtheningInventory.size()) { return; }
	m_weaponsStrengtheningInventory[_weaponsStrengtheningType].m_num++;


	//入手アクションを起こす
	std::shared_ptr<ItemGetUIController>spItemGetUIController = m_wpItemGetUIController.lock();
	if (!spItemGetUIController) { return; }

	GetItem item;
	item.m_getNum = 1;
	item.m_ID = _weaponsStrengtheningType;
	spItemGetUIController->AddGetItemList(item, ItemType::WeaponsStrengthening);
}

void PlayerInventory::SetPotionsInventoryList(const std::vector<Inventory>& _potions)
{
	for (auto potions : _potions)
	{
		if (potions.m_ID < 0 || potions.m_ID >= m_potionsInventory.size()) { continue; }
		m_potionsInventory[potions.m_ID].m_num = potions.m_num;
		m_potionsInventory[potions.m_ID].ItemBarFlg = potions.ItemBarFlg;
	}


	RefreshQuickSlot();
}

void PlayerInventory::SetWeaponsStrengtheningInventoryList(const std::vector<Inventory>& _weapons)
{
	for (auto weapons : _weapons)
	{
		if (weapons.m_ID < 0 || weapons.m_ID >= m_weaponsStrengtheningInventory.size()) { continue; }
		m_weaponsStrengtheningInventory[weapons.m_ID].m_num = weapons.m_num;
	}
}

//=============================================
// その他の処理
//=============================================

void PlayerInventory::UsePotionsInventory(int _potionsType)
{
	if (_potionsType<0 || _potionsType>m_potionsInventory.size() - 1) { return; }
	m_potionsInventory[_potionsType].m_num--;

	RefreshQuickSlot();
}

void PlayerInventory::UseWeaponsStrengtheningInventory(int _weaponsStrengtheningType, int _num)
{
	if (_weaponsStrengtheningType < 0 || _weaponsStrengtheningType >= m_weaponsStrengtheningInventory.size()) { return; }
	m_weaponsStrengtheningInventory[_weaponsStrengtheningType].m_num -= _num;
}

void PlayerInventory::RefreshQuickSlot()
{
	m_potionsItemBarInventory.clear();

	for (const auto& list : m_potionsInventory)
	{
		if (list.ItemBarFlg)
		{
			m_potionsItemBarInventory.push_back(list);
		}
	}
}
