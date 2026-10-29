#include "PlayerBuffManager.h"
#include"../PlayerBase.h"
#include"../../../Potions/PotionsType.h"
void PlayerBuffManager::Update()
{
	float deltaTime = DeltaTime::Instance().GetGameDeltaTime();

	auto it = m_statusBuffList.begin();

	while (it != m_statusBuffList.end())
	{
		it->m_nowSeconds -= deltaTime;

		if (it->m_nowSeconds <= 0)
		{
			BuffFinish(*it);
			it = m_statusBuffList.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void PlayerBuffManager::BuffFinish(const StatusBuff& _statusBuff)
{
	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayer.lock();
	if (spPlayer)
	{
		spPlayer->AddMaxHP(-_statusBuff.m_buffHP);
		spPlayer->BoostAddAttack(-_statusBuff.m_buffAttck);
		spPlayer->BoostAddDefense(-_statusBuff.m_buffDefense);
		spPlayer->AddStaminaRegenBuff(-_statusBuff.m_buffStaminaRegen);


		switch (_statusBuff.m_ID)
		{
		case PotionsType_InstantHeal:
			break;
		case PotionsType_AttackBaseUP:
			break;
		case PotionsType_AttackBoostPotion:
			spPlayer->StopAttackBoostEffect();
			break;
		case PotionsType_InstantStaminaHeal:
			break;
		case PotionsType_StaminaRegenBaseUP:
			break;
		case PotionsType_StaminaRegenBoost:
			break;
		case PotionsType_StaminaMaxUP:
			break;
		case PotionsTypeMax:
			break;
		default:
			break;
		}
	}
}

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
		spPlayer->BoostAddAttack(_statusBuff.m_buffAttck);
		spPlayer->BoostAddDefense(_statusBuff.m_buffDefense);
		spPlayer->AddStaminaRegenBuff(_statusBuff.m_buffStaminaRegen);
		m_statusBuffList.push_back(_statusBuff);

		switch (_statusBuff.m_ID)
		{
		case PotionsType_InstantHeal:
			break;
		case PotionsType_AttackBaseUP:
			break;
		case PotionsType_AttackBoostPotion:
			spPlayer->PlayAttackBoostEffect();
			break;
		case PotionsType_InstantStaminaHeal:
			break;
		case PotionsType_StaminaRegenBaseUP:
			break;
		case PotionsType_StaminaRegenBoost:
			break;
		case PotionsType_StaminaMaxUP:
			break;
		case PotionsTypeMax:
			break;
		default:
			break;
		}
	}

}
