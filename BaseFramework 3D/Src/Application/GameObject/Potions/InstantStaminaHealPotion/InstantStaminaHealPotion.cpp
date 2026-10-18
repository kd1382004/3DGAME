#include "InstantStaminaHealPotion.h"
#include"../../Character/Player/PlayerBase.h"

void InstantStaminaHealPotion::UseInstantStaminaHeal()
{
	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayerBase.lock();
	if (spPlayer)
	{
		float maxStamina = spPlayer->GetStaminaMax();
		float heal = maxStamina * 0.1f;
		spPlayer->StaminaHeal(heal);
	}
}
