//---- 自分のヘッダー ----
#include "LootTableManager.h"		//宝箱のドロップテーブル管理

//---- ポーション ----
#include"../../../../Potions/PotionsType.h"	//ポーションの種類

//=============================================
// 取得
//=============================================

std::shared_ptr<LootTableManager> LootTableManager::GetShared()
{
	static std::shared_ptr<LootTableManager> spInstance = std::make_shared<LootTableManager>();
	return spInstance;
}

LootItem LootTableManager::GetRandomLoot(const std::string& _tableName) const
{
	auto it = m_tables.find(_tableName);
	if (it == m_tables.end() || it->second.m_items.empty() || it->second.m_totalWeight <= 0.0f)
	{
		return LootItem(); // type = -1（何も出ない）
	}

	const LootTable& table = it->second;

	float r = KdRandom::GetFloat(0.0f, table.m_totalWeight);
	float accum = 0.0f;

	for (const LootItem& item : table.m_items)
	{
		accum += item.m_weight;
		if (r <= accum)
		{
			return item;
		}
	}

	return table.m_items.back();
}

int LootTableManager::GetRandomDropNum(const std::string& _tableName) const
{
	auto it = m_tables.find(_tableName);
	if (it == m_tables.end()) { return 0; }

	return KdRandom::GetInt(it->second.m_dropMin, it->second.m_dropMax);
}

//=============================================
// その他の処理
//=============================================

void LootTableManager::Load(const std::string& _path)
{
	m_path = _path;

	std::ifstream ifs(_path);
	if (!ifs.is_open()) {
		std::cerr << "File open error: " << _path << std::endl;
		return;
	}

	nlohmann::json j = nlohmann::json::parse(ifs, nullptr, false);
	if (j.is_discarded() || !j.is_object()) {
		std::cerr << "JSON parse error: " << _path << std::endl;
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
		table.m_dropMin = (std::max)(0, tableData.value("dropMin", 1));
		table.m_dropMax = (std::max)(table.m_dropMin, tableData.value("dropMax", table.m_dropMin));

		if (tableData.contains("items") && tableData["items"].is_array())
		{
			for (auto& itemData : tableData["items"])
			{
				LootItem item;
				item.m_type = itemData.value("type", -1);
				item.m_id = itemData.value("id", 0);
				item.m_name = itemData.value("name", std::string{});

				// ポーションは名前で番号を探す(消したポーションは出さない)
				if (item.m_type == ItemType::Potion)
				{
					item.m_id = GetJsonPotionID(itemData);
					if (item.m_id < 0) { continue; }
				}
				item.m_weight = (std::max)(0.0f, itemData.value("weight", 0.0f));

				if (item.m_type < 0 || item.m_weight <= 0.0f) { continue; } // 出ないものは入れない

				table.m_totalWeight += item.m_weight;
				table.m_items.push_back(item);
			}
		}

		m_tables[tableName] = table;
	}
}

void LootTableManager::LoadOldFormat(const nlohmann::json& _j)
{
	std::unordered_map<std::string, float> rankWeights;
	if (_j.contains("rank_weights") && _j["rank_weights"].is_object())
	{
		for (auto& [rank, weight] : _j["rank_weights"].items()) {
			rankWeights[rank] = weight.get<float>();
		}
	}

	for (auto& [slotName, typeObj] : _j["slots"].items())
	{
		LootTable table;
		const int lv = slotName.size() > 4 ? std::atoi(slotName.c_str() + 4) : 0;
		table.m_dropMin = 2;
		table.m_dropMax = 4 + lv; // 旧仕様の個数（2〜4+Lv）

		for (auto& [typeName, itemArray] : typeObj.items())
		{
			for (auto& itemData : itemArray)
			{
				LootItem item;
				item.m_type = itemData.value("type", -1);
				item.m_id = itemData.value("id", 0);
				item.m_name = itemData.value("name", std::string{});

				// ポーションは名前で番号を探す(消したポーションは出さない)
				if (item.m_type == ItemType::Potion)
				{
					item.m_id = GetJsonPotionID(itemData);
					if (item.m_id < 0) { continue; }
				}

				auto it = rankWeights.find(itemData.value("rank", std::string{}));
				item.m_weight = (it != rankWeights.end()) ? it->second : 0.0f;

				if (item.m_type < 0 || item.m_weight <= 0.0f) { continue; }

				table.m_totalWeight += item.m_weight;
				table.m_items.push_back(item);
			}
		}

		m_tables[slotName] = table;
	}
}
