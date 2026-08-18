#include "AttackBaseUPPotion.h"
#include"../../Character/Player/PlayerBase.h"

void AttackBaseUPPotion::UseAttackBaseUPPotion()
{
	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayerBase.lock();
	if (spPlayer)
	{
		int boost = 10;

		spPlayer->BoostAttackBase(boost);
	}
}
