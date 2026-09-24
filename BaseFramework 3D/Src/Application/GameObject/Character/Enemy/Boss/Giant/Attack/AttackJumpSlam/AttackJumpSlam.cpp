#include "AttackJumpSlam.h"

#include"../../../../../Player/PlayerBase.h"
void AttackJumpSlam::AttackJumpSlamUpdate()
{
	//攻撃範囲表示
	UAEffectShaderManager::Instance().WriteCBColoer(m_Rpos, m_radius, { 0.5,0,0 });
	m_radiusPercent += 2* DeltaTime::Instance().GetGameDeltaTime();
	if (m_radiusPercent > 1)
	{
		m_radiusPercent = 0;
	}

	UAEffectShaderManager::Instance().WriteCBColoer(m_Rpos, m_radius * m_radiusPercent, { 1,0,0 });


	if (!m_hitFlg) { return; }

	KdCollider::SphereInfo spherRInfo;

	spherRInfo.m_sphere.Center = m_Rpos;
	spherRInfo.m_sphere.Radius = m_radius;
	spherRInfo.m_type = KdCollider::Type::TypeDamage;
	std::shared_ptr<PlayerBase>spPlayerBase = m_wpPlayerBase.lock();
	if (spPlayerBase)
	{
		if (spPlayerBase->Intersects(spherRInfo, nullptr))
		{
			m_hitFlg = false;

			float dmage = m_attckPower * m_attckMagnification;
			spPlayerBase->OnAttackHit(dmage, m_knockbackDistance, m_knockbackDir, m_hitStunTime, false, 0);
		}
	}

	if (m_pDebugWire)
	{
		m_pDebugWire->AddDebugSphere(spherRInfo.m_sphere.Center, spherRInfo.m_sphere.Radius);
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
