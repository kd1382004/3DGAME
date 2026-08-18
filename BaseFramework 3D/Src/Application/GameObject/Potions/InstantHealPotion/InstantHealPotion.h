#pragma once
class PlayerBase;

class InstantHealPotion
{
public:
	InstantHealPotion() {};
	~InstantHealPotion() {};

	//HP即回復
	void UseInstantHeal();

	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }

private:

	std::weak_ptr<PlayerBase>m_wpPlayerBase;

};
