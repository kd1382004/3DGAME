#pragma once

#include"../EnemyAmbush/EnemyAmbush.h"

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

	//攻撃中か・攻撃の当たり判定・攻撃待機時間(m_attackFlg・m_wpPatternAttack・m_attackWait・m_attackWaitMax)は EnemyBase に移した

	//攻撃待機時間の最小(秒)
	float m_attackWaitMin =2;

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

	//パターンを始めたときの処理(叫び声)
	void OnPatternStart()override;
};
