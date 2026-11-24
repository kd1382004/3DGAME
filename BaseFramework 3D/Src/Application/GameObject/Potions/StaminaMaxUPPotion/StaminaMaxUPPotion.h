#pragma once
class PlayerBase;

//最大スタミナアップポーション
class StaminaMaxUPPotion
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	StaminaMaxUPPotion() {};

	//デストラクタ
	~StaminaMaxUPPotion() {};

	//---- 設定 ----

	//ポーションを使うプレイヤーをセット
	// _spPlayerBase ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }

	//---- その他の処理 ----

	//最大スタミナ増加(永久的)
	void UseStaminaMaxUPPotion();

private:

	//---- メンバ変数 ----

	//ポーションを使うプレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayerBase;
};
