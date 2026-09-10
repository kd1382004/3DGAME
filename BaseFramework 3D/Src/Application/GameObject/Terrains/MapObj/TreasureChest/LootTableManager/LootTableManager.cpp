#include "LootTableManager.h"

void LootTableManager::Load(const std::string& path)
{
	std::ifstream ifs(path);
	nlohmann::json j;
	ifs >> j;

	// rank_weights 読み取り
	for (auto& [rank, weight] : j["rank_weights"].items()) {
		m_table.rankWeights[rank] = weight.get<float>();
	}

	// slots 読み取り（動的）
	for (auto& [slotName, itemArray] : j["slots"].items()) {

		std::vector<LootItem> slotItems;

		for (auto& item : itemArray) {
			LootItem li;
			li.id = item["id"].get<int>();
			li.name = item["name"].get<std::string>();
			li.rank = item["rank"].get<std::string>();
			slotItems.push_back(li);
		}

		m_table.slots[slotName] = slotItems;
	}
}

LootItem LootTableManager::GetRandomLoot(std::string slotName) const
{
	const auto& items = m_table.slots.at(slotName);

	float totalWeight = 0.0f;
	for (auto& item : items) {
		totalWeight += m_table.rankWeights.at(item.rank);
	}

	float r = KdRandom::GetFloat(0.0f, totalWeight);;
	float accum = 0.0f;

	for (auto& item : items) {
		accum += m_table.rankWeights.at(item.rank);
		if (r <= accum) {
			return item;
		}
	}

	return items.back();
}
