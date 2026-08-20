#pragma once
#include"../../EnemyBase.h"

class GameScene;

class AttackJumpSlam;
class AttackLeftPunch;

class Giant :public EnemyBase
{
public:
	Giant() {};
	~Giant() {};

	void Init()override;
	void PreUpdate()override;
	void Update()override;
	void PostUpdate()override;
	void PreDraw()override;
	void GenerateDepthMapFromLight()override;
	void DrawLit()override;


	//攻撃をくらった処理
	//_damage ... ダメージ量
	//_knockbackDistance ... ふっとばし距離
	//_knockbackDir ... ふっとばし方向
	// _hitStunTime ... のけぞり時間
	//_isCritical ... クリティカルかどうか
	//__ignoreRate ... 防御無視
	void OnAttackHit(float _damage, float _knockbackDistance, const Math::Vector3& _knockbackDir, float _hitStunTime, bool _isCritical, float _ignoreRate)override;

	//ゲームシーンセット
	void SetGameScene(std::shared_ptr<GameScene>_spGameScene) { m_wpGameScene = _spGameScene; }
private:
	//ゲームシーン
	std::weak_ptr<GameScene> m_wpGameScene;

	////////////////////////
	//攻撃モード
	enum GiantAttackMode
	{
		//左パンチ
		LeftPunchAttack,

		//右パンチ
		RightAttack,

		//ジャンプ攻撃
		JumpSlamAttack
	};

	GiantAttackMode m_giantAttackMode;

	void AttackMode();

	bool m_attackFlg;

	void ChangeAttackAnime();
	void AttackUpdate();

	float m_attackCoolTime = 0;

	/////////////////////////////////////////////////
	//左攻撃
	std::weak_ptr<AttackLeftPunch> m_leftAttack;

	void LeftAttackUpdate();

	//あたり判定許可を出したか
	bool m_IsAttackleftHITFlg = false;

	//はじめ
	float m_leftAttackHitStart = 0.5f;
	float m_leftAttackHitEnd = 0.6f;


	/////////////////////////////////////////////////
	//ジャンプ攻撃
	std::weak_ptr<AttackJumpSlam>m_wpAttackJumpSlam;

	enum JumpSlamAttackMode
	{
		//ジャンプ攻撃中
		JumpSlamAttackMode_JumpSlamAttack,

		//アイドル中
		JumpSlamAttackMode_Idle,
	};

	JumpSlamAttackMode m_JumpSlamAttackMode;

	void JumpSlamAttackMode_JumpSlamAttackUpdate();
	void JumpSlamAttackMode_IdleUpdate();

	void AttackJumpSlamUpdate();
	void AttackJumpSlamAnimeUpdate();

	//hitする時間

	//あたり判定許可を出したか
	bool m_IsAttackJumpSlamHITFlg = false;

	//はじめ
	float m_JumpSlamHitStart = 0.45f;
	float m_JumpSlamHitEnd = 0.5f;

	//ジャンプ開始地点
	Math::Vector3 m_jnpStartPos;

	//Idle時間MAX
	float m_attackJumpSlamIdleMax = 5;
	float m_attackJumpSlamIdleNow = 0;

	/////////////////////////////////////////////////
	//アニメーション
	struct GiantAnimeName
	{
		std::string IdleAnime = "idle";
		std::string RunAnime = "Giant Run";

		//ジャンプ攻撃
		std::string JumpSlamAttackAnime = "Jump Slam";

		//左パンチ
		std::string LeftPunchAttackAnime = "Left Punch";

		//右パンチ
		std::string RightAttackAnime = "Right Punch";

		//死亡
		std::string DieAnime = "Death";
	};

	GiantAnimeName m_giantAnimeName;
	void ChangeAnime();


	/////////////////////////////////////////////////
	//ノード

	//左手首
	const std::string BONE_LEFT_HAND = "mixamorigLeftHand";

	//右手首
	const std::string BONE_RIGHT_HAND = "mixamorigRightHand";

	//ノード名を入れる
	Math::Matrix GetBoneWorldMatrix(std::string _nodeName);
};
