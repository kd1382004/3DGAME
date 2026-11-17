#pragma once
class PlayerBase;

class StaminaRegenBaseUPPotion
{
public:
	//コンストラクタ
	StaminaRegenBaseUPPotion() {};
	//デストラクタ
	~StaminaRegenBaseUPPotion() {};

	//スタミナ回復速度増加(永久的)
	void UseStaminaRegenBaseUPPotion();

	//ポーションを使うプレイヤーをセット
	// _spPlayerBase ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }

private:

	//ポーションを使うプレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayerBase;

};
