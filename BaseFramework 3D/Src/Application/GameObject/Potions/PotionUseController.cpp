//---- 自分のヘッダー ----
#include "PotionUseController.h"									//ポーションの使用処理

//---- プレイヤー ----
#include"../Character/Player/PlayerBase.h"							//プレイヤー

//---- ポーション ----
#include"InstantHealPotion/InstantHealPotion.h"						//体力回復ポーション
#include"AttackBaseUPPotion/AttackBaseUPPotion.h"					//基礎攻撃力アップポーション
#include"AttackBoostPotion/AttackBoostPotion.h"						//攻撃力一時アップポーション
#include"InstantStaminaHealPotion/InstantStaminaHealPotion.h"		//スタミナ回復ポーション
#include"StaminaRegenBaseUPPotion/StaminaRegenBaseUPPotion.h"		//基礎スタミナ回復量アップポーション
#include"StaminaRegenBoostPotion/StaminaRegenBoostPotion.h"			//スタミナ回復量一時アップポーション
#include"StaminaMaxUPPotion/StaminaMaxUPPotion.h"					//最大スタミナアップポーション

//---- 便利機能 ----
#include"../../Info/VOL/VOLInfo.h"									//音量設定

void PotionUseController::PotionUse(int _potionsID)
{
	PotionsType potionsType = (PotionsType)_potionsID;

	switch (potionsType)
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
	std::shared_ptr<KdSoundInstance> spSE = KdAudioManager::Instance().Play(_path, false);
	if (spSE)
	{
		spSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
	}
}
