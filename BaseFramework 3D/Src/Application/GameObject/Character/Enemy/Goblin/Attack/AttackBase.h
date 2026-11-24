#pragma once

class PlayerBase;

//ゴブリンの攻撃の基底クラス
class AttackBase :public KdGameObject
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	AttackBase() { };

	//デストラクタ
	~AttackBase()override {};

	//---- 基本の処理 ----

	//初期化
	virtual void Init()override {};

	//更新
	virtual void Update()override {};

	//---- 設定 ----

	//プレイヤーをセット
	// _spPlayerBase ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }

	//座標をセット
	// _pos ... 座標
	void SetPos(const Math::Vector3& _pos)override { m_pos = _pos; }

	//当たり判定を出すかをセット
	// _flg ... 出すならtrue
	void SetAttackFlg(bool _flg = true) { m_hitFlg = _flg; }

	//攻撃力をセット
	// _attackPower ... 攻撃力
	void SetAttackPower(float _attackPower) { m_attackPower = _attackPower; }

	//ふっとばし方向セット
	// _dir ... ふっとばす方向
	void SetKnockbackDir(Math::Vector3 _dir) { m_knockbackDir = _dir; }

	//---- その他の処理 ----

	//攻撃を終わる(この当たり判定を消す)
	void FinishAttack_Slamming() { m_isExpired = true; }

protected:

	//---- メンバ変数 ----

	//プレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayerBase;

	//座標
	Math::Vector3 m_pos;

	//当たり判定を出しているか
	bool m_hitFlg = false;

	//攻撃力倍率
	float m_attackMagnification =0;

	//攻撃力
	float m_attackPower = 0;

	//ふっとばし距離
	float m_knockbackDistance = 0;

	//ふっとばし方向
	Math::Vector3  m_knockbackDir = Math::Vector3::Zero;

	//スタン時間
	float m_hitStunTime = 0;

	//---- 設定 ----

	//攻撃のステータスを決める
	virtual void SetAttackStatus();
};

