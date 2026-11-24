#pragma once

#include"../EnemyAmbush/EnemyAmbush.h"

//敵(スクラップボット)。工場マップのゴブリン枠。小さくてすばしっこいロボット
class ScrapBot :public EnemyAmbush
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	ScrapBot() {};

	//デストラクタ
	~ScrapBot()override { Release(); };

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
	struct ScrapBotAnimeName
	{
		//待機のアニメーション名
		std::string m_idleAnime = "Idle";
		//歩きのアニメーション名
		std::string m_walkAnime = "Walk";
		//走りのアニメーション名
		std::string m_runAnime = "Run";

		//攻撃のアニメ(SlashR・SlashL・Slam・Thrust・Leap)は
		//Asset/Data/ObjeData/EnemyAttackPattern/ScrapBot.json から使う

		//死亡
		std::string m_dieAnime = "Die";
	};

	//---- メンバ変数 ----

	//アニメーション名
	ScrapBotAnimeName m_scrapBotAnimeName;

	//攻撃中か・攻撃の当たり判定・攻撃待機時間(m_attackFlg・m_wpPatternAttack・m_attackWait・m_attackWaitMax)は EnemyBase に移した

	//攻撃待機時間の最小(秒)
	float m_attackWaitMin =2;

	// 3D足音SE
	std::shared_ptr<KdSoundInstance3D> m_spWalk3DSE;

	//ダッシュの3D音
	std::shared_ptr<KdSoundInstance3D> m_spDash3DSE;

	//鳴き声の3D音
	std::shared_ptr<KdSoundInstance3D> m_spCrying3DSE;

	//---- 基本の処理 ----

	//解放
	void Release()override;

	//---- その他の処理 ----

	// 3D足音SE更新
	void UpdateFootstepSE();

	//今の状態に合わせてアニメーションを変える
	void ChangeAnime();

	//パターンを始めたときの処理(起動音)
	void OnPatternStart()override;
};
