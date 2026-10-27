#pragma once

#include"../EnemyAmbush/EnemyAmbush.h"

class AttackBase;

class Golem :public EnemyAmbush
{
public:
	Golem() {};
	~Golem()override { Release(); };


	void Init()override;
	void Update()override;
	void PreUpdate()override;

private:

	void Release()override;

	// 3D足音SE更新
	void UpdateFootstepSE();

	////////////////////////////////////////////
	//アニメーションモード
	struct GolemAnimeName
	{
		std::string IdleAnime = "idle";
		std::string WalkAnime = "walk";
		//すまっしゅ
		std::string smashAnime = "smash";
	};


	GolemAnimeName m_golemAnimeName;

	void ChangeAnime();



	//攻撃
	void Attack();
	void AttacksSlammingDown();

	bool m_attackFlg = false;
	bool m_hitTriggered = false;

	std::weak_ptr<AttackBase>m_wpAttack;

	//攻撃待機時間(秒)
	float m_attackWaitMax = 5;
	float m_attackWaitMine = 2;
	float m_attackWait = m_attackWaitMax;

	//叩きつけ攻撃のあたり判定を出すタイミング
	float m_SlammingDownAttackAnimeHitTriggerTime = 0.58f;
};
