#include "PotionUseController.h"

//HP関係
#include"InstantHealPotion/InstantHealPotion.h"

//攻撃力関係
#include"AttackBaseUPPotion/AttackBaseUPPotion.h"
#include"AttackBoostPotion/AttackBoostPotion.h"

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
	spAttackBoostPotion->UseAttackBoostPotion();
}
