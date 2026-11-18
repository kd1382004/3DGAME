#include "PlayerInventory.h"
#include"../../../Potions/PotionsType.h"


#include"../../../UI/ItemGetUIController/ItemGetUIController.h"

void PlayerInventory::Init()
{
	{
		std::vector<Inventory> potionInventoryList;

		std::ifstream ifs("Asset/Data/ObjeData/Item/Potions/PotionsName.json");
		nlohmann::json jsonData = nlohmann::json::parse(ifs, nullptr, false);

		for (auto& item : jsonData["inventory"])
		{
			Inventory inv;
			inv.m_ID = item["id"].get<int>();
			inv.m_name = item["name"].get<std::string>();
			inv.m_num = 0;
			potionInventoryList.push_back(inv);
		}

		// ID順にソート
		std::sort(potionInventoryList.begin(), potionInventoryList.end(),
			[](const Inventory& _a, const Inventory& _b)
			{
				return _a.m_ID < _b.m_ID;
			});

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
	}
}

void PlayerInventory::SetWeaponsStrengtheningInventoryList(const std::vector<Inventory>&_weapons)
{
	for (auto weapons : _weapons)
	{
		if (weapons.m_ID < 0 || weapons.m_ID >= m_weaponsStrengtheningInventory.size()) { continue; }
		m_weaponsStrengtheningInventory[weapons.m_ID].m_num = weapons.m_num;
	}
}

void PlayerInventory::UsePotionsInventory(int _potionsType)
{
	if (_potionsType<0 || _potionsType>m_potionsInventory.size() - 1) { return; }
	m_potionsInventory[_potionsType].m_num--;
}

void PlayerInventory::UseWeaponsStrengtheningInventory(int _weaponsStrengtheningType, int _num)
{
	if (_weaponsStrengtheningType < 0 || _weaponsStrengtheningType >= m_weaponsStrengtheningInventory.size()) { return; }
	m_weaponsStrengtheningInventory[_weaponsStrengtheningType].m_num -= _num;
}

