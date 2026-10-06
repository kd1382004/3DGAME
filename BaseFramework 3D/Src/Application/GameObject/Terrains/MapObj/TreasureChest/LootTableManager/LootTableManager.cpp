#include "LootTableManager.h"

void LootTableManager::Load(const std::string& path)
{
	std::ifstream ifs(path);
	if (!ifs.is_open()) {
		std::cerr << "File open error: " << path << std::endl;
		return;
	}

	nlohmann::json j;
	try {
		ifs >> j;
	}
	catch (const std::exception& e) {
		std::cerr << "JSON parse error: " << e.what() << std::endl;
		return;
	}

	// rank_weights 読み取り
	for (auto& [rank, weight] : j["rank_weights"].items()) {
		m_table.rankWeights[rank] = weight.get<float>();
	}

	// slots 読み取り（type0/type1）
	for (auto& [slotName, typeObj] : j["slots"].items()) {

		for (auto& [typeName, itemArray] : typeObj.items()) {

			std::vector<LootItem> slotItems;

			for (auto& item : itemArray) {
				LootItem li;
				li.id = item["id"].get<int>();
				li.name = item["name"].get<std::string>();
				li.rank = item["rank"].get<std::string>();
				li.type = item["type"].get<int>();
				slotItems.push_back(li);
			}

			m_table.slots[slotName + "_" + typeName] = slotItems;
		}
	}
}

LootItem LootTableManager::GetRandomLoot(std::string slotName) const
{
	std::vector<LootItem> merged;

	// slot0_type0
	auto it0 = m_table.slots.find(slotName + "_type0");
	if (it0 != m_table.slots.end()) {
		merged.insert(merged.end(), it0->second.begin(), it0->second.end());
	}

	// slot0_type1
	auto it1 = m_table.slots.find(slotName + "_type1");
	if (it1 != m_table.slots.end()) {
		merged.insert(merged.end(), it1->second.begin(), it1->second.end());
	}

	// どちらも無かったら例外
	if (merged.empty()) {
		throw std::runtime_error("Slot not found: " + slotName);
	}

	// 重み計算
	float totalWeight = 0.0f;
	for (auto& item : merged) {
		totalWeight += m_table.rankWeights.at(item.rank);
	}

	float r = KdRandom::GetFloat(0.0f, totalWeight);
	float accum = 0.0f;

	for (auto& item : merged) {
		accum += m_table.rankWeights.at(item.rank);
		if (r <= accum) {
			return item;
		}
	}

	return merged.back();
}
