#pragma once
#include"../EnemyBase.h"

class EnemyAmbush :public EnemyBase
{
public:
	EnemyAmbush() {};
	~EnemyAmbush()override {};


	void Init()override;
	void Update()override;

	//徘徊
	void  Wander();


private:

	//目的地に向かってるかどうか
	bool m_isMovingToTarget = false;

	//徘徊半径
	float wanderRadius;

	//目的地
	Math::Vector3 m_targetPos;


};
