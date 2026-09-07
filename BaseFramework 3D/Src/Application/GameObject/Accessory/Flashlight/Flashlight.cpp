#include "Flashlight.h"

void Flashlight::Init()
{
	if (!m_spFlashlightModel)
	{
		m_spFlashlightModel = std::make_shared<KdModelWork>();
		m_spFlashlightModel->SetModelData("Asset/Models/Accessory/Flashlight/torch.gltf");
	}


	m_localMat = Math::Matrix::CreateTranslation({0.3, 0.1, 0});
}

void Flashlight::Update()
{
}

void Flashlight::PostUpdate()
{
	m_mWorld = m_localMat * m_accessoryParentMat;

	Math::Vector3 pos = m_mWorld.Translation();
	Math::Vector3 dir = m_mWorld.Backward(); // 前方ベクトル
	dir.Normalize();

	// スポットライトを追加 (色, 照射距離, 位置, 方向, 照射角度)
	KdShaderManager::Instance().WorkAmbientController().AddSpotLight(
		m_flashlightconfig.color,			// 光の色・強度
		m_flashlightconfig.radius,			// 照射距離
		pos,                                // 懐中電灯の位置
		dir,                                // 前方方向
		m_flashlightconfig.angle,           // 照射角 
		true
	);
}

void Flashlight::GenerateDepthMapFromLight()
{
	if (!m_spFlashlightModel) { return; }
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spFlashlightModel, m_mWorld);
}

void Flashlight::DrawLit()
{
	if (!m_spFlashlightModel) { return; }
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spFlashlightModel, m_mWorld);
}

void Flashlight::DrawBright()
{
	if (!m_spFlashlightModel) { return; }
	//KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spFlashlightModel, m_mWorld);
}

void Flashlight::DrawImGui()
{
	if (ImGui::CollapsingHeader("Flashlight"))
	{
		ImGui::ColorEdit3("Color", &m_flashlightconfig.color.x);

		ImGui::SliderFloat("Radius", &m_flashlightconfig.radius, 10.0f, 500.0f);

		ImGui::SliderFloat("Angle", &m_flashlightconfig.angle, 5.0f, 90.0f);
	}
}