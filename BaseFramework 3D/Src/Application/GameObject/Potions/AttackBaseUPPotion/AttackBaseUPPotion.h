#pragma once
class PlayerBase;

class AttackBaseUPPotion
{
public:
	//コンストラクタ
	AttackBaseUPPotion() {};
	//デストラクタ
	~AttackBaseUPPotion() {};

	//攻撃力増加(永久的)
	void UseAttackBaseUPPotion();

	//ポーションを使うプレイヤーをセット
	// _spPlayerBase ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }

private:

	//ポーションを使うプレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayerBase;

};
