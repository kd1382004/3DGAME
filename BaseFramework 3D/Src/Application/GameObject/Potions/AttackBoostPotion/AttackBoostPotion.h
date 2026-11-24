#pragma once
class PlayerBase;

//攻撃力一時アップポーション
class AttackBoostPotion
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	AttackBoostPotion() {};

	//デストラクタ
	~AttackBoostPotion() {};

	//---- 設定 ----

	//ポーションを使うプレイヤーをセット
	// _spPlayerBase ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }

	//---- その他の処理 ----

	//攻撃力増加(永久的)
	//_ID...ポーションID
	void UseAttackBoostPotion(int _ID);

private:

	//---- メンバ変数 ----

	//ポーションを使うプレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayerBase;
};
