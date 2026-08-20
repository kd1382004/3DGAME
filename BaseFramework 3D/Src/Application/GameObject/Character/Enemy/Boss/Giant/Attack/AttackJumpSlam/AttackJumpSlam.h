#pragma once
class PlayerBase;

class AttackJumpSlam :public KdGameObject
{
public:
	AttackJumpSlam() {
		if (!m_pDebugWire)
		{
			m_pDebugWire = std::make_unique<KdDebugWireFrame>();
		}

		SetAttackStatus();
	};
	~AttackJumpSlam() override{};

	//攻撃ジャンプ攻撃
	void AttackJumpSlamUpdate();

	void SetRPos(const Math::Vector3& pos) { m_Rpos = pos; }
	void SetLPos(const Math::Vector3& pos) { m_Lpos = pos; }

	void SetAttckFlg(bool _flg = true) { m_hitFlg = _flg; }
	void SetAttckPower(float _attackPower) { m_attckPower = _attackPower; }

	//ふっとばし方向セット
	void SetKnockbackDir(Math::Vector3 _dir) { 
		m_knockbackDir = _dir; 
		m_knockbackDir.y = 0;
		m_knockbackDir.Normalize();
	}


	void SetExpired(bool _flg) { m_isExpired = _flg; }

	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }
private:

	void SetAttackStatus();

	void Load();

	std::weak_ptr<PlayerBase>m_wpPlayerBase;

	Math::Vector3 m_Lpos;
	Math::Vector3 m_Rpos;

	bool m_hitFlg = false;


	//攻撃力倍率
	float m_attckMagnification = 1.5;
	float m_attckPower = 0;

	//ふっとばし距離
	float m_knockbackDistance = 0;

	//ふっとばし方向
	Math::Vector3  m_knockbackDir = Math::Vector3::Zero;

	//スタン時間
	float m_hitStunTime = 0;
};
