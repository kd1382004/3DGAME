#pragma once
#include"../../EnemyBase.h"

//ボス(巨人)
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

	//ゲームシーンは EnemyBase の m_wpGameScene を使う
	//(ここに同じ名前の変数を作ると、EnemyManager が EnemyBase::SetGameScene で入れた値が見えず、
	// 倒してもワープゲートが出なくなる)

	//アニメーション名
	GiantAnimeName m_giantAnimeName;

	//ノード

	//左手首
	const std::string kBoneLeftHand = "mixamorigLeftHand";

	//右手首
	const std::string kBoneRightHand = "mixamorigRightHand";

	// 3DサウンドSE
	std::shared_ptr<KdSoundInstance3D> m_spRun3DSE;


	//鳴き声の3D音
	std::shared_ptr<KdSoundInstance3D> m_spCrying3DSE;


	//---- 基本の処理 ----

	//解放
	void Release() override;

	//---- 取得 ----

	//ノード名を入れる
	// _nodeName ... ノード名
	Math::Matrix GetBoneWorldMatrix(std::string _nodeName);

	//---- その他の処理 ----

	//今の状態に合わせてアニメーションを変える
	void ChangeAnime();

	// 3D足音SE更新
	void UpdateFootstepSE();
};
