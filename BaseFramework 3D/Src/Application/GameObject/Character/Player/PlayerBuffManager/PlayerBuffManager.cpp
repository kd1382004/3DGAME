//---- 自分のヘッダー ----
#include "PlayerBuffManager.h"					//プレイヤーのバフ管理

//---- プレイヤー ----
#include"../PlayerBase.h"						//プレイヤー

//---- ポーション ----
#include"../../../Potions/PotionsType.h"		//ポーションの種類

//---- 便利機能 ----
#include"../../../../Info/SE/SEPlayer.h"		//効果音

//=============================================
// 基本の処理
//=============================================

void PlayerBuffManager::Update()
{
	float deltaTime = DeltaTime::Instance().GetGameDeltaTime();

	bool buffEnded = false;

	auto it = m_statusBuffList.begin();

	while (it != m_statusBuffList.end())
	{
		it->m_nowSeconds -= deltaTime;

		if (it->m_nowSeconds <= 0)
		{
			BuffFinish(*it);
			it = m_statusBuffList.erase(it);

			//時間切れでバフが切れた音(同じフレームで何個切れても1回だけ)
			buffEnded = true;
		}
		else
		{
			it++;
		}
	}

	if (buffEnded)
	{
		SEPlayer::Play2D("Asset/Sound/SE/Player/BuffEnd.wav", 0.7f);
	}
}

//=============================================
// 設定
//=============================================

void PlayerBuffManager::AddStatusBuff(StatusBuff _statusBuff)
{
	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayer.lock();
	if (spPlayer)
	{
		auto it = m_statusBuffList.begin();

		while (it != m_statusBuffList.end())
		{

			if (it->m_ID == _statusBuff.m_ID)
			{
				BuffFinish(*it);
				it = m_statusBuffList.erase(it);
			}
			else
			{
				++it;
			}
		}

		spPlayer->AddMaxHP(_statusBuff.m_buffHP);
		spPlayer->BoostAddAttack(_statusBuff.m_buffAttack);
		spPlayer->BoostAddDefense(_statusBuff.m_buffDefense);
		spPlayer->AddStaminaRegenBuff(_statusBuff.m_buffStaminaRegen);
		m_statusBuffList.push_back(_statusBuff);

		// 攻撃力アップ中は体にエフェクトを出す
		if (_statusBuff.m_ID == PotionsType_AttackBoostPotion)
		{
			spPlayer->PlayAttackBoostEffect();
		}
	}

}

//=============================================
// その他の処理
//=============================================

void PlayerBuffManager::BuffFinish(const StatusBuff& _statusBuff)
{
	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayer.lock();
	if (spPlayer)
	{
		spPlayer->AddMaxHP(-_statusBuff.m_buffHP);
		spPlayer->BoostAddAttack(-_statusBuff.m_buffAttack);
		spPlayer->BoostAddDefense(-_statusBuff.m_buffDefense);
		spPlayer->AddStaminaRegenBuff(-_statusBuff.m_buffStaminaRegen);


		// 攻撃力アップが切れたらエフェクトを消す
		if (_statusBuff.m_ID == PotionsType_AttackBoostPotion)
		{
			spPlayer->StopAttackBoostEffect();
		}
	}
}

void PlayerBuffManager::ClearAllBuffs()
{
	auto it = m_statusBuffList.begin();

	while (it != m_statusBuffList.end())
	{
		BuffFinish(*it);
		it = m_statusBuffList.erase(it);
	}
}
