#pragma once
class PlayerBase;

class StaminaMaxUPPotion
{
public:
	//コンストラクタ
	StaminaMaxUPPotion() {};
	//デストラクタ
	~StaminaMaxUPPotion() {};

	//最大スタミナ増加(永久的)
	void UseStaminaMaxUPPotion();

	//ポーションを使うプレイヤーをセット
	// _spPlayerBase ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }

private:

	//ポーションを使うプレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayerBase;

};
