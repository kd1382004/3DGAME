#include "InstantHealPotion.h"
#include"../../Character/Player/PlayerBase.h"

void InstantHealPotion::UseInstantHeal()
{
	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayerBase.lock();
	if (spPlayer)
	{
		int MaxHP = spPlayer->GetMaxHP();
		int heal = MaxHP * 0.1;
		spPlayer->HPHeal(heal);
	}
}
