#pragma once
#include"../AttackBase.h"


class Attack_Slamming :public AttackBase
{
public:

	Attack_Slamming() {
		if (!m_pDebugWire)
		{
			m_pDebugWire = std::make_unique<KdDebugWireFrame>();
		}

		SetAttackStatus();
	};
	~Attack_Slamming()override {};

	//腕の位置
	void Update()override;

private:
	void SetAttackStatus()override;
};
