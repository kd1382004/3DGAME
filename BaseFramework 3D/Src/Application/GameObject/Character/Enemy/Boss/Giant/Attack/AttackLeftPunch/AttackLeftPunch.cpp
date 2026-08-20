#include "AttackLeftPunch.h"
#include"../../../../../Player/PlayerBase.h"
void AttackLeftPunch::AttackLeftPunchUpdate()
{

	if (!m_hitFlg) { return; }

	KdCollider::SphereInfo spherLInfo;

	spherLInfo.m_sphere.Center = m_Lpos;
	spherLInfo.m_sphere.Center.y -= 1;
	spherLInfo.m_sphere.Radius = 3;
	spherLInfo.m_type = KdCollider::Type::TypeDamage;

	std::shared_ptr<PlayerBase>spPlayerBase = m_wpPlayerBase.lock();
	if (spPlayerBase)
	{
		if (spPlayerBase->Intersects(spherLInfo, nullptr))
		{
			m_hitFlg = false;

			float dmage = m_attckPower * m_attckMagnification;
			spPlayerBase->OnAttackHit(dmage, m_knockbackDistance, m_knockbackDir, m_hitStunTime, false, 0);
		}
	}

	if (m_pDebugWire)
	{
		m_pDebugWire->AddDebugSphere(spherLInfo.m_sphere.Center, spherLInfo.m_sphere.Radius);
	}
}

void AttackLeftPunch::SetAttackStatus()
{
	//攻撃力倍率
	m_attckMagnification = 1.2;

	//ふっとばし距離
	m_knockbackDistance = 1;

	//スタン時間
	m_hitStunTime = 0;
}

void AttackLeftPunch::Load()
{

}
