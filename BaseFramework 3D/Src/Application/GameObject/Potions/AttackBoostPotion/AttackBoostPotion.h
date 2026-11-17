#pragma once
class PlayerBase;

class AttackBoostPotion
{
public:
	//コンストラクタ
	AttackBoostPotion() {};
	//デストラクタ
	~AttackBoostPotion() {};

	//攻撃力増加(永久的)
	//_ID...ポーションID
	void UseAttackBoostPotion(int _ID);

	//ポーションを使うプレイヤーをセット
	// _spPlayerBase ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }

private:

	//ポーションを使うプレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayerBase;

};
