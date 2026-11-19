#pragma once

class ItemGetUIController;

//持っているアイテム1種類分の情報
struct Inventory
{
	//アイテムの番号
	int m_ID = 0;
	//持っている数
	int m_num = 0;
	//アイテムの名前
	std::string m_name;
};

//プレイヤーが持ってるアイテムの数を管理するクラス
class PlayerInventory
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	PlayerInventory() {};

	//デストラクタ
	~PlayerInventory() {};

	//---- 基本の処理 ----

	//jsonからアイテムの一覧を読み込んで、所持数0で初期化
	void Init();

	//---- 取得 ----

	//ポーションのインベントリを返す
	//戻り値 ... ポーションのインベントリ(コピー)
	std::vector<Inventory> GetPotionsInventory() { return m_potionsInventory; }

	//武器強化素材のインベントリを返す
	//戻り値 ... 武器強化素材のインベントリ(コピー)
	std::vector<Inventory> GetWeaponsStrengtheningInventory() { return m_weaponsStrengtheningInventory; }

	//ポーションの数を取得
	// _potionsType ... 数を知りたいポーションの番号
	int GetPotionsInventoryNum(int _potionsType);

	//武器強化素材の数を取得
	// _weaponsStrengtheningType ... 数を知りたい武器強化素材の番号
	int GetWeaponsStrengtheningInventoryNum(int _weaponsStrengtheningType);

	//ポーションのインベントリリストを取得
	const std::vector<Inventory>& GetPotionsInventoryList() { return m_potionsInventory; }

	//武器強化素材のインベントリリストを取得
	const std::vector<Inventory>& GetWeaponsStrengtheningInventoryList() { return m_weaponsStrengtheningInventory; }

	//所持金を取得
	long GetMoney() const { return m_money; }

	//---- 設定 ----

	//ポーションがインベントリで増えたとき
	// _potionsType ... 増やすポーションの番号
	void AddPotionsInventory(int _potionsType);

	//武器強化素材がインベントリで増えたとき
	// _weaponsStrengtheningType ... 増やす武器強化素材の番号
	void AddWeaponsStrengtheningInventory(int _weaponsStrengtheningType);

	//アイテムを手に入れたときのUIをセット
	// _spItemGetUIController ... アイテム取得UIの管理
	void SetItemGetUIController(std::shared_ptr<ItemGetUIController>_spItemGetUIController) { m_wpItemGetUIController = _spItemGetUIController; }

	//ポーションのインベントリリストを設定
	// _potions ... 設定するポーションのリスト
	void SetPotionsInventoryList(const std::vector<Inventory>& _potions);

	//武器強化素材のインベントリリストを設定
	// _weapons ... 設定する武器強化素材のリスト
	void SetWeaponsStrengtheningInventoryList(const std::vector<Inventory>& _weapons);

	//所持金を設定(セーブデータを読み込んだとき)
	// _money ... 設定する所持金(0未満は0にする)
	void SetMoney(long _money) { m_money = std::max(0L, _money); }

	//お金が増えたとき(拾った・もらった)
	// _money ... 増やす金額(0以下なら何もしない)
	void AddMoney(long _money) { if (_money > 0) { m_money += _money; } }

	//---- その他の処理 ----

	//ポーションが使われたとき
	// _potionsType ... 使うポーションの番号
	void UsePotionsInventory(int _potionsType);

	//武器強化素材が使われたとき
	// _weaponsStrengtheningType ... 使う武器強化素材の番号
	// _num ... 使う数
	void UseWeaponsStrengtheningInventory(int _weaponsStrengtheningType, int _num);

	//お金を使うとき。足りなければ使わずにfalseを返す
	// _money ... 使う金額
	bool UseMoney(long _money)
	{
		if (_money < 0 || m_money < _money) { return false; }
		m_money -= _money;
		return true;
	}

private:

	//---- メンバ変数 ----

	//各番号のポーションがどれだけあるか
	std::vector<Inventory>m_potionsInventory;

	//武器強化素材がどれだけあるか
	std::vector<Inventory>m_weaponsStrengtheningInventory;

	//アイテムを手に入れたときのUI
	std::weak_ptr<ItemGetUIController>m_wpItemGetUIController;

	//お金
	long m_money = 0;
};
