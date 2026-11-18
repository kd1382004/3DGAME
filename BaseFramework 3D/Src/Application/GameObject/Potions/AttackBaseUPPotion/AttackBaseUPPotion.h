#pragma once
class PlayerBase;

class AttackBaseUPPotion
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	AttackBaseUPPotion() {};

	//デストラクタ
	~AttackBaseUPPotion() {};

	//---- 設定 ----

	//ポーションを使うプレイヤーをセット
	// _spPlayerBase ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }

	//---- その他の処理 ----

	//攻撃力増加(永久的)
	void UseAttackBaseUPPotion();

private:

	//---- メンバ変数 ----

	//ポーションを使うプレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayerBase;
};
