#include "LootTableManager.h"

std::shared_ptr<LootTableManager> LootTableManager::GetShared()
{
	static std::shared_ptr<LootTableManager> spInstance = std::make_shared<LootTableManager>();
	return spInstance;
}

void LootTableManager::Load(const std::string& path)
{
	m_path = path;

	std::ifstream ifs(path);
	if (!ifs.is_open()) {
		std::cerr << "File open error: " << path << std::endl;
		return;
	}

	nlohmann::json j = nlohmann::json::parse(ifs, nullptr, false);
	if (j.is_discarded() || !j.is_object()) {
		std::cerr << "JSON parse error: " << path << std::endl;
		return;
	}

	// 読み込みに成功したときだけ入れ替える
	m_tables.clear();

	if (!j.contains("tables") && j.contains("slots"))
	{
		LoadOldFormat(j);
		return;
	}

	if (!j.contains("tables") || !j["tables"].is_object()) { return; }

	for (auto& [tableName, tableData] : j["tables"].items())
	{
		if (!tableData.is_object()) { continue; }

		LootTable table;
		table.dropMin = (std::max)(0, tableData.value("dropMin", 1));
		table.dropMax = (std::max)(table.dropMin, tableData.value("dropMax", table.dropMin));

		if (tableData.contains("items") && tableData["items"].is_array())
		{
			for (auto& itemData : tableData["items"])
			{
				LootItem item;
				item.type = itemData.value("type", -1);
				item.id = itemData.value("id", 0);
				item.name = itemData.value("name", std::string{});
				item.weight = (std::max)(0.0f, itemData.value("weight", 0.0f));

				if (item.type < 0 || item.weight <= 0.0f) { continue; } // 出ないものは入れない

				table.totalWeight += item.weight;
				table.items.push_back(item);
			}
		}

		m_tables[tableName] = table;
	}
}

void LootTableManager::LoadOldFormat(const nlohmann::json& j)
{
	std::unordered_map<std::string, float> rankWeights;
	if (j.contains("rank_weights") && j["rank_weights"].is_object())
	{
		for (auto& [rank, weight] : j["rank_weights"].items()) {
			rankWeights[rank] = weight.get<float>();
		}
	}

	for (auto& [slotName, typeObj] : j["slots"].items())
	{
		LootTable table;
		const int lv = slotName.size() > 4 ? std::atoi(slotName.c_str() + 4) : 0;
		table.dropMin = 2;
		table.dropMax = 4 + lv; // 旧仕様の個数（2〜4+Lv）

		for (auto& [typeName, itemArray] : typeObj.items())
		{
			for (auto& itemData : itemArray)
			{
				LootItem item;
				item.type = itemData.value("type", -1);
				item.id = itemData.value("id", 0);
				item.name = itemData.value("name", std::string{});

				auto it = rankWeights.find(itemData.value("rank", std::string{}));
				item.weight = (it != rankWeights.end()) ? it->second : 0.0f;

				if (item.type < 0 || item.weight <= 0.0f) { continue; }

				table.totalWeight += item.weight;
				table.items.push_back(item);
			}
		}

		m_tables[slotName] = table;
	}
}

LootItem LootTableManager::GetRandomLoot(const std::string& tableName) const
{
	auto it = m_tables.find(tableName);
	if (it == m_tables.end() || it->second.items.empty() || it->second.totalWeight <= 0.0f)
	{
		return LootItem(); // type = -1（何も出ない）
	}

	const LootTable& table = it->second;

	float r = KdRandom::GetFloat(0.0f, table.totalWeight);
	float accum = 0.0f;

	for (const LootItem& item : table.items)
	{
		accum += item.weight;
		if (r <= accum)
		{
			return item;
		}
	}

	return table.items.back();
}

int LootTableManager::GetRandomDropNum(const std::string& tableName) const
{
	auto it = m_tables.find(tableName);
	if (it == m_tables.end()) { return 0; }

	return KdRandom::GetInt(it->second.dropMin, it->second.dropMax);
}
