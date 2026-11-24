//---- 自分のヘッダー ----
#include "StaminaMaxUPPotion.h"						//最大スタミナアップポーション

//---- プレイヤー ----
#include"../../Character/Player/PlayerBase.h"		//プレイヤー


void StaminaMaxUPPotion::UseStaminaMaxUPPotion()
{
	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayerBase.lock();
	if (spPlayer)
	{
		float boost = 20.0f;
		spPlayer->AddMaxStamina(boost);
	}
}
