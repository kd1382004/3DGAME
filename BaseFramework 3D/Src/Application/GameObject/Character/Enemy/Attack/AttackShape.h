#pragma once
#include"../EnemyBase.h"

class PlayerBase;

//////////////////////////////////////////////////////////////
// 敵の攻撃の当たり判定(骨・円・箱を外から選べる)
// Bone   → m_pos を中心に半径 m_radius の球
// Circle → m_pos を中心に半径 m_radius の球 + 予告の円(AttackJumpSlam と同じ)
// Box    → m_pos を中心に m_boxSize の回転する箱 + 予告の箱(AttackLeftPunch と同じ)
//////////////////////////////////////////////////////////////
class AttackShape :public KdGameObject
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	AttackShape()
	{
		if (!m_pDebugWire)
		{
			m_pDebugWire = std::make_unique<KdDebugWireFrame>();
		}
	}

	//デストラクタ
	~AttackShape() override {}

	//---- 基本の処理 ----

	//更新(予告の表示と、当たったかの判定)
	void Update() override;

	//---- 設定 ----

	//当たり判定の形をセット
	// _shape ... Bone / Circle / Box
	void SetShape(AttackHitShape _shape) { m_shape = _shape; }

	//中心の位置をセット
	// _pos ... 位置
	void SetPos(const Math::Vector3& _pos) override { m_pos = _pos; }

	//向き(Box だけ使う)をセット
	// _angleY ... Y軸の角度(度)
	void SetAngleY(float _angleY) { m_angleY = _angleY; }

	//半径(Bone・Circle)をセット
	// _radius ... 半径
	void SetRadius(float _radius) { m_radius = _radius; }

	//箱の大きさの半分(Box)をセット
	// _size ... 横・高さ・奥行きの半分
	void SetBoxSize(const Math::Vector3& _size) { m_boxSize = _size; }

	//当たり判定を出すかをセット
	// _flg ... 出すならtrue
	void SetAttackFlg(bool _flg = true) { m_hitFlg = _flg; }

	//予告(円・箱)を出すかをセット
	// _flg ... 出すならtrue
	void SetWarnFlg(bool _flg) { m_warnFlg = _flg; }

	//攻撃力をセット(敵の攻撃力 × 段の m_damageRate を渡す)
	// _attackPower ... 攻撃力
	void SetAttackPower(float _attackPower) { m_attackPower = _attackPower; }

	//ふっとばしをセット
	// _distance ... ふっとばし距離
	// _dir ... ふっとばす方向
	void SetKnockback(float _distance, const Math::Vector3& _dir)
	{
		m_knockbackDistance = _distance;
		m_knockbackDir = _dir;
		m_knockbackDir.y = 0;
		m_knockbackDir.Normalize();
	}

	//当てるプレイヤーをセット
	// _spPlayer ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase> _spPlayer) { m_wpPlayer = _spPlayer; }

	//---- その他の処理 ----

	//攻撃を終わる(この当たり判定を消す)
	void FinishAttack() { m_isExpired = true; }

private:

	//---- メンバ変数 ----

	//当てるプレイヤー
	std::weak_ptr<PlayerBase> m_wpPlayer;

	//形
	AttackHitShape m_shape = AttackHitShape::Bone;

	//中心の位置
	Math::Vector3 m_pos = Math::Vector3::Zero;

	//向き(度。Box だけ)
	float m_angleY = 0.0f;

	//半径(Bone・Circle)
	float m_radius = 1.0f;

	//箱の大きさの半分(Box)
	Math::Vector3 m_boxSize = { 1.0f, 1.0f, 1.0f };

	//当たり判定を出しているか
	bool m_hitFlg = false;

	//予告を出しているか
	bool m_warnFlg = false;

	//予告が広がる動き(0〜1を繰り返す)
	float m_warnPercent = 0.0f;

	//攻撃力
	float m_attackPower = 0.0f;

	//ふっとばし距離
	float m_knockbackDistance = 0.0f;

	//ふっとばし方向
	Math::Vector3 m_knockbackDir = Math::Vector3::Zero;

	//スタン時間
	float m_hitStunTime = 0.0f;

	//---- その他の処理 ----

	//予告を描く(Circle・Box)
	void DrawWarn();
};
