#include "WallBase.h"

void WallBase::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Terrains/Map/Wall/Base.gltf");


		if (!m_pCollider)
		{
			m_pCollider = std::make_unique<KdCollider>();
			m_pCollider->RegisterCollisionShape("Wall", m_spModel, KdCollider::TypeBump);
		}
	}
}

void WallBase::Update()
{

}
