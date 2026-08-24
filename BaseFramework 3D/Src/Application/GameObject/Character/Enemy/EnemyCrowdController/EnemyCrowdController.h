#pragma once
class EnemyBase;


class EnemyCrowdController
{
public:
	EnemyCrowdController() {};
	~EnemyCrowdController() {};

	void Update(EnemyBase* enemy, float dt);

private:
	//プレイヤーへ向かう
	Math::Vector3 CalcSeparation(EnemyBase* enemy);

	//敵同士の押し返し
	Math::Vector3 CalcAvoidance(EnemyBase* enemy);

	//進行方向の敵からずれる
	Math::Vector3 CalcToPlayer(EnemyBase* enemy);
};
