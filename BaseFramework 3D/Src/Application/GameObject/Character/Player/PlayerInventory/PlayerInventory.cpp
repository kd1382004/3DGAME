#include "PlayerInventory.h"
#include"../../../Potions/PotionsType.h"


#include"../../../UI/ItemGetUIController/ItemGetUIController.h"
void PlayerInventory::Init()
{
	std::vector<Inventory> inventoryList;

	std::ifstream ifs("Asset/Data/ObjeData/Item/Potions/PotionsName.json");
	nlohmann::json jsonData = nlohmann::json::parse(ifs, nullptr, false);

	for (auto& item : jsonData["inventory"])
	{
		Inventory inv;
		inv.m_ID = item["id"].get<int>();
		inv.m_name = item["name"].get<std::string>();
		inv.m_num = 0;
		inventoryList.push_back(inv);
	}

	// ID順にソート
	std::sort(inventoryList.begin(), inventoryList.end(),
		[](const Inventory& a, const Inventory& b)
		{
			return a.m_ID < b.m_ID;
		});

	m_potionsInventory = inventoryList;
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
	spItemGetUIController->AddGetItemList(item);
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
