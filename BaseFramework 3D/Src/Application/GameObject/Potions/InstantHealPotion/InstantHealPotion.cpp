#include "InstantHealPotion.h"
#include"../../Character/Player/PlayerBase.h"

void InstantHealPotion::UseInstantHeal()
{
	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayerBase.lock();
	if (spPlayer)
	{
		int maxHP = spPlayer->GetMaxHP();
		int heal = maxHP * 0.1;
		spPlayer->HPHeal(heal);
	}
}
