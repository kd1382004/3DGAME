#pragma once
#include"PotionsType.h"

class PlayerBase;

//ポーションを使う指示を出すクラス
class PotionUseController
{
public:
	PotionUseController() {};
	~PotionUseController() {};

	void PotionUse(int _potionsID);

	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }

private:

	std::weak_ptr<PlayerBase>m_wpPlayerBase;

	/////////////////
	//HP関係
	void UseInstantHeal();

	/////////////////
	//攻撃力関係
	void UseBaseAttackUPPotion();

	void UseAttackBoostPotion();
};
