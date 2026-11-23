#include"../CampLayoutData/CampLayoutData.h"
#include"CampObject.h"

void CampObject::SetData(const CampObjectData& _data)
{
	m_data = _data;

	if (!m_spModel) { m_spModel = std::make_shared<KdModelWork>(); }
	m_spModel->SetModelData(m_data.m_modelPath);

	ApplyData();
}

void CampObject::DrawLit()
{
	if (!m_spModel) { return; }
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}

void CampObject::Init()
{
	// 背景なのでカメラの遮蔽判定などは無し。モデルは SetData で読む
}

void CampObject::GenerateDepthMapFromLight()
{
	// 影を落とす(落としたくない物は描かない)
	if (!m_spModel) { return; }
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}
