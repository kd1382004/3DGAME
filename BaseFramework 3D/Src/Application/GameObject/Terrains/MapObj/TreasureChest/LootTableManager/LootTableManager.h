#pragma once

enum ItemType
{
	Potion,	// 0 : ポーション（PotionsName.json の id）
	Weapon	// 1 : 武器強化素材（WeaponsStrengtheningItemName.json の id）
};


struct LootItem
{
	int type = -1;		// ItemType（-1 = 何も出ない）
	int id = 0;
	std::string name;
	float weight = 0.0f; // 出やすさ（テーブル内の合計に対する割合が出現率になる）
};

struct LootTable
{
	int dropMin = 1;	// 1回開けたときに出る個数の最小
	int dropMax = 1;	// 1回開けたときに出る個数の最大
	std::vector<LootItem> items;
	float totalWeight = 0.0f;
};

// 宝箱のドロップテーブル（Asset/Data/ObjeData/Item/TreasureChestTable/slot.json）
class LootTableManager
{
public:
	LootTableManager() {
		Load(m_path);
	};
	~LootTableManager() {};

	// ゲーム中で共有するインスタンス（エディタで保存したときにここを読み直す）
	static std::shared_ptr<LootTableManager> GetShared();

	void Load(const std::string& path);
	void Reload() { Load(m_path); }

	// テーブルから重みに従って1つ選ぶ。テーブルがなければ type = -1
	LootItem GetRandomLoot(const std::string& tableName) const;

	// 1回開けたときに出る個数（dropMin〜dropMax）。テーブルがなければ 0
	int GetRandomDropNum(const std::string& tableName) const;

	bool HasTable(const std::string& tableName) const { return m_tables.count(tableName) > 0; }

	static constexpr const char* kDefaultPath = "Asset/Data/ObjeData/Item/TreasureChestTable/slot.json";

private:
	// 旧形式（rank_weights + slots）の読み込み
	void LoadOldFormat(const nlohmann::json& j);

	std::unordered_map<std::string, LootTable> m_tables;

	std::string m_path = kDefaultPath;
};
