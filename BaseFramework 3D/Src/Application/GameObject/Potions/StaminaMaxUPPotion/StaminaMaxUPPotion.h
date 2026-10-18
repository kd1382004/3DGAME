#pragma once
class PlayerBase;

class StaminaMaxUPPotion
{
public:
	StaminaMaxUPPotion() {};
	~StaminaMaxUPPotion() {};

	//最大スタミナ増加(永久的)
	void UseStaminaMaxUPPotion();

	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }

private:

	std::weak_ptr<PlayerBase>m_wpPlayerBase;

};
