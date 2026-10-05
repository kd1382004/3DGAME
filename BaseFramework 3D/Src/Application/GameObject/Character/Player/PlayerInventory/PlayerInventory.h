#pragma once

class ItemGetUIController;

struct Inventory
{
	int m_ID;
	int m_num;
	std::string m_name;
};

//プレイヤーが持ってるアイテムの数を管理するクラス
class PlayerInventory
{
public:
	PlayerInventory() {};
	~PlayerInventory() {};

	void Init();

	std::vector<Inventory> GetPotionsInventory() { return m_potionsInventory; }
	std::vector<Inventory> GetWeaponsStrengtheningInventory() { return m_weaponsStrengtheningInventory; }


	//ポーションがインベントリで増えたとき
	void AddPotionsInventory(int _PotionsType);

	//ポーションが使われたとき
	void UsePotionsInventory(int _PotionsType);

	//ポーションの数を取得
	int GetPotionsInventoryNum(int _PotionsType);

	//武器強化素材がインベントリで増えたとき
	void AddWeaponsStrengtheningInventory(int _WeaponsStrengtheningType);

	//武器強化素材が使われたとき
	void UseWeaponsStrengtheningInventory(int _WeaponsStrengtheningType,int _num);

	//武器強化素材の数を取得
	int GetWeaponsStrengtheningInventoryNum(int _WeaponsStrengtheningType);

	void SetItemGetUIController(std::shared_ptr<ItemGetUIController>_ItemGetUIController) { m_wpItemGetUIController = _ItemGetUIController; }
private:

	//各番号のポーションがどれだけあるか
	std::vector<Inventory>m_potionsInventory;


	//各番号の武器強化素材がどれだけあるか
	std::vector<Inventory>m_weaponsStrengtheningInventory;


	std::weak_ptr<ItemGetUIController>m_wpItemGetUIController;
};
