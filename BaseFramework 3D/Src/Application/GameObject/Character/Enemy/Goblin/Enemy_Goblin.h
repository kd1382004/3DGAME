#pragma once

#include"../EnemyAmbush/EnemyAmbush.h"

class AttackBase;

class Goblin :public EnemyAmbush
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	Goblin() {};

	//デストラクタ
	~Goblin()override { Release(); };

	//---- 基本の処理 ----

	//初期化
	void Init()override;

	//更新
	void Update()override;

	//更新前の処理
	void PreUpdate()override;

	//攻撃を受けたとき(ひるみ値がたまったら攻撃を止める)
	void OnAttackHit(float _damage, float _knockbackDistance, const Math::Vector3& _knockbackDir, float _hitStunTime, bool _isCritical, float _ignoreRate, const Math::Vector3* _hitPos = nullptr)override;

private:

	//---- 型 ----

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

	//---- メンバ変数 ----

	//アニメーション名
	GoblinAnimeName m_goblinAnimeName;

	//攻撃中か
	bool m_attackFlg=false;

	//当たり判定を出したか(m_hitOn に置き換え。動きを確かめたら消す)
	//bool m_hitTriggered =false;

	//攻撃の当たり判定
	std::weak_ptr<AttackBase>m_wpAttack;

	//攻撃待機時間(秒)
	float m_attackWaitMax=5;

	//攻撃待機時間の最小(秒)
	float m_attackWaitMin =2;

	//次の攻撃までの待機時間(秒)
	float m_attackWait= m_attackWaitMax;

	//叩きつけ攻撃のあたり判定を出すタイミング(段の m_hitStart に置き換え。動きを確かめたら消す)
	//float m_slammingDownAttackAnimeHitTriggerTime = 0.58f;

	// 3D足音SE
	std::shared_ptr<KdSoundInstance3D> m_spWalk3DSE;

	//ダッシュの3D音
	std::shared_ptr<KdSoundInstance3D> m_spDash3DSE;

	//鳴き声の3D音
	std::shared_ptr<KdSoundInstance3D> m_spCrying3DSE;
	std::shared_ptr<KdSoundInstance3D> m_spSlam3DSE;	// 叩きつけの音

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

	//叩きつけ攻撃(StartPattern〜EndPattern に置き換え。動きを確かめたら消す)
	//void AttacksSlammingDown();



	//パターンを始める
	// _patternIndex ... m_patternList の番号
	void StartPattern(int _patternIndex);

	//段を始める(向き直し・アニメの切り替え・当たり判定の作り直し)
	// _stepIndex ... 何段目か(0が1段目)
	void StartStep(int _stepIndex);

	//攻撃中の毎フレームの処理(骨の位置・当たり判定のオンオフ・終わり)
	void UpdateStep();

	//パターンを終わる(当たり判定を消して、番号を-1に戻す)
	void EndPattern();

	//パターンをこの場でやめる(当たり判定を消して番号を-1に戻すだけ。アニメも待ち時間も触らない)
	//死んだとき・見失ったとき・解放するときに使う
	void CancelPattern();
};
