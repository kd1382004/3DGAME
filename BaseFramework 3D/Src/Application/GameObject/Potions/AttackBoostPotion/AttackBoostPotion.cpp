#include "AttackBoostPotion.h"
#include"../../Character/Player/PlayerBase.h"
#include"../../Character/Player/PlayerBuffManager/PlayerBuffManager.h"

void AttackBoostPotion::UseAttackBoostPotion()
{
	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayerBase.lock();
	if (spPlayer)
	{
		int boost = 10;

		std::shared_ptr<PlayerBuffManager>spPlayerBuffManager = spPlayer->GetPlayerBuffManager();
		if (spPlayerBuffManager)
		{
			StatusBuff buff;
			buff.m_buffAttck += 10;
			buff.m_maxSeconds = 100;
			buff.m_nowSeconds = buff.m_maxSeconds;
			spPlayerBuffManager->AddStatusBuff(buff);
		}
	}
}
