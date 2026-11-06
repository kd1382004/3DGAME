#pragma once

enum PotionsType
{
	//即座に回復
	PotionsType_InstantHeal = 0,

	//攻撃力Baseがアップ
	PotionsType_AttackBaseUP,

	//攻撃力が一時的にアップ
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