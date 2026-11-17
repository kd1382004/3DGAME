#include "AttackJumpSlam.h"

#include"../../../../../Player/PlayerBase.h"
void AttackJumpSlam::AttackJumpSlamUpdate()
{
	//攻撃範囲表示
	if (m_radiusDrawFlg)
	{

		UAEffectShaderManager::Instance().WriteCBCircleEffect(m_rPos, m_radius, { 0.5,0,0 });
		m_radiusPercent += 2 * DeltaTime::Instance().GetGameDeltaTime();
		if (m_radiusPercent > 1)
		{
			m_radiusPercent = 0;
		}

		UAEffectShaderManager::Instance().WriteCBCircleEffect(m_rPos, m_radius * m_radiusPercent, { 1,0,0 });
	}



	if (!m_hitFlg) { return; }

	KdCollider::SphereInfo sphereRInfo;

	sphereRInfo.m_sphere.Center = m_rPos;
	sphereRInfo.m_sphere.Radius = m_radius;
	sphereRInfo.m_type = KdCollider::Type::TypeDamage;
	std::shared_ptr<PlayerBase>spPlayerBase = m_wpPlayerBase.lock();
	if (spPlayerBase)
	{
		if (spPlayerBase->Intersects(sphereRInfo, nullptr))
		{
			m_hitFlg = false;

			float damage = m_attackPower * m_attackMagnification;
			spPlayerBase->OnAttackHit(damage, m_knockbackDistance, m_knockbackDir, m_hitStunTime, false, 0);
		}
	}

	if (m_pDebugWire)
	{
		m_pDebugWire->AddDebugSphere(sphereRInfo.m_sphere.Center, sphereRInfo.m_sphere.Radius);
	}



}

void AttackJumpSlam::SetAttackStatus()
{
	//攻撃力倍率
	m_attackMagnification = 1.5;

	//ふっとばし距離
	m_knockbackDistance = 10;

	//スタン時間
	m_hitStunTime = 0;
}

void AttackJumpSlam::Load()
{

}
