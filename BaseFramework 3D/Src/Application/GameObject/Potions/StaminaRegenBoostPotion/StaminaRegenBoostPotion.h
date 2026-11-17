#pragma once
class PlayerBase;

class StaminaRegenBoostPotion
{
public:
	//コンストラクタ
	StaminaRegenBoostPotion() {};
	//デストラクタ
	~StaminaRegenBoostPotion() {};

	//スタミナ回復速度一時増加(時間制限バフ)
	//_ID...ポーションID
	void UseStaminaRegenBoostPotion(int _ID);

	//ポーションを使うプレイヤーをセット
	// _spPlayerBase ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }

private:

	//ポーションを使うプレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayerBase;

};
