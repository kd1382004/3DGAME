#include "FloorBase.h"


void FloorBase::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Terrains/Map/Floor/Base.gltf");


		if (!m_pCollider)
		{
			m_pCollider = std::make_unique<KdCollider>();
			m_pCollider->RegisterCollisionShape("Ground", m_spModel, KdCollider::TypeGround);
		}
	}
}

void FloorBase::Update()
{
}
