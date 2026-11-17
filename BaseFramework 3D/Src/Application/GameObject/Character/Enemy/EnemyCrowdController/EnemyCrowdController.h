#pragma once
class EnemyBase;


class EnemyCrowdController
{
public:
	//コンストラクタ
	EnemyCrowdController() {};
	//デストラクタ
	~EnemyCrowdController() {};

	//敵の移動方向を、ほかの敵とぶつからないように決める
	// _enemy ... 動かす敵
	// _dt ... デルタタイム(秒)
	void Update(EnemyBase* _enemy, float _dt);

private:
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
