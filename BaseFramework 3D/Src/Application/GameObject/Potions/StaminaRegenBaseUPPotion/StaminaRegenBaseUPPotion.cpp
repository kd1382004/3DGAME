#include "StaminaRegenBaseUPPotion.h"
#include"../../Character/Player/PlayerBase.h"

void StaminaRegenBaseUPPotion::UseStaminaRegenBaseUPPotion()
{
	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayerBase.lock();
	if (spPlayer)
	{
		float boost = 1.0f;
		spPlayer->BoostStaminaRegenBase(boost);
	}
}
