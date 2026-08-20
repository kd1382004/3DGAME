#include "WarpGateBase.h"
#include"../Character/Player/PlayerBase.h"

void WarpGateBase::Init()
{
	if (!m_spWarpGate)
	{
		m_spWarpGate = std::make_shared<KdModelData>();
		m_spWarpGate->Load("Asset/Models/WarpGate/WarpGateblend.gltf");
	}



	if (!m_pCollider)
	{
		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("WarpGate", m_spWarpGate, KdCollider::TypeCameraOcclusion | KdCollider::TypeGround);
	}
}

void WarpGateBase::Update()
{
	PlayerHit();
}

void WarpGateBase::PreDraw()
{

}

void WarpGateBase::DrawLit()
{
	if (m_spWarpGate)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spWarpGate, m_mWorld);
	}
}

bool WarpGateBase::PlayerHit()
{
	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayer.lock();
	if (!spPlayer) { return false; }


	KdCollider::RayInfo rayInfo;
	rayInfo.m_pos = spPlayer->GetPos();
	rayInfo.m_pos.y += 1;
	rayInfo.m_dir = { 0,-1,0 };
	rayInfo.m_type = KdCollider::TypeGround;
	rayInfo.m_range = 3;

	if (Intersects(rayInfo, nullptr))
	{
		return true;
	}

	return false;
}
