#include "PlayerInventory.h"
#include"../../../Potions/PotionsType.h"


#include"../../../UI/ItemGetUIController/ItemGetUIController.h"
void PlayerInventory::Init()
{
	{
		std::vector<Inventory> PotionInventoryList;

		std::ifstream ifs("Asset/Data/ObjeData/Item/Potions/PotionsName.json");
		nlohmann::json jsonData = nlohmann::json::parse(ifs, nullptr, false);

		for (auto& item : jsonData["inventory"])
		{
			Inventory inv;
			inv.m_ID = item["id"].get<int>();
			inv.m_name = item["name"].get<std::string>();
			inv.m_num = 0;
			PotionInventoryList.push_back(inv);
		}

		// ID順にソート
		std::sort(PotionInventoryList.begin(), PotionInventoryList.end(),
			[](const Inventory& a, const Inventory& b)
			{
				return a.m_ID < b.m_ID;
			});

		m_potionsInventory = PotionInventoryList;

	}

	{
		std::vector<Inventory> WeaponsStrengtheningInventoryList;

		std::ifstream ifs("Asset/Data/ObjeData/Item/WeaponsStrengtheningItem/WeaponsStrengtheningItemName.json");
		nlohmann::json jsonData = nlohmann::json::parse(ifs, nullptr, false);

		for (auto& item : jsonData["inventory"])
		{
			Inventory inv;
			inv.m_ID = item["id"].get<int>();
			inv.m_name = item["name"].get<std::string>();
			inv.m_num = 0;
			WeaponsStrengtheningInventoryList.push_back(inv);
		}

		// ID順にソート
		std::sort(WeaponsStrengtheningInventoryList.begin(), WeaponsStrengtheningInventoryList.end(),
			[](const Inventory& a, const Inventory& b)
			{
				return a.m_ID < b.m_ID;
			});

		m_weaponsStrengtheningInventory = WeaponsStrengtheningInventoryList;
	}
}

void PlayerInventory::AddPotionsInventory(int _PotionsType)
{
	if (_PotionsType<0 || _PotionsType>m_potionsInventory.size() - 1) { return; }
	m_potionsInventory[_PotionsType].m_num++;

	//入手アクションを起こす
	std::shared_ptr<ItemGetUIController>spItemGetUIController = m_wpItemGetUIController.lock();
	if (!spItemGetUIController) { return; }

	GetItem item;
	item.GetNum = 1;
	item.ID = _PotionsType;
	spItemGetUIController->AddGetItemList(item, ItemType::Potion);
}

void PlayerInventory::UsePotionsInventory(int _PotionsType)
{
	if (_PotionsType<0 || _PotionsType>m_potionsInventory.size() - 1) { return; }
	m_potionsInventory[_PotionsType].m_num--;
}

int PlayerInventory::GetPotionsInventoryNum(int _PotionsType)
{
	if (_PotionsType<0 || _PotionsType>m_potionsInventory.size() - 1) { return 0; }
	return m_potionsInventory[_PotionsType].m_num;
}

void PlayerInventory::AddWeaponsStrengtheningInventory(int _WeaponsStrengtheningType)
{
	if (_WeaponsStrengtheningType < 0 || _WeaponsStrengtheningType >= m_weaponsStrengtheningInventory.size()) { return; }
	m_weaponsStrengtheningInventory[_WeaponsStrengtheningType].m_num++;


	//入手アクションを起こす
	std::shared_ptr<ItemGetUIController>spItemGetUIController = m_wpItemGetUIController.lock();
	if (!spItemGetUIController) { return; }

	GetItem item;
	item.GetNum = 1;
	item.ID = _WeaponsStrengtheningType;
	spItemGetUIController->AddGetItemList(item, ItemType::WeaponsStrengthening);
}

void PlayerInventory::UseWeaponsStrengtheningInventory(int _WeaponsStrengtheningType, int _num)
{
	if (_WeaponsStrengtheningType < 0 || _WeaponsStrengtheningType >= m_weaponsStrengtheningInventory.size()) { return; }
	m_weaponsStrengtheningInventory[_WeaponsStrengtheningType].m_num -= _num;
}

int PlayerInventory::GetWeaponsStrengtheningInventoryNum(int _WeaponsStrengtheningType)
{
	if (_WeaponsStrengtheningType < 0 || _WeaponsStrengtheningType >= m_weaponsStrengtheningInventory.size()) { return 0; }
	return m_weaponsStrengtheningInventory[_WeaponsStrengtheningType].m_num;
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

