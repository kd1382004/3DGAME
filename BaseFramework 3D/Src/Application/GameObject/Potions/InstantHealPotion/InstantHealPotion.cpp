//---- 自分のヘッダー ----
#include "InstantHealPotion.h"						//体力回復ポーション

//---- プレイヤー ----
#include"../../Character/Player/PlayerBase.h"		//プレイヤー


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
