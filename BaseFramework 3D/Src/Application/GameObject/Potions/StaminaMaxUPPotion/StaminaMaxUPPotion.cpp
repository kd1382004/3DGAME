#include "StaminaMaxUPPotion.h"
#include"../../Character/Player/PlayerBase.h"

void StaminaMaxUPPotion::UseStaminaMaxUPPotion()
{
	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayerBase.lock();
	if (spPlayer)
	{
		float boost = 20.0f;
		spPlayer->AddMaxStamina(boost);
	}
}
