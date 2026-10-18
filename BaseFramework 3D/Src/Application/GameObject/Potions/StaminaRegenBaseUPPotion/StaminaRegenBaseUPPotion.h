#pragma once
class PlayerBase;

class StaminaRegenBaseUPPotion
{
public:
	StaminaRegenBaseUPPotion() {};
	~StaminaRegenBaseUPPotion() {};

	//スタミナ回復速度増加(永久的)
	void UseStaminaRegenBaseUPPotion();

	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }

private:

	std::weak_ptr<PlayerBase>m_wpPlayerBase;

};
