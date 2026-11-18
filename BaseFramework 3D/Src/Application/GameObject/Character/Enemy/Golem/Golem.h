#pragma once

#include"../EnemyAmbush/EnemyAmbush.h"

class AttackBase;

class Golem :public EnemyAmbush
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	Golem() {};

	//デストラクタ
	~Golem()override { Release(); };

	//---- 基本の処理 ----

	//初期化
	void Init()override;

	//更新
	void Update()override;

	//更新前の処理
	void PreUpdate()override;

private:

	//---- 型 ----

	//アニメーションモード
	struct GolemAnimeName
	{
		//待機のアニメーション名
		std::string m_idleAnime = "idle";
		//歩きのアニメーション名
		std::string m_walkAnime = "walk";
		//走りのアニメーション名
		std::string m_dashAnime = "run";
		//死亡のアニメーション名
		std::string m_dieAnime = "death";
		//すまっしゅ
		std::string m_smashAnime = "smash";
	};

	//---- メンバ変数 ----

	//アニメーション名
	GolemAnimeName m_golemAnimeName;

	//攻撃中か
	bool m_attackFlg = false;

	//当たり判定を出したか
	bool m_hitTriggered = false;

	//攻撃の当たり判定
	std::weak_ptr<AttackBase>m_wpAttack;

	//攻撃待機時間(秒)
	float m_attackWaitMax = 5;

	//攻撃待機時間の最小(秒)
	float m_attackWaitMin = 2;

	//次の攻撃までの待機時間(秒)
	float m_attackWait = m_attackWaitMax;

	//叩きつけ攻撃のあたり判定を出すタイミング
	float m_smashAttackAnimeHitTriggerTime = 0.46f;

	// 3D効果音
	std::shared_ptr<KdSoundInstance3D> m_spWalk3DSE;	// 足音（1歩ぶんの音を鳴り終わるたびに鳴らし直す）
	std::shared_ptr<KdSoundInstance3D> m_spCrying3DSE;	// 攻撃を始めるときのうなり声
	std::shared_ptr<KdSoundInstance3D> m_spSmash3DSE;	// 叩きつけの音

	//---- 基本の処理 ----

	//解放
	void Release()override;

	//---- その他の処理 ----

	// 3D足音SE更新
	void UpdateFootstepSE();

	//今の状態に合わせてアニメーションを変える
	void ChangeAnime();

	//攻撃
	void Attack();

	//叩きつけ攻撃
	void AttacksSlammingDown();
};
