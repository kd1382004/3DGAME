#include "PotionUseController.h"



//HP関係
#include"InstantHealPotion/InstantHealPotion.h"

//攻撃力関係
#include"AttackBaseUPPotion/AttackBaseUPPotion.h"
#include"AttackBoostPotion/AttackBoostPotion.h"

//スタミナ関係
#include"InstantStaminaHealPotion/InstantStaminaHealPotion.h"
#include"StaminaRegenBaseUPPotion/StaminaRegenBaseUPPotion.h"
#include"StaminaRegenBoostPotion/StaminaRegenBoostPotion.h"
#include"StaminaMaxUPPotion/StaminaMaxUPPotion.h"

#include"../Character/Player/PlayerBase.h"
#include"../../Info/VOL/VOLInfo.h"

void PotionUseController::PotionUse(int _potionsID)
{
	PotionsType _potionsType = (PotionsType)_potionsID;

	switch (_potionsType)
	{
	case PotionsType_InstantHeal:
		UseInstantHeal();
		break;

	case PotionsType_AttackBaseUP:
		UseBaseAttackUPPotion();
		break;

	case PotionsType_AttackBoostPotion:
		UseAttackBoostPotion();
		break;

	case PotionsType_InstantStaminaHeal:
		UseInstantStaminaHeal();
		break;

	case PotionsType_StaminaRegenBaseUP:
		UseStaminaRegenBaseUPPotion();
		break;

	case PotionsType_StaminaRegenBoost:
		UseStaminaRegenBoostPotion();
		break;

	case PotionsType_StaminaMaxUP:
		UseStaminaMaxUPPotion();
		break;

	default:
		break;
	}
}

void PotionUseController::UseInstantHeal()
{
	std::shared_ptr<InstantHealPotion>spInstantHealPotion = std::make_shared<InstantHealPotion>();
	spInstantHealPotion->SetPlayer(m_wpPlayerBase.lock());
	spInstantHealPotion->UseInstantHeal();

	PlayPotionSE("Asset/Sound/SE/Potion/Heal.wav");
}

void PotionUseController::UseBaseAttackUPPotion()
{
	std::shared_ptr<AttackBaseUPPotion>spAttackBaseUPtPotion = std::make_shared<AttackBaseUPPotion>();
	spAttackBaseUPtPotion->SetPlayer(m_wpPlayerBase.lock());
	spAttackBaseUPtPotion->UseAttackBaseUPPotion();

	PlayPotionSE("Asset/Sound/SE/Potion/Power.wav");
}

void PotionUseController::UseAttackBoostPotion()
{
	std::shared_ptr<AttackBoostPotion>spAttackBoostPotion = std::make_shared<AttackBoostPotion>();
	spAttackBoostPotion->SetPlayer(m_wpPlayerBase.lock());
	spAttackBoostPotion->UseAttackBoostPotion(PotionsType_AttackBoostPotion);

	PlayPotionSE("Asset/Sound/SE/Potion/Power.wav");
}

void PotionUseController::UseInstantStaminaHeal()
{
	std::shared_ptr<InstantStaminaHealPotion>spPotion = std::make_shared<InstantStaminaHealPotion>();
	spPotion->SetPlayer(m_wpPlayerBase.lock());
	spPotion->UseInstantStaminaHeal();

	PlayStaminaEffect();
}

void PotionUseController::UseStaminaRegenBaseUPPotion()
{
	std::shared_ptr<StaminaRegenBaseUPPotion>spPotion = std::make_shared<StaminaRegenBaseUPPotion>();
	spPotion->SetPlayer(m_wpPlayerBase.lock());
	spPotion->UseStaminaRegenBaseUPPotion();

	PlayStaminaEffect();
}

void PotionUseController::UseStaminaRegenBoostPotion()
{
	std::shared_ptr<StaminaRegenBoostPotion>spPotion = std::make_shared<StaminaRegenBoostPotion>();
	spPotion->SetPlayer(m_wpPlayerBase.lock());
	spPotion->UseStaminaRegenBoostPotion(PotionsType_StaminaRegenBoost);

	PlayStaminaEffect();
}

void PotionUseController::UseStaminaMaxUPPotion()
{
	std::shared_ptr<StaminaMaxUPPotion>spPotion = std::make_shared<StaminaMaxUPPotion>();
	spPotion->SetPlayer(m_wpPlayerBase.lock());
	spPotion->UseStaminaMaxUPPotion();

	PlayStaminaEffect();
}

void PotionUseController::PlayStaminaEffect()
{
	std::shared_ptr<PlayerBase> spPlayer = m_wpPlayerBase.lock();
	if (!spPlayer) { return; }

	KdEffekseerManager::GetInstance().Play("Potion/Stamina/StaminaHeal.efkefc", spPlayer->GetPos(), 1.0f, 1.0f, false);

	// スタミナ系はここで音も鳴らす（4種類共通）
	PlayPotionSE("Asset/Sound/SE/Potion/Stamina.wav");
}

void PotionUseController::PlayPotionSE(const std::string& _path)
{
	std::shared_ptr<KdSoundInstance> se = KdAudioManager::Instance().Play(_path, false);
	if (se)
	{
		se->SetVolume(VOLInfo::Instance().GetMSSEVol());
	}
}
