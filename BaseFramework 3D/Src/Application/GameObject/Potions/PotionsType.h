#pragma once

enum PotionsType
{
	PotionsType_InstantHeal = 0,

	PotionsType_AttackBaseUP,

	PotionsType_AttackBoostPotion,

	// スタミナ即座に回復
	PotionsType_InstantStaminaHeal,

	// スタミナ回復量アップ（永続）
	PotionsType_StaminaRegenBaseUP,

	// スタミナ回復量アップ（時間制限）
	PotionsType_StaminaRegenBoost,

	// スタミナ量増加（最大スタミナ増加）
	PotionsType_StaminaMaxUP,

	//ポーション種類数
	PotionsTypeMax
};