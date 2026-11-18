#pragma once
#include"PotionsType.h"

class PlayerBase;

//ポーションを使う指示を出すクラス
class PotionUseController
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	PotionUseController() {};

	//デストラクタ
	~PotionUseController() {};

	//---- 設定 ----

	//ポーションを使うプレイヤーをセット
	// _spPlayerBase ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }

	//---- その他の処理 ----

	//ポーションを使う(番号で効果を分ける)
	// _potionsID ... 使うポーションの番号
	void PotionUse(int _potionsID);

private:

	//---- メンバ変数 ----

	//ポーションを使うプレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayerBase;

	//---- その他の処理 ----

	//HP関係
	void UseInstantHeal();

	//攻撃力関係
	void UseBaseAttackUPPotion();

	//攻撃力を一時的に上げるポーションを使う
	void UseAttackBoostPotion();

	//スタミナ関係
	void UseInstantStaminaHeal();

	//スタミナ回復速度の基礎を上げるポーションを使う
	void UseStaminaRegenBaseUPPotion();

	//スタミナ回復速度を一時的に上げるポーションを使う
	void UseStaminaRegenBoostPotion();

	//最大スタミナを上げるポーションを使う
	void UseStaminaMaxUPPotion();

	//スタミナ系ポーションを使ったときのエフェクト
	void PlayStaminaEffect();

	//ポーションを飲んだときの音（_path：鳴らす音のファイル）
	void PlayPotionSE(const std::string& _path);
};
