#include "StaminaRegenBoostPotion.h"
#include"../../Character/Player/PlayerBase.h"
#include"../../Character/Player/PlayerBuffManager/PlayerBuffManager.h"

void StaminaRegenBoostPotion::UseStaminaRegenBoostPotion(int ID)
{
	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayerBase.lock();
	if (spPlayer)
	{
		std::shared_ptr<PlayerBuffManager>spPlayerBuffManager = spPlayer->GetPlayerBuffManager();
		if (spPlayerBuffManager)
		{
			StatusBuff buff;
			buff.m_buffStaminaRegen = 3.0f;
			buff.m_maxSeconds = 60.0f;
			buff.m_nowSeconds = buff.m_maxSeconds;
			buff.m_ID = ID;
			spPlayerBuffManager->AddStatusBuff(buff);
		}
	}
}
