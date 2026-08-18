#pragma once
class PlayerBase;

class AttackBoostPotion
{
public:
	AttackBoostPotion() {};
	~AttackBoostPotion() {};

	//攻撃力増加(永久的)
	//ID...ポーションID
	void UseAttackBoostPotion(int ID);

	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }

private:

	std::weak_ptr<PlayerBase>m_wpPlayerBase;

};
