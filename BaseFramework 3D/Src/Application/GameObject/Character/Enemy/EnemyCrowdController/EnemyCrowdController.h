#pragma once
class EnemyBase;


//敵同士の押し合い・群れ制御
class EnemyCrowdController
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	EnemyCrowdController() {};

	//デストラクタ
	~EnemyCrowdController() {};

	//---- 基本の処理 ----

	//敵の移動方向を、ほかの敵とぶつからないように決める
	// _enemy ... 動かす敵
	// _dt ... デルタタイム(秒)
	void Update(EnemyBase* _enemy, float _dt);

private:

	//---- その他の処理 ----

	//プレイヤーへ向かう
	// _enemy ... 計算する敵
	Math::Vector3 CalcSeparation(EnemyBase* _enemy);

	//敵同士の押し返し
	// _enemy ... 計算する敵
	Math::Vector3 CalcAvoidance(EnemyBase* _enemy);

	//進行方向の敵からずれる
	// _enemy ... 計算する敵
	Math::Vector3 CalcToPlayer(EnemyBase* _enemy);
};
