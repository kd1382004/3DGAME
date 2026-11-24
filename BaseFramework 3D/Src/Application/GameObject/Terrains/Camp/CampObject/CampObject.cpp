#include"../CampLayoutData/CampLayoutData.h"
#include"CampObject.h"

void CampObject::SetData(const CampObjectData& _data)
{
	m_data = _data;

	if (!m_spModel) { m_spModel = std::make_shared<KdModelWork>(); }
	m_spModel->SetModelData(m_data.m_modelPath);

	ApplyData();
}

void CampObject::PlayAnime(const std::string& _name, bool _loop)
{
	if (!m_spModel || !m_spModel->GetData()) { return; }
	auto anime = m_spModel->GetData()->GetAnimation(_name);

	if (!anime) { return; }

	if (!m_spAnimator) { m_spAnimator = std::make_shared<KdAnimator>(); }


	m_spAnimator->SetAnimation(anime, _loop);
	m_spModel->CalcNodeMatrices();
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

void CampObject::Update()
{
	if (!m_spAnimator || !m_spModel) { return; }
	m_spAnimator->AdvanceTime(m_spModel->WorkNodes(), 100.0f);
	m_spModel->CalcNodeMatrices();
}

void CampObject::GenerateDepthMapFromLight()
{
	// 影を落とす(落としたくない物は描かない)
	if (!m_spModel) { return; }
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}
