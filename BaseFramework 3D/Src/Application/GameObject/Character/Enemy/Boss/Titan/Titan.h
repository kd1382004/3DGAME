#pragma once
#include"../../EnemyBase.h"

//ボス(タイタン)。工場マップのジャイアント枠。巨大なロボット
class Titan :public EnemyBase
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	Titan() {};

	//デストラクタ
	~Titan() override { Release(); };

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

protected:

	//---- 取得 ----

	//使うモデルのパス(同じ骨・アニメーションで見た目だけ違う敵を作るときに、子クラスで上書きする)
	virtual std::string GetModelPath() const { return "Asset/Models/Character/Enemy/Factory/Titan/Titan.gltf"; }

private:

	//---- 型 ----

	//アニメーション
	struct TitanAnimeName
	{
		//待機のアニメーション名
		std::string m_idleAnime = "idle";
		//歩きのアニメーション名
		std::string m_walkAnime = "Walk";
		//走りのアニメーション名
		std::string m_runAnime = "Run";

		//死亡
		std::string m_dieAnime = "Death";

		//攻撃のアニメ(LeftPunch・RightPunch・DoubleSlam・Stomp・Spin・JumpSlam・Roar)は
		//Asset/Data/ObjeData/EnemyAttackPattern/Titan.json から使う
	};

	//---- メンバ変数 ----


	//アニメーション名
	TitanAnimeName m_titanAnimeName;

	// 3DサウンドSE
	std::shared_ptr<KdSoundInstance3D> m_spRun3DSE;


	//鳴き声の3D音
	std::shared_ptr<KdSoundInstance3D> m_spCrying3DSE;


	//---- 基本の処理 ----

	//解放
	void Release() override;

	//---- その他の処理 ----

	//今の状態に合わせてアニメーションを変える
	void ChangeAnime();

	// 3D足音SE更新
	void UpdateFootstepSE();
};
