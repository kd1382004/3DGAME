//---- 自分のヘッダー ----
#include "StaminaRegenBaseUPPotion.h"				//基礎スタミナ回復量アップポーション

//---- プレイヤー ----
#include"../../Character/Player/PlayerBase.h"		//プレイヤー


void StaminaRegenBaseUPPotion::UseStaminaRegenBaseUPPotion()
{
	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayerBase.lock();
	if (spPlayer)
	{
		float boost = 1.0f;
		spPlayer->BoostStaminaRegenBase(boost);
	}
}
