#pragma once
#include"../../EnemyBase.h"

class GameScene;

class AttackJumpSlam;
class AttackLeftPunch;

class Giant :public EnemyBase
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	Giant() {};

	//デストラクタ
	~Giant() override { Release(); };

	//---- 基本の処理 ----

	//初期化
	void Init()override;

	//更新前の処理
	void PreUpdate()override;

	//更新
	void Update()override;

	//更新後の処理
	void PostUpdate()override;

	//描画前の処理
	void PreDraw()override;

	//影の描画
	void GenerateDepthMapFromLight()override;

	//ライトありの描画
	void DrawLit()override;

	//---- 設定 ----

	//ゲームシーンセット
	// _spGameScene ... ゲームシーン
	void SetGameScene(std::shared_ptr<GameScene>_spGameScene) { m_wpGameScene = _spGameScene; }

	//---- その他の処理 ----

	//攻撃をくらった処理
	//_damage ... ダメージ量
	//_knockbackDistance ... ふっとばし距離
	//_knockbackDir ... ふっとばし方向
	// _hitStunTime ... のけぞり時間
	//_isCritical ... クリティカルかどうか
	//_ignoreRate ... 防御無視
	// _hitPos ... 当たった位置(ダメージ数字を出す場所。nullptrなら頭の上)
	void OnAttackHit(float _damage, float _knockbackDistance, const Math::Vector3& _knockbackDir, float _hitStunTime, bool _isCritical, float _ignoreRate, const Math::Vector3* _hitPos = nullptr)override;

private:

	//---- 型 ----

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

	//ジャンプ攻撃の状態
	enum JumpSlamAttackMode
	{
		//ジャンプ攻撃中
		JumpSlamAttackMode_JumpSlamAttack,

		//アイドル中
		JumpSlamAttackMode_Idle,
	};

	//アニメーション
	struct GiantAnimeName
	{
		//待機のアニメーション名
		std::string m_idleAnime = "idle";
		//走りのアニメーション名
		std::string m_runAnime = "Giant Run";

		//ジャンプ攻撃
		std::string m_jumpSlamAttackAnime = "Jump Slam";

		//左パンチ
		std::string m_leftPunchAttackAnime = "Left Punch";

		//右パンチ
		std::string m_rightAttackAnime = "Right Punch";

		//死亡
		std::string m_dieAnime = "Death";
	};

	//---- メンバ変数 ----

	//ゲームシーン
	std::weak_ptr<GameScene> m_wpGameScene;

	//今の攻撃の種類
	GiantAttackMode m_giantAttackMode;

	//攻撃中か
	bool m_attackFlg;

	//次の攻撃までの経過時間
	float m_attackCoolTime = 0;

	//攻撃の間隔(秒)
	float m_attackCoolTimeMax = 3;

	//左攻撃
	std::weak_ptr<AttackLeftPunch> m_wpLeftAttack;

	//あたり判定許可を出したか
	bool m_isAttackleftHITFlg = false;

	//はじめ
	float m_leftAttackHitStart = 0.5f;

	//左パンチの当たり判定を終わるタイミング(アニメーションの割合)
	float m_leftAttackHitEnd = 0.6f;

	//ジャンプ攻撃
	std::weak_ptr<AttackJumpSlam>m_wpAttackJumpSlam;

	//今のジャンプ攻撃の状態
	JumpSlamAttackMode m_jumpSlamAttackMode;

	//hitする時間

	//あたり判定許可を出したか
	bool m_isAttackJumpSlamHITFlg = false;

	//はじめ
	float m_jumpSlamHitStart = 0.45f;

	//ジャンプ攻撃の当たり判定を終わるタイミング(アニメーションの割合)
	float m_jumpSlamHitEnd = 0.5f;

	//ジャンプ開始地点
	Math::Vector3 m_jumpStartPos;

	//Idle時間MAX
	float m_attackJumpSlamIdleMax = 5;

	//今の待機時間
	float m_attackJumpSlamIdleNow = 0;

	//ターゲット座標
	Math::Vector3 m_attackJumpSlamTargetPos;

	//アニメーション名
	GiantAnimeName m_giantAnimeName;

	//ノード

	//左手首
	const std::string kBoneLeftHand = "mixamorigLeftHand";

	//右手首
	const std::string kBoneRightHand = "mixamorigRightHand";

	// 3DサウンドSE
	std::shared_ptr<KdSoundInstance3D> m_spRun3DSE;

	//パンチの3D音
	std::shared_ptr<KdSoundInstance3D> m_spPunch3DSE;

	//鳴き声の3D音
	std::shared_ptr<KdSoundInstance3D> m_spCrying3DSE;

	//着地(ドン)の3D音
	std::shared_ptr<KdSoundInstance3D> m_spDon3DSE;

	//ジャンプ攻撃の着地エフェクトを出したか
	bool m_jumpSlam_EndEfkefcFlg = false;

	//ジャンプ攻撃の開始エフェクトを出したか
	bool m_jumpSlam_StartEfkefcFlg = false;

	//---- 基本の処理 ----

	//解放
	void Release() override;

	//---- 取得 ----

	//ノード名を入れる
	// _nodeName ... ノード名
	Math::Matrix GetBoneWorldMatrix(std::string _nodeName);

	//---- その他の処理 ----

	//攻撃の種類を決める
	void AttackMode();

	//攻撃の種類に合わせてアニメーションを変える
	void ChangeAttackAnime();

	//攻撃の更新
	void AttackUpdate();

	//左パンチの更新
	void LeftAttackUpdate();

	//ジャンプ攻撃中の更新
	void JumpSlamAttackMode_JumpSlamAttackUpdate();

	//ジャンプ攻撃のあとの待機中の更新
	void JumpSlamAttackMode_IdleUpdate();

	//ジャンプ攻撃の更新
	void AttackJumpSlamUpdate();

	//ジャンプ攻撃のアニメーションの更新
	void AttackJumpSlamAnimeUpdate();

	//今の状態に合わせてアニメーションを変える
	void ChangeAnime();

	// 3D足音SE更新
	void UpdateFootstepSE();
};
