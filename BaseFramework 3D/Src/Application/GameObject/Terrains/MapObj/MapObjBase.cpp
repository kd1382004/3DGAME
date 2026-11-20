#include "MapObjBase.h"
#include"../../../Scene/SceneManager.h"
void MapObjBase::DrawModelWithFade(KdModelWork& _model, bool _isEdge)
{

	if (m_fader.IsHidden()) { return; }

	auto& shader = KdShaderManager::Instance().m_StandardShader;

	if (_isEdge)
	{
		float         edgeRange = 0.04f;
		Math::Vector3 edgeColor = { SceneManager::Instance().GetFogCol()};
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

