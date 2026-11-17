#pragma once

enum ItemType
{
	Potion,	// 0 : ポーション（PotionsName.json の id）
	Weapon	// 1 : 武器強化素材（WeaponsStrengtheningItemName.json の id）
};


struct LootItem
{
	int m_type = -1;		// ItemType（-1 = 何も出ない）
	int m_id = 0;
	//アイテムの名前
	std::string m_name;
	float m_weight = 0.0f; // 出やすさ（テーブル内の合計に対する割合が出現率になる）
};

struct LootTable
{
	int m_dropMin = 1;	// 1回開けたときに出る個数の最小
	int m_dropMax = 1;	// 1回開けたときに出る個数の最大
	std::vector<LootItem> m_items;
	//重みの合計
	float m_totalWeight = 0.0f;
};

// 宝箱のドロップテーブル（Asset/Data/ObjeData/Item/TreasureChestTable/slot.json）
class LootTableManager
{
public:
	//コンストラクタ
	LootTableManager() {
		Load(m_path);
	};
	//デストラクタ
	~LootTableManager() {};

	// ゲーム中で共有するインスタンス（エディタで保存したときにここを読み直す）
	static std::shared_ptr<LootTableManager> GetShared();

	//ドロップテーブルを読み込む
	// _path ... jsonのパス
	void Load(const std::string& _path);
	//前に読んだファイルを読み直す
	void Reload() { Load(m_path); }

	// テーブルから重みに従って1つ選ぶ。テーブルがなければ type = -1
	// _tableName ... テーブルの名前
	LootItem GetRandomLoot(const std::string& _tableName) const;

	// 1回開けたときに出る個数（dropMin〜dropMax）。テーブルがなければ 0
	// _tableName ... テーブルの名前
	int GetRandomDropNum(const std::string& _tableName) const;

	//テーブルがあるか
	// _tableName ... テーブルの名前
	//戻り値 ... あるならtrue
	bool HasTable(const std::string& _tableName) const { return m_tables.count(_tableName) > 0; }

	//ドロップテーブルのファイルのパス
	static constexpr const char* kDefaultPath = "Asset/Data/ObjeData/Item/TreasureChestTable/slot.json";

private:
	// 旧形式（rank_weights + slots）の読み込み
	// _j ... 読み込んだjson
	void LoadOldFormat(const nlohmann::json& _j);

	//テーブルの名前ごとのドロップテーブル
	std::unordered_map<std::string, LootTable> m_tables;

	//読み込んだファイルのパス
	std::string m_path = kDefaultPath;
};
