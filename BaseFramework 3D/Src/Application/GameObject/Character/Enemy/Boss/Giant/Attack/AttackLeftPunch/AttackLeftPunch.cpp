#include "AttackLeftPunch.h"
#include"../../../../../Player/PlayerBase.h"
void AttackLeftPunch::AttackLeftPunchUpdate()
{

	UAEffectShaderManager::Instance().WriteCBBoxEffect(m_targetCenterPos, m_boxSiz, DirectX::XMConvertToRadians(m_angleY), { 0.5,0,0 });
	m_boxPercent += 2 * DeltaTime::Instance().GetGameDeltaTime();
	if (m_boxPercent > 1)
	{
		m_boxPercent = 0;
	}

	UAEffectShaderManager::Instance().WriteCBBoxEffect(m_targetCenterPos, m_boxSiz* m_boxPercent, DirectX::XMConvertToRadians(m_angleY), { 0.5,0,0 });

	if (!m_hitFlg) { return; }

	// OBB（回転対応ボックス）の作成
	DirectX::BoundingOrientedBox obb;
	obb.Center = m_targetCenterPos;
	obb.Extents = m_boxSiz;
	obb.Orientation = Math::Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_angleY), 0.0f, 0.0f);

	KdCollider::BoxInfo boxInfo(KdCollider::Type::TypeDamage, obb);

	std::shared_ptr<PlayerBase>spPlayerBase = m_wpPlayerBase.lock();
	if (spPlayerBase)
	{
		if (spPlayerBase->Intersects(boxInfo, nullptr))
		{
			m_hitFlg = false;

			float dmage = m_attckPower * m_attckMagnification;
			spPlayerBase->OnAttackHit(dmage, m_knockbackDistance, m_knockbackDir, m_hitStunTime, false, 0);
		}
	}

	if (m_pDebugWire)
	{
		Math::Matrix boxMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angleY)) * Math::Matrix::CreateTranslation(m_targetCenterPos);
		m_pDebugWire->AddDebugBox(boxMat, m_boxSiz, Math::Vector3::Zero, true,kRedColor);
	}
}

void AttackLeftPunch::DrawLit()
{

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
