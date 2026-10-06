#pragma once

enum ItemType
{
	Potion,
	Weapon
};


struct LootItem {
	int type;
	int id;
	std::string name;
	std::string rank;
};

struct LootTable {
	std::unordered_map<std::string, float> rankWeights;
	std::unordered_map<std::string, std::vector<LootItem>> slots;
};

class LootTableManager {
public:
	LootTableManager() {
			Load(m_path);
	};
	~LootTableManager() {};

	void Load(const std::string& path);
	LootItem GetRandomLoot(std::string slotName) const;

private:
	LootTable m_table;

	std::string m_path = "Asset/Data/ObjeData/Item/TreasureChestTable/slot.json";
};
