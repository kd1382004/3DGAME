#pragma once
class PlayerBase;

class AttackJumpSlam :public KdGameObject
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ(攻撃のステータスを決める)
	AttackJumpSlam() {
		if (!m_pDebugWire)
		{
			m_pDebugWire = std::make_unique<KdDebugWireFrame>();
		}

		SetAttackStatus();
	};

	//デストラクタ
	~AttackJumpSlam() override{};

	//---- 設定 ----

	//右手(攻撃の中心)の位置をセット
	// _pos ... 位置
	void SetRPos(const Math::Vector3& _pos) { m_rPos = _pos; }

	//左手の位置をセット
	// _pos ... 位置
	void SetLPos(const Math::Vector3& _pos) { m_lPos = _pos; }

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

	//攻撃範囲を表示するかをセット
	// _flg ... 表示するならtrue
	void SetRadiusDrawFlg(bool _flg){ m_radiusDrawFlg = _flg; }

	//---- その他の処理 ----

	//攻撃ジャンプ攻撃
	void AttackJumpSlamUpdate();

private:

	//---- メンバ変数 ----

	//当てるプレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayerBase;

	//左手の位置
	Math::Vector3 m_lPos;

	//右手(攻撃の中心)の位置
	Math::Vector3 m_rPos;

	//当たり判定を出しているか
	bool m_hitFlg = false;

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

	//攻撃の範囲
	float m_radius = 7;

	//攻撃範囲表示用変数
	float m_radiusPercent = 0;

	//攻撃範囲を表示するか
	bool m_radiusDrawFlg=false;

	//---- 設定 ----

	//攻撃のステータス(倍率・ふっとばし・スタン)を決める
	void SetAttackStatus();

	//---- その他の処理 ----

	//読み込み(今は何もしない)
	void Load();
};
