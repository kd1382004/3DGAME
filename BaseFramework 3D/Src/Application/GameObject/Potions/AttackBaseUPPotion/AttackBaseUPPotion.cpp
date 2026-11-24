//---- 自分のヘッダー ----
#include "AttackBaseUPPotion.h"						//基礎攻撃力アップポーション

//---- プレイヤー ----
#include"../../Character/Player/PlayerBase.h"		//プレイヤー


void AttackBaseUPPotion::UseAttackBaseUPPotion()
{
	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayerBase.lock();
	if (spPlayer)
	{
		int boost = 10;

		spPlayer->BoostAttackBase(boost);
	}
}
