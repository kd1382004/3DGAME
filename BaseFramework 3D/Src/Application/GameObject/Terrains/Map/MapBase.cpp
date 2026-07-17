#include "MapBase.h"

void MapBase::Init()
{
	m_spModel = std::make_shared<KdModelWork>();
	m_spModel->SetModelData("Asset/Models/Terrains/Map/Base/Base.gltf");

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("Ground", m_spModel, KdCollider::TypeGround);
}

void MapBase::Update()
{}

void MapBase::DrawLit()
{
	if (!m_spModel) { return; }

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}

void MapBase::GenerateDepthMapFromLight()
{
	if (!m_spModel) { return; }

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}
