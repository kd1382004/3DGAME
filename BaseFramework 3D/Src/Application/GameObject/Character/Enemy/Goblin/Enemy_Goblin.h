#pragma once

#include"../EnemyAmbush/EnemyAmbush.h"

class AttackBase;

class Goblin :public EnemyAmbush
{
public:
	Goblin() {};
	~Goblin()override {};


	void Init()override;
	void Update()override;

private:

	////////////////////////////////////////////
	//アニメーションモード
	struct GoblinAnimeName
	{
		std::string IdleAnime = "Idle";
		std::string WalkAnime = "Walk";
		std::string RunAnime = "Run";

		//叩きつけ攻撃
		std::string SlammingDownAttackAnime = "Attack.000";

		//ビンタ
		std::string SlapAttackAnime = "Attack.001";

		//ひっかき
		std::string ScratchAttackAnime = "Attack.002";

		//死亡
		std::string DieAnime = "Die";
	};


	GoblinAnimeName m_goblinAnimeName;

	void ChangeAnime();



	//攻撃
	void Attack();
	void AttacksSlammingDown();

	bool m_attackFlg=false;
	bool m_hitTriggered =false;

	std::weak_ptr<AttackBase>m_wpAttack;

	//攻撃待機時間(秒)
	float m_attackWaitMax=5;
	float m_attackWaitMine =2;
	float m_attackWait= m_attackWaitMax;

	//叩きつけ攻撃のあたり判定を出すタイミング
	float m_SlammingDownAttackAnimeHitTriggerTime = 0.58f;
};
