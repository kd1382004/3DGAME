#pragma once
class PlayerBase;

//基礎スタミナ回復量アップポーション
class StaminaRegenBaseUPPotion
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	StaminaRegenBaseUPPotion() {};

	//デストラクタ
	~StaminaRegenBaseUPPotion() {};

	//---- 設定 ----

	//ポーションを使うプレイヤーをセット
	// _spPlayerBase ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }

	//---- その他の処理 ----

	//スタミナ回復速度増加(永久的)
	void UseStaminaRegenBaseUPPotion();

private:

	//---- メンバ変数 ----

	//ポーションを使うプレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayerBase;
};
