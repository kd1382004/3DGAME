#pragma once
#include"../AttackBase.h"


class Attack_Slamming :public AttackBase
{
public:

	//コンストラクタ(攻撃のステータスを決める)
	Attack_Slamming() {
		if (!m_pDebugWire)
		{
			m_pDebugWire = std::make_unique<KdDebugWireFrame>();
		}

		SetAttackStatus();
	};
	//デストラクタ
	~Attack_Slamming()override {};

	//腕の位置
	void Update()override;

private:
	//攻撃のステータス(倍率・ふっとばし・スタン)を決める
	void SetAttackStatus()override;

};
