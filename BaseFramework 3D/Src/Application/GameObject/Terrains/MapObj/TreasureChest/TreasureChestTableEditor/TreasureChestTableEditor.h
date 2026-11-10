#pragma once
// 宝箱のドロップテーブル（slot.json）を編集する ImGui ウィンドウ
//  ・テーブルの追加 / 複製 / 削除 / 名前変更
//  ・テーブルごとに出る個数（最小〜最大）
//  ・アイテムの追加 / 削除と、重み（出やすさ）の変更。出現率(%)を横に表示する
// 宝箱は「slot + 宝箱のLv」という名前のテーブルを使う（例: Lv2 → slot2）
class TreasureChestTableEditor
{
public:
	explicit TreasureChestTableEditor(const std::string& _filePath = "Asset/Data/ObjeData/Item/TreasureChestTable/slot.json");

	// 毎フレーム ImGui のフレーム内で呼ぶ
	void Draw(bool* _open = nullptr);

	bool Load();
	bool Save();

private:
	// アイテムの種類（LootTableManager の ItemType と同じ番号）
	enum ItemTypeID
	{
		ItemType_Potion = 0,
		ItemType_Material = 1,
		ItemTypeSiz
	};

	struct MasterItem
	{
		int id = 0;
		std::string name;
	};

	// アイテムの名前一覧（PotionsName.json / WeaponsStrengtheningItemName.json）を読む
	void LoadMasters();
	static std::vector<MasterItem> LoadMaster(const std::string& _filePath);
	const std::vector<MasterItem>& GetMaster(int _type) const;
	std::string FindItemName(int _type, int _id) const;

	// テーブルを選ぶ（名前変更用の入力欄も合わせる）
	void SelectTable(const std::string& _tableName);

	// 左側：テーブル一覧と追加・複製・削除
	void DrawTableList();
	// 右側：選択中のテーブル
	void DrawTable(const std::string& _tableName, nlohmann::json& _table);
	// アイテムの行
	bool DrawItemRow(nlohmann::json& _item, int _index, float _totalWeight, float _avgDropNum);

	// 問題があればメッセージを返す（空なら保存できる）
	std::vector<std::string> Validate(const std::string& _tableName, const nlohmann::json& _table) const;
	std::vector<std::string> ValidateAll() const;

	// 使われていない "slotN" の名前を作る
	std::string MakeNewTableName() const;
	nlohmann::json MakeDefaultTable() const;

	nlohmann::json& Tables() { return m_data["tables"]; }

private:
	std::string    m_filePath;
	nlohmann::json m_data;
	bool           m_dirty = false; // 未保存の変更があるか
	std::string    m_message;

	std::string m_selectTable;      // 選択中のテーブル名
	char        m_renameBuf[64] = {};

	std::vector<MasterItem> m_potions;
	std::vector<MasterItem> m_materials;

	const std::string m_potionMasterPath = "Asset/Data/ObjeData/Item/Potions/PotionsName.json";
	const std::string m_materialMasterPath = "Asset/Data/ObjeData/Item/WeaponsStrengtheningItem/WeaponsStrengtheningItemName.json";
};
