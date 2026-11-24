//---- 自分のヘッダー ----
#include "InstantStaminaHealPotion.h"				//スタミナ回復ポーション

//---- プレイヤー ----
#include"../../Character/Player/PlayerBase.h"		//プレイヤー


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
