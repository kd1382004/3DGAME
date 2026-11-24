#pragma once

#include"../EnemyAmbush/EnemyAmbush.h"

//敵(プレスボット)。工場マップのゴーレム枠。重くて腕が太いロボット
class PressBot :public EnemyAmbush
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	PressBot() {};

	//デストラクタ
	~PressBot()override { Release(); };

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
	struct PressBotAnimeName
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
	PressBotAnimeName m_pressBotAnimeName;

	//攻撃中か・攻撃の当たり判定・攻撃待機時間(m_attackFlg・m_wpPatternAttack・m_attackWait・m_attackWaitMax)は EnemyBase にある
	//攻撃の中身は Asset/Data/ObjeData/EnemyAttackPattern/PressBot.json(パターンのエディターで編集)

	//攻撃待機時間の最小(秒)
	float m_attackWaitMin = 2;

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

	//パターンを始めたときの処理(うなり音)
	void OnPatternStart()override;
};
