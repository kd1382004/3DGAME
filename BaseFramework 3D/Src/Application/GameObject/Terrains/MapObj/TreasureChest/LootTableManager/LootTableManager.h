#pragma once

struct LootItem {
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
	LootTableManager() { Load(path); };
	~LootTableManager() {};

	void Load(const std::string& path);
	LootItem GetRandomLoot(std::string slotName) const;

private:
	LootTable m_table;

	std::string path = "Asset/Data/ObjeData/Item/treasure_slot1_table.json";
};
