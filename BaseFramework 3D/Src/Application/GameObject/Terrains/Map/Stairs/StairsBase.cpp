#include "StairsBase.h"
#include"../../../Character/Player/PlayerBase.h"


void StairsBase::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Terrains/Map/Stairs/Stairs.gltf");


		if (!m_pCollider)
		{
			m_pCollider = std::make_unique<KdCollider>();
			m_pCollider->RegisterCollisionShape("Stairs", m_spModel, KdCollider::TypeBump | KdCollider::TypeCameraOcclusion | KdCollider::TypeGround);
		}

		MapBase::Init();
	}
}

void StairsBase::Update()
{
	Math::Vector3 noePos = GetPos();

	float lest = noePos.x - 2.5;
	float right = noePos.x + 2.5;
	float top = noePos.z + 2.5;
	float bot = noePos.z - 2.5;

	


	std::shared_ptr<PlayerBase>spPalyerBase = m_wpPlayerBase.lock();

	if (spPalyerBase)
	{
		Math::Vector3 playerPos = spPalyerBase->GetPos();

		KdCollider::RayInfo rayInfo;
		rayInfo.m_pos = playerPos;
		rayInfo.m_dir = Math::Vector3::Down;
		rayInfo.m_range = 1;
		rayInfo.m_type = KdCollider::TypeGround;

		if (Intersects(rayInfo, nullptr))
		{
			m_playerHit = true;
		}
		else
		{
			m_playerHit = false;
		}


		spPalyerBase->SetNextFloorActionFlg(m_playerHit);

	}

	auto r = m_mWorld.Right();
	auto u = m_mWorld.Up();
	auto b = m_mWorld.Backward();

	
	KdDebugGUI::Instance().ClearLog();
	KdDebugGUI::Instance().AddLog("Right: %f %f %f\n", r.x, r.y, r.z);
	KdDebugGUI::Instance().AddLog("Up: %f %f %f\n", u.x, u.y, u.z);
	KdDebugGUI::Instance().AddLog("Back: %f %f %f\n", b.x, b.y, b.z);

}

void StairsBase::IsHit()
{

}
