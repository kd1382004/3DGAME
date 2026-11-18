#pragma once
class PlayerBase;

class InstantStaminaHealPotion
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	InstantStaminaHealPotion() {};

	//デストラクタ
	~InstantStaminaHealPotion() {};

	//---- 設定 ----

	//ポーションを使うプレイヤーをセット
	// _spPlayerBase ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }

	//---- その他の処理 ----

	//スタミナ即回復
	void UseInstantStaminaHeal();

private:

	//---- メンバ変数 ----

	//ポーションを使うプレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayerBase;
};
