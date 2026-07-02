#include "Ground.h"

void Ground::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Terrains/Ground/Terrain.gltf");

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("Ground", m_spModel, KdCollider::TypeGround);

		m_gameObjectClass = KdGameObject::GameObjectClass::GameObjectClass_Terrains;

		m_ImGUIName = "Ground";
	}
}

void Ground::Update()
{

}

void Ground::DrawLit()
{
	if (!m_spModel) { return; }

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel,m_mWorld);
}

void Ground::GenerateDepthMapFromLight()
{
	if (!m_spModel) { return; }

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}
