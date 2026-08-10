#pragma once

class PlayerBase;

class AttackBase :public KdGameObject
{
public:
	AttackBase() { };
	~AttackBase()override {};

	virtual void Init()override {};
	virtual void Update()override {};

	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }

	void SetPos(const Math::Vector3& pos)override { m_pos = pos; }


	void FinishAttack_Slamming() { m_isExpired = true; }

	void SetAttckFlg(bool _flg = true) { m_hitFlg = _flg; }
	void SetAttckPower(float _attackPower) { m_attckPower = _attackPower; }

	//ふっとばし方向セット
	void SetKnockbackDir(Math::Vector3 _dir) { m_knockbackDir = _dir; }
protected:

	virtual void SetAttackStatus();

	std::weak_ptr<PlayerBase>m_wpPlayerBase;

	Math::Vector3 m_pos;

	bool m_hitFlg = false;


	//攻撃力倍率
	float m_attckMagnification =0;
	float m_attckPower = 0;

	//ふっとばし距離
	float m_knockbackDistance = 0;
	
	//ふっとばし方向
	Math::Vector3  m_knockbackDir = Math::Vector3::Zero;

	//スタン時間
	float m_hitStunTime = 0;
};

