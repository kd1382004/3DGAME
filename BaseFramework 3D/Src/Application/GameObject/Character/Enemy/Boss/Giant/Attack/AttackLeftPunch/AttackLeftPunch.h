#pragma once
class PlayerBase;

class AttackLeftPunch :public KdGameObject
{
public:
	//コンストラクタ(攻撃のステータスを決める)
	AttackLeftPunch()
	{
		if (!m_pDebugWire)
		{
			m_pDebugWire = std::make_unique<KdDebugWireFrame>();
		}

		SetAttackStatus();
	};
	//デストラクタ
	~AttackLeftPunch() {};


	//攻撃左パンチ攻撃
	void AttackLeftPunchUpdate();

	//攻撃の中心の位置をセット
	// _pos ... 位置
	void SetTargetCenterPos(const Math::Vector3& _pos) { m_targetCenterPos = _pos; }

	//当たり判定を出すかをセット
	// _flg ... 出すならtrue
	void SetAttackFlg(bool _flg = true) { m_hitFlg = _flg; }
	//攻撃力をセット
	// _attackPower ... 攻撃力
	void SetAttackPower(float _attackPower) { m_attackPower = _attackPower; }

	//ふっとばし方向セット
	// _dir ... ふっとばす方向
	void SetKnockbackDir(Math::Vector3 _dir) {
		m_knockbackDir = _dir;
		m_knockbackDir.y = 0;
		m_knockbackDir.Normalize();
	}


	//この攻撃を消すかをセット
	// _flg ... 消すならtrue
	void SetExpired(bool _flg) { m_isExpired = _flg; }

	//当てるプレイヤーをセット
	// _spPlayerBase ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }


	//ライトありの描画(今は何もしない)
	void DrawLit()override;

	//攻撃の向き(Y軸の角度)をセット
	// _angY ... 角度(度)
	void SetAngleY(float _angY) { m_angleY = _angY; }
private:

	//攻撃のステータス(倍率・ふっとばし・スタン)を決める
	void SetAttackStatus();

	//読み込み(今は何もしない)
	void Load();

	//当てるプレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayerBase;

	//攻撃の中心の位置
	Math::Vector3 m_targetCenterPos;

	//当たり判定を出しているか
	bool m_hitFlg = false;

	//攻撃の向き(Y軸の角度)
	float m_angleY = 0;

	//攻撃力倍率
	float m_attackMagnification = 1.5;
	//攻撃力
	float m_attackPower = 0;

	//ふっとばし距離
	float m_knockbackDistance = 0;

	//ふっとばし方向
	Math::Vector3  m_knockbackDir = Math::Vector3::Zero;

	//スタン時間
	float m_hitStunTime = 0;


	//攻撃の箱の大きさ(半分の大きさ)
	Math::Vector3 m_boxSiz = { 10,5,5 };
	//攻撃範囲表示の広がり具合(0〜1)
	float m_boxPercent = 0;
};