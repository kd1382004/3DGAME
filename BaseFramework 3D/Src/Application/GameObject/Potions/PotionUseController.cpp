//---- 自分のヘッダー ----
#include "PotionUseController.h"									//ポーションの使用処理

//---- プレイヤー ----
#include"../Character/Player/PlayerBase.h"							//プレイヤー

//---- シーン ----
#include"../../Scene/GameScene/GameScene.h"							//ゲームシーン

//---- UI ----
#include"../UI/UIManager.h"											//UIの管理

//---- ポーション ----
#include"InstantHealPotion/InstantHealPotion.h"						//体力回復ポーション
#include"AttackBoostPotion/AttackBoostPotion.h"						//攻撃力一時アップポーション
#include"InstantStaminaHealPotion/InstantStaminaHealPotion.h"		//スタミナ回復ポーション
#include"StaminaRegenBoostPotion/StaminaRegenBoostPotion.h"			//スタミナ回復量一時アップポーション
#include"StaminaMaxUPPotion/StaminaMaxUPPotion.h"					//最大スタミナアップポーション

//---- 便利機能 ----
#include"../../Info/VOL/VOLInfo.h"									//音量設定

void PotionUseController::PotionUse(int _potionsID)
{
	//ポーションの種類 → 使ったときの処理 の表
	//効果のあるポーションを増やしたら、ここに1行足す(載っていない種類は何も起きない)
	struct UseFunc
	{
		PotionsType m_type;							// 種類
		void (PotionUseController::* m_func)();		// 使ったときに呼ぶ関数
	};

	static const UseFunc kUseFuncs[] =
	{
		{ PotionsType_InstantHeal,        &PotionUseController::UseInstantHeal             },
		{ PotionsType_AttackBoostPotion,  &PotionUseController::UseAttackBoostPotion       },
		{ PotionsType_InstantStaminaHeal, &PotionUseController::UseInstantStaminaHeal      },
		{ PotionsType_StaminaRegenBoost,  &PotionUseController::UseStaminaRegenBoostPotion },
		{ PotionsType_StaminaMaxUP,       &PotionUseController::UseStaminaMaxUPPotion      },
		{ PotionsType_MapScroll,          &PotionUseController::UseMapScroll               },
	};

	for (const UseFunc& use : kUseFuncs)
	{
		if (use.m_type == _potionsID)
		{
			(this->*use.m_func)();
			return;
		}
	}
}

void PotionUseController::UseInstantHeal()
{
	std::shared_ptr<InstantHealPotion>spInstantHealPotion = std::make_shared<InstantHealPotion>();
	spInstantHealPotion->SetPlayer(m_wpPlayerBase.lock());
	spInstantHealPotion->UseInstantHeal();

	PlayPotionSE("Asset/Sound/SE/Potion/Heal.wav");
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

void PotionUseController::UseMapScroll()
{
	std::shared_ptr<PlayerBase> spPlayer = m_wpPlayerBase.lock();
	if (!spPlayer) { return; }

	std::shared_ptr<GameScene> spGameScene = spPlayer->GetGameScene();
	if (!spGameScene) { return; }

	std::shared_ptr<UIManager> spUIManager = spGameScene->GetUIManager();
	if (!spUIManager) { return; }

	// 今いる階のミニマップを全部表示する(次の階ではまた最初から)
	spUIManager->SetMinimapRevealAll(true);

	PlayPotionSE("Asset/Sound/SE/UI/PageTurn.wav");
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
