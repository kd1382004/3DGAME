#pragma once

class PlayerBase;

class AttackBase :public KdGameObject
{
public:
	//コンストラクタ
	AttackBase() { };
	//デストラクタ
	~AttackBase()override {};

	//初期化
	virtual void Init()override {};
	//更新
	virtual void Update()override {};

	//プレイヤーをセット
	// _spPlayerBase ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }

	//座標をセット
	// _pos ... 座標
	void SetPos(const Math::Vector3& _pos)override
	{ 
		m_pos = _pos;
		m_mWorld.Translation(_pos);
	}


	//攻撃を終わる(この当たり判定を消す)
	void FinishAttack_Slamming() { m_isExpired = true; }

	//当たり判定を出すかをセット
	// _flg ... 出すならtrue
	void SetAttackFlg(bool _flg = true) { m_hitFlg = _flg; }
	//攻撃力をセット
	// _attackPower ... 攻撃力
	void SetAttackPower(float _attackPower) { m_attackPower = _attackPower; }

	//ふっとばし方向セット
	// _dir ... ふっとばす方向
	void SetKnockbackDir(Math::Vector3 _dir) { m_knockbackDir = _dir; }

	//攻撃の向き(Y軸の角度)をセット
	// _angY ... 角度(度)
	void SetAngleY(float _angY) { m_angleY = _angY; }
protected:

	//攻撃のステータスを決める
	virtual void SetAttackStatus();

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


	//攻撃の向き(Y軸の角度)
	float m_angleY = 0;
};

