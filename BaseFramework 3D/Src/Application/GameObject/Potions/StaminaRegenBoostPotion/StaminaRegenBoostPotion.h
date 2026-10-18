#pragma once
class PlayerBase;

class StaminaRegenBoostPotion
{
public:
	StaminaRegenBoostPotion() {};
	~StaminaRegenBoostPotion() {};

	//スタミナ回復速度一時増加(時間制限バフ)
	//ID...ポーションID
	void UseStaminaRegenBoostPotion(int ID);

	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }

private:

	std::weak_ptr<PlayerBase>m_wpPlayerBase;

};
