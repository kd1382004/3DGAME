#pragma once
class PlayerBase;

//スタミナ回復量一時アップポーション
class StaminaRegenBoostPotion
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	StaminaRegenBoostPotion() {};

	//デストラクタ
	~StaminaRegenBoostPotion() {};

	//---- 設定 ----

	//ポーションを使うプレイヤーをセット
	// _spPlayerBase ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }

	//---- その他の処理 ----

	//スタミナ回復速度一時増加(時間制限バフ)
	//_ID...ポーションID
	void UseStaminaRegenBoostPotion(int _ID);

private:

	//---- メンバ変数 ----

	//ポーションを使うプレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayerBase;
};
