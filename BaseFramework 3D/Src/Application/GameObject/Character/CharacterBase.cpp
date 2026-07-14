#include "CharacterBase.h"

void CharacterBase::Init()
{

}

void CharacterBase::Update()
{

}

void CharacterBase::PostUpdate()
{
	CollisionUpdate();
}

void CharacterBase::GenerateDepthMapFromLight()
{
	if (m_spCharaModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spCharaModel, m_mWorld);
	}
}

void CharacterBase::DrawLit()
{
	if (m_spCharaModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spCharaModel, m_mWorld);
	}
}

void CharacterBase::CollisionUpdate()
{
	//レイ判定設定
	/////////////////////////////////////////////
	// 当たり判定(レイ判定)用の情報作成
	KdCollider::RayInfo rayInfo;
	// レイの発射位置を設定
	rayInfo.m_pos = GetPos();
	// 少し高いところから飛ばす(段差の許容範囲)
	static float enableStepHigh = 0.2f;
	rayInfo.m_pos.y += enableStepHigh;

	// レイの発射方向を設定
	rayInfo.m_dir = Math::Vector3::Down;

	// レイの長さを設定
	rayInfo.m_range = m_Gravity + enableStepHigh;
	// 当たり判定タイプを設定
	rayInfo.m_type = KdCollider::TypeGround;
	/////////////////////////////////////////////

	// その他球による衝突判定
	/////////////////////////////////////////////
	// 当たり判定(球判定)用の情報作成
	DirectX::BoundingSphere sphere;
	sphere.Center = GetPos() + Math::Vector3(0, 0.5f, 0);
	sphere.Radius = 0.5f;
	KdCollider::SphereInfo spherInfo(KdCollider::TypeBump, sphere);
	/////////////////////////////////////////////

	for (std::weak_ptr<KdGameObject> wpGameObj : m_wpHitObjectList)
	{
		std::shared_ptr<KdGameObject> spGameObj = wpGameObj.lock();
		if (spGameObj)
		{

			//レイ判定
			//////////////////////////////////////////////////
			std::list<KdCollider::CollisionResult> retRayList;
			spGameObj->Intersects(rayInfo, &retRayList);

			float maxOverLap = 0;
			Math::Vector3 hitPos = {};
			bool hit = false;
			for (auto& ret : retRayList)
			{
				if (maxOverLap < ret.m_overlapDistance)
				{
					maxOverLap = ret.m_overlapDistance;
					hitPos = ret.m_hitPos;
					hit = true;
				}
			}

			if (hit)
			{
				// 地面に当たっている
				SetPos(hitPos);
				m_Gravity = 0;
			}
			//////////////////////////////////////////////////

			//その他球による衝突判定
			std::list<KdCollider::CollisionResult> retBumpList;
			spGameObj->Intersects(spherInfo, &retBumpList);

			for (auto& ret : retBumpList)
			{
				Math::Vector3 newPos = GetPos() + (ret.m_hitDir * ret.m_overlapDistance);
				SetPos(newPos);
			}
		}
	}
}

void CharacterBase::Release()
{
	m_spCharaModel = nullptr;
}
