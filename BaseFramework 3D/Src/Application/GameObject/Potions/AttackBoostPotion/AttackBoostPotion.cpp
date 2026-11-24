//---- 自分のヘッダー ----
#include "AttackBoostPotion.h"												//攻撃力一時アップポーション

//---- プレイヤー ----
#include"../../Character/Player/PlayerBase.h"								//プレイヤー
#include"../../Character/Player/PlayerBuffManager/PlayerBuffManager.h"		//プレイヤーのバフ管理


void AttackBoostPotion::UseAttackBoostPotion(int _ID)
{
	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayerBase.lock();
	if (spPlayer)
	{
		int boost = spPlayer->GetAttackBase() * 0.1;

		std::shared_ptr<PlayerBuffManager>spPlayerBuffManager = spPlayer->GetPlayerBuffManager();
		if (spPlayerBuffManager)
		{
			StatusBuff buff;
			buff.m_buffAttack += boost;
			buff.m_maxSeconds = 100;
			buff.m_nowSeconds = buff.m_maxSeconds;
			buff.m_ID = _ID;
			spPlayerBuffManager->AddStatusBuff(buff);
		}
	}
}
