#include "Slope.h"

void Slope::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Terrains/Map/Castle/Slope/Slope.gltf");
	}

	if (!m_pCollider)
	{
		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("Slope", m_spModel, KdCollider::TypeGround | KdCollider::TypeCameraOcclusion/*| KdCollider::TypeBump*/);
	}

	MapBase::Init();
}

void Slope::Update()
{

}

