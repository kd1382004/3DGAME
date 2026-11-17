#include "Attack_Slamming.h"
#include"../../../../Player/PlayerBase.h"


void Attack_Slamming::Update()
{
	if (!m_hitFlg) { return; }



	KdCollider::SphereInfo sphereInfo;

	sphereInfo.m_sphere.Center = m_pos;
	sphereInfo.m_sphere.Radius = 2.0;
	sphereInfo.m_type = KdCollider::Type::TypeDamage;

	std::shared_ptr<PlayerBase>spPlayerBase = m_wpPlayerBase.lock();
	if (spPlayerBase)
	{
		if (spPlayerBase->Intersects(sphereInfo, nullptr))
		{
			m_hitFlg = false;

			float damage = m_attackPower * m_attackMagnification;
			spPlayerBase->OnAttackHit(damage, m_knockbackDistance, m_knockbackDir, m_hitStunTime, false, 0);
		}
	}

	if (m_pDebugWire)
	{
		m_pDebugWire->AddDebugSphere(sphereInfo.m_sphere.Center, sphereInfo.m_sphere.Radius);
	}
}

void Attack_Slamming::SetAttackStatus()
{
	//攻撃力倍率
	m_attackMagnification = 1.3;

	//ふっとばし距離
	m_knockbackDistance = 0;

	//スタン時間
	m_hitStunTime = 0.2;
}
