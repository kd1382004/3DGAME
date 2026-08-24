#pragma once


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


	//ポーションがインベントリで増えたとき
	void AddPotionsInventory(int _PotionsType);

	//ポーションが使われたとき
	void UsePotionsInventory(int _PotionsType);

	int GetPotionsInventoryNum(int _PotionsType);
private:

	//各番号のポーションがどれだけあるか
	std::vector<Inventory>m_potionsInventory;

};
