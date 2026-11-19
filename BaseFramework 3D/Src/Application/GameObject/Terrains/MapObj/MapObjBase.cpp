#include "MapObjBase.h"

void MapObjBase::DrawModelWithFade(KdModelWork& _model, bool _isEdge)
{

	if (m_fader.IsHidden()) { return; }

	auto& shader = KdShaderManager::Instance().m_StandardShader;

	if (_isEdge)
	{
		float         edgeRange = 0.04f;
		Math::Vector3 edgeColor = { 0.25f, 0.35f, 0.4f };
		shader.SetDissolve(m_fader.GetDissolve(), &edgeRange, &edgeColor);
	}
	else
	{
		shader.SetDissolve(m_fader.GetDissolve());
	}

	shader.DrawModel(_model, m_mWorld);

	// 後片付け
	shader.SetDissolve(0.0f);
}

