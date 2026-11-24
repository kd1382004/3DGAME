//---- 自分のヘッダー ----
#include "AttackShape.h"				//敵の攻撃判定の形

//---- プレイヤー ----
#include"../../Player/PlayerBase.h"		//プレイヤー

//=============================================
// 基本の処理
//=============================================

void AttackShape::Update()
{
	//予告(円・箱)
	if (m_warnFlg)
	{
		DrawWarn();
	}

	if (!m_hitFlg) { return; }

	std::shared_ptr<PlayerBase> spPlayer = m_wpPlayer.lock();
	if (!spPlayer) { return; }

	bool hit = false;

	if (m_shape == AttackHitShape::Box)
	{
		// OBB(回転する箱)。AttackLeftPunch と同じ
		DirectX::BoundingOrientedBox obb;
		obb.Center = m_pos;
		obb.Extents = m_boxSize;
		obb.Orientation = Math::Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(m_angleY), 0.0f, 0.0f);

		KdCollider::BoxInfo boxInfo(KdCollider::Type::TypeDamage, obb);
		hit = spPlayer->Intersects(boxInfo, nullptr);

		if (m_pDebugWire)
		{
			Math::Matrix boxMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angleY)) * Math::Matrix::CreateTranslation(m_pos);
			m_pDebugWire->AddDebugBox(boxMat, m_boxSize, Math::Vector3::Zero, true, kRedColor);
		}
	}
	else
	{
		// 球(Bone・Circle)。AttackJumpSlam と同じ
		KdCollider::SphereInfo sphereInfo;
		sphereInfo.m_sphere.Center = m_pos;
		sphereInfo.m_sphere.Radius = m_radius;
		sphereInfo.m_type = KdCollider::Type::TypeDamage;
		hit = spPlayer->Intersects(sphereInfo, nullptr);

		if (m_pDebugWire)
		{
			m_pDebugWire->AddDebugSphere(m_pos, m_radius);
		}
	}

	if (hit)
	{
		//1回だけ当てる(段ごとに作り直すので、連撃は段ごとに当たる)
		m_hitFlg = false;
		spPlayer->OnAttackHit(m_attackPower, m_knockbackDistance, m_knockbackDir, m_hitStunTime, false, 0);
	}
}

//=============================================
// その他の処理
//=============================================

void AttackShape::DrawWarn()
{
	if (m_shape == AttackHitShape::Bone) { return; }

	//外側の線と、広がっていく内側を出す
	m_warnPercent += 2 * DeltaTime::Instance().GetGameDeltaTime();
	if (m_warnPercent > 1)
	{
		m_warnPercent = 0;
	}

	if (m_shape == AttackHitShape::Circle)
	{
		UAEffectShaderManager::Instance().WriteCBCircleEffect(m_pos, m_radius, { 0.5,0,0 });
		UAEffectShaderManager::Instance().WriteCBCircleEffect(m_pos, m_radius * m_warnPercent, { 1,0,0 });
	}
	else if (m_shape == AttackHitShape::Box)
	{
		const float rad = DirectX::XMConvertToRadians(m_angleY);
		UAEffectShaderManager::Instance().WriteCBBoxEffect(m_pos, m_boxSize, rad, { 0.5,0,0 });
		UAEffectShaderManager::Instance().WriteCBBoxEffect(m_pos, m_boxSize * m_warnPercent, rad, { 0.5,0,0 });
	}
}
