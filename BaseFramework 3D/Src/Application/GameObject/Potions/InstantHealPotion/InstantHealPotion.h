#pragma once
class PlayerBase;

class InstantHealPotion
{
public:
	//コンストラクタ
	InstantHealPotion() {};
	//デストラクタ
	~InstantHealPotion() {};

	//HP即回復
	void UseInstantHeal();

	//ポーションを使うプレイヤーをセット
	// _spPlayerBase ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }

private:

	//ポーションを使うプレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayerBase;

};
