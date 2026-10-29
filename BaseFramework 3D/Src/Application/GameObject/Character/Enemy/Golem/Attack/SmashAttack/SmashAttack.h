#pragma once
#include"../AttackBase.h"

class SmashAttack :public AttackBase
{
public:

	SmashAttack() {
		if (!m_pDebugWire)
		{
			m_pDebugWire = std::make_unique<KdDebugWireFrame>();
		}

		SetAttackStatus();
	};
	~SmashAttack()override {};

	//腕の位置
	void Update()override;

private:
	void SetAttackStatus()override;


	Math::Vector3 m_targetCenterPos;
	Math::Vector3 m_boxSiz = { 10,5,5 };
};
