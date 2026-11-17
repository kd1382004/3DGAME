#pragma once
class PlayerBase;

class InstantStaminaHealPotion
{
public:
	//コンストラクタ
	InstantStaminaHealPotion() {};
	//デストラクタ
	~InstantStaminaHealPotion() {};

	//スタミナ即回復
	void UseInstantStaminaHeal();

	//ポーションを使うプレイヤーをセット
	// _spPlayerBase ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }

private:

	//ポーションを使うプレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayerBase;

};
