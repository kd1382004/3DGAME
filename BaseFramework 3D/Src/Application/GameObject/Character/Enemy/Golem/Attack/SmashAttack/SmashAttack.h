#pragma once
#include"../AttackBase.h"

class SmashAttack :public AttackBase
{
public:

	//コンストラクタ(攻撃のステータスを決める)
	SmashAttack() {
		if (!m_pDebugWire)
		{
			m_pDebugWire = std::make_unique<KdDebugWireFrame>();
		}

		SetAttackStatus();
	};
	//デストラクタ
	~SmashAttack()override {};

	//腕の位置
	void Update()override;

private:
	//攻撃のステータス(倍率・ふっとばし・スタン・箱の大きさ)を決める
	void SetAttackStatus()override;


	//攻撃の中心の位置
	Math::Vector3 m_targetCenterPos;
	//攻撃の箱の大きさ(半分の大きさ)
	Math::Vector3 m_boxSiz = { 10,5,5 };
};
