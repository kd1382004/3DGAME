#include "AttackJumpSlam.h"

#include"../../../../../Player/PlayerBase.h"
void AttackJumpSlam::AttackJumpSlamUpdate()
{

	if (!m_hitFlg) { return; }



	KdCollider::SphereInfo spherRInfo;
	KdCollider::SphereInfo spherLInfo;

	spherRInfo.m_sphere.Center = m_Rpos;
	spherRInfo.m_sphere.Radius = 3;
	spherRInfo.m_type = KdCollider::Type::TypeDamage;

	spherLInfo.m_sphere.Center = m_Lpos;
	spherLInfo.m_sphere.Radius = 3;
	spherLInfo.m_type = KdCollider::Type::TypeDamage;

	std::shared_ptr<PlayerBase>spPlayerBase = m_wpPlayerBase.lock();
	if (spPlayerBase)
	{
		if (spPlayerBase->Intersects(spherRInfo, nullptr))
		{
			m_hitFlg = false;

			float dmage = m_attckPower * m_attckMagnification;
			spPlayerBase->OnAttackHit(dmage, m_knockbackDistance, m_knockbackDir, m_hitStunTime, false, 0);
		}

		if (spPlayerBase->Intersects(spherLInfo, nullptr))
		{
			m_hitFlg = false;

			float dmage = m_attckPower * m_attckMagnification;
			spPlayerBase->OnAttackHit(dmage, m_knockbackDistance, m_knockbackDir, m_hitStunTime, false, 0);
		}
	}

	if (m_pDebugWire)
	{
		m_pDebugWire->AddDebugSphere(spherRInfo.m_sphere.Center, spherRInfo.m_sphere.Radius);
		m_pDebugWire->AddDebugSphere(spherLInfo.m_sphere.Center, spherLInfo.m_sphere.Radius);
	}
}

void AttackJumpSlam::SetAttackStatus()
{
	//攻撃力倍率
	m_attckMagnification = 1.5;

	//ふっとばし距離
	m_knockbackDistance = 10;

	//スタン時間
	m_hitStunTime = 0;
}

void AttackJumpSlam::Load()
{

}
