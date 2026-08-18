#pragma once
class PlayerBase;

class AttackBaseUPPotion
{
public:
	AttackBaseUPPotion() {};
	~AttackBaseUPPotion() {};

	//攻撃力増加(永久的)
	void UseAttackBaseUPPotion();

	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }

private:

	std::weak_ptr<PlayerBase>m_wpPlayerBase;

};
