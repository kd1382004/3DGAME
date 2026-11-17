#pragma once
// 宝箱のドロップテーブル（slot.json）を編集する ImGui ウィンドウ
//  ・テーブルの追加 / 複製 / 削除 / 名前変更
//  ・テーブルごとに出る個数（最小〜最大）
//  ・アイテムの追加 / 削除と、重み（出やすさ）の変更。出現率(%)を横に表示する
// 宝箱は「slot + 宝箱のLv」という名前のテーブルを使う（例: Lv2 → slot2）
class TreasureChestTableEditor
{
public:
	//コンストラクタ(ファイルを読み込む)
	// _filePath ... 編集するjsonのパス
	explicit TreasureChestTableEditor(const std::string& _filePath = "Asset/Data/ObjeData/Item/TreasureChestTable/slot.json");

	// 毎フレーム ImGui のフレーム内で呼ぶ
	// _open ... ウィンドウを開いているか(×で閉じるとfalseになる)
	void Draw(bool* _open = nullptr);

	//ファイルを読み込む
	//戻り値 ... 読めたらtrue
	bool Load();
	//ファイルに保存する
	//戻り値 ... 保存できたらtrue
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
		//アイテムの番号
		int m_id = 0;
		//アイテムの名前
		std::string m_name;
	};

	// アイテムの名前一覧（PotionsName.json / WeaponsStrengtheningItemName.json）を読む
	void LoadMasters();
	//名前一覧のjsonを1つ読む
	// _filePath ... jsonのパス
	//戻り値 ... アイテムの一覧
	static std::vector<MasterItem> LoadMaster(const std::string& _filePath);
	//種類ごとの名前一覧を返す
	// _type ... アイテムの種類
	//戻り値 ... アイテムの一覧
	const std::vector<MasterItem>& GetMaster(int _type) const;
	//アイテムの名前を探す
	// _type ... アイテムの種類
	// _id ... アイテムの番号
	//戻り値 ... アイテムの名前
	std::string FindItemName(int _type, int _id) const;

	// テーブルを選ぶ（名前変更用の入力欄も合わせる）
	// _tableName ... 選ぶテーブルの名前
	void SelectTable(const std::string& _tableName);

	// 左側：テーブル一覧と追加・複製・削除
	void DrawTableList();
	// 右側：選択中のテーブル
	// _tableName ... テーブルの名前
	// _table ... テーブルのjson
	void DrawTable(const std::string& _tableName, nlohmann::json& _table);
	// アイテムの行
	// _item ... アイテムのjson
	// _index ... 行の番号
	// _totalWeight ... 重みの合計
	// _avgDropNum ... 平均で出る個数
	bool DrawItemRow(nlohmann::json& _item, int _index, float _totalWeight, float _avgDropNum);

	// 問題があればメッセージを返す（空なら保存できる）
	// _tableName ... テーブルの名前
	// _table ... テーブルのjson
	std::vector<std::string> Validate(const std::string& _tableName, const nlohmann::json& _table) const;
	//全部のテーブルを検査する
	//戻り値 ... 問題のメッセージの一覧
	std::vector<std::string> ValidateAll() const;

	// 使われていない "slotN" の名前を作る
	std::string MakeNewTableName() const;
	//新しいテーブルの初期の中身を作る
	//戻り値 ... テーブルのjson
	nlohmann::json MakeDefaultTable() const;

	//テーブルの一覧を返す
	//戻り値 ... テーブルの一覧のjson
	nlohmann::json& Tables() { return m_data["tables"]; }

private:
	//編集するファイルのパス
	std::string    m_filePath;
	//読み込んだjson
	nlohmann::json m_data;
	bool           m_dirty = false; // 未保存の変更があるか
	std::string    m_message;

	std::string m_selectTable;      // 選択中のテーブル名
	char        m_renameBuf[64] = {};

	//ポーションの名前一覧
	std::vector<MasterItem> m_potions;
	//武器強化素材の名前一覧
	std::vector<MasterItem> m_materials;

	//ポーションの名前一覧のパス
	const std::string m_potionMasterPath = "Asset/Data/ObjeData/Item/Potions/PotionsName.json";
	//武器強化素材の名前一覧のパス
	const std::string m_materialMasterPath = "Asset/Data/ObjeData/Item/WeaponsStrengtheningItem/WeaponsStrengtheningItemName.json";
};
