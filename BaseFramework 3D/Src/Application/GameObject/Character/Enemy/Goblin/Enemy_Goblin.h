#pragma once

#include"../EnemyAmbush/EnemyAmbush.h"

class AttackBase;

class Goblin :public EnemyAmbush
{
public:
	//コンストラクタ
	Goblin() {};
	//デストラクタ
	~Goblin()override { Release(); };


	//初期化
	void Init()override;
	//更新
	void Update()override;
	//更新前の処理
	void PreUpdate()override;

private:

	//解放
	void Release()override;

	// 3D足音SE更新
	void UpdateFootstepSE();

	////////////////////////////////////////////
	//アニメーションモード
	struct GoblinAnimeName
	{
		//待機のアニメーション名
		std::string m_idleAnime = "Idle";
		//歩きのアニメーション名
		std::string m_walkAnime = "Walk";
		//走りのアニメーション名
		std::string m_runAnime = "Run";

		//叩きつけ攻撃
		std::string m_slammingDownAttackAnime = "Attack.000";

		//ビンタ
		std::string m_slapAttackAnime = "Attack.001";

		//ひっかき
		std::string m_scratchAttackAnime = "Attack.002";

		//死亡
		std::string m_dieAnime = "Die";
	};


	//アニメーション名
	GoblinAnimeName m_goblinAnimeName;

	//今の状態に合わせてアニメーションを変える
	void ChangeAnime();



	//攻撃
	void Attack();
	//叩きつけ攻撃
	void AttacksSlammingDown();

	//攻撃中か
	bool m_attackFlg=false;
	//当たり判定を出したか
	bool m_hitTriggered =false;

	//攻撃の当たり判定
	std::weak_ptr<AttackBase>m_wpAttack;

	//攻撃待機時間(秒)
	float m_attackWaitMax=5;
	//攻撃待機時間の最小(秒)
	float m_attackWaitMin =2;
	//次の攻撃までの待機時間(秒)
	float m_attackWait= m_attackWaitMax;

	//叩きつけ攻撃のあたり判定を出すタイミング
	float m_slammingDownAttackAnimeHitTriggerTime = 0.58f;

	// 3D足音SE
	std::shared_ptr<KdSoundInstance3D> m_spWalk3DSE;
	//ダッシュの3D音
	std::shared_ptr<KdSoundInstance3D> m_spDash3DSE;
	//鳴き声の3D音
	std::shared_ptr<KdSoundInstance3D> m_spCrying3DSE;
	std::shared_ptr<KdSoundInstance3D> m_spSlam3DSE;	// 叩きつけの音
};
