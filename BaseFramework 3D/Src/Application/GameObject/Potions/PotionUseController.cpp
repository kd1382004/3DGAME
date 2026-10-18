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
}

void PotionUseController::UseBaseAttackUPPotion()
{
	std::shared_ptr<AttackBaseUPPotion>spAttackBaseUPtPotion = std::make_shared<AttackBaseUPPotion>();
	spAttackBaseUPtPotion->SetPlayer(m_wpPlayerBase.lock());
	spAttackBaseUPtPotion->UseAttackBaseUPPotion();
}

void PotionUseController::UseAttackBoostPotion()
{
	std::shared_ptr<AttackBoostPotion>spAttackBoostPotion = std::make_shared<AttackBoostPotion>();
	spAttackBoostPotion->SetPlayer(m_wpPlayerBase.lock());
	spAttackBoostPotion->UseAttackBoostPotion(PotionsType_AttackBoostPotion);
}

void PotionUseController::UseInstantStaminaHeal()
{
	std::shared_ptr<InstantStaminaHealPotion>spPotion = std::make_shared<InstantStaminaHealPotion>();
	spPotion->SetPlayer(m_wpPlayerBase.lock());
	spPotion->UseInstantStaminaHeal();
}

void PotionUseController::UseStaminaRegenBaseUPPotion()
{
	std::shared_ptr<StaminaRegenBaseUPPotion>spPotion = std::make_shared<StaminaRegenBaseUPPotion>();
	spPotion->SetPlayer(m_wpPlayerBase.lock());
	spPotion->UseStaminaRegenBaseUPPotion();
}

void PotionUseController::UseStaminaRegenBoostPotion()
{
	std::shared_ptr<StaminaRegenBoostPotion>spPotion = std::make_shared<StaminaRegenBoostPotion>();
	spPotion->SetPlayer(m_wpPlayerBase.lock());
	spPotion->UseStaminaRegenBoostPotion(PotionsType_StaminaRegenBoost);
}

void PotionUseController::UseStaminaMaxUPPotion()
{
	std::shared_ptr<StaminaMaxUPPotion>spPotion = std::make_shared<StaminaMaxUPPotion>();
	spPotion->SetPlayer(m_wpPlayerBase.lock());
	spPotion->UseStaminaMaxUPPotion();
}
