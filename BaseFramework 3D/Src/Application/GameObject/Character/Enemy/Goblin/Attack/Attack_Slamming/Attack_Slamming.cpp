#include "Attack_Slamming.h"
#include"../../../../Player/PlayerBase.h"


void Attack_Slamming::Update()
{
	if (!m_hitFlg) { return; }



	KdCollider::SphereInfo spherInfo;

	spherInfo.m_sphere.Center = m_pos;
	spherInfo.m_sphere.Radius = 2.0;
	spherInfo.m_type = KdCollider::Type::TypeDamage;

	std::shared_ptr<PlayerBase>spPlayerBase = m_wpPlayerBase.lock();
	if (spPlayerBase)
	{
		if (spPlayerBase->Intersects(spherInfo, nullptr))
		{
			m_hitFlg = false;

			float dmage = m_attckPower * m_attckMagnification;
			spPlayerBase->OnAttackHit(dmage, m_knockbackDistance, m_knockbackDir, m_hitStunTime, false, 0);
		}
	}

	if (m_pDebugWire)
	{
		m_pDebugWire->AddDebugSphere(spherInfo.m_sphere.Center, spherInfo.m_sphere.Radius);
	}
}

void Attack_Slamming::SetAttackStatus()
{
	//攻撃力倍率
	m_attckMagnification = 1.3;

	//ふっとばし距離
	m_knockbackDistance = 10;

	//スタン時間
	m_hitStunTime = 1;
}
