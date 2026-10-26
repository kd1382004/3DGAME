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

	// スポットライトの位置（懐中電灯モデル自身の影で前方が遮られないよう、わずかに前方に配置）
	Math::Vector3 lightPos = pos + dir * 0.15f;

	// スポットライトの View 行列を作成 (左手系LHを明示的に使用)
	Math::Vector3 up = Math::Vector3::Up;
	if (fabsf(dir.Dot(up)) > 0.99f) { up = Math::Vector3::Forward; }
	Math::Matrix mView = DirectX::XMMatrixLookAtLH(lightPos, lightPos + dir, up);

	// 視野角（円錐全体が四角錐のシャドウマップにすっぽり収まるよう、少し広めの 1.3 倍にする）
	float fov = DirectX::XMConvertToRadians(m_flashlightconfig.angle * 2.0f * 1.3f);
	fov = std::clamp(fov, 0.01f, DirectX::XM_PI - 0.05f);

	// Perspective 射影行列を作成 (近クリップを0.1fにして深度分解能を向上)
	Math::Matrix mProj = DirectX::XMMatrixPerspectiveFovLH(
		fov,
		1.0f,                                     // アスペクト比 1.0 (正方形テクスチャ)
		0.1f,                                     // 近クリップ
		m_flashlightconfig.radius                 // 遠クリップ (照射距離)
	);

	Math::Matrix mSpotLightVP = mView * mProj;

	// シャドウマップ用に行列を登録
	KdShaderManager::Instance().WorkAmbientController().SetSpotLightShadow(mSpotLightVP, true);

	// スポットライトを追加 (色, 照射距離, 位置, 方向, 照射角度, 明度加算)
	KdShaderManager::Instance().WorkAmbientController().AddSpotLight(
		m_flashlightconfig.color,			// 光の色・強度
		m_flashlightconfig.radius,			// 照射距離
		lightPos,                           // スポットライトの位置
		dir,                                // 前方方向
		m_flashlightconfig.angle,           // 照射角 
		true
	);
}

void Flashlight::GenerateDepthMapFromLight()
{
	// 懐中電灯自身のメッシュはシャドウマップに落とさない（前方を自身の影で遮るのを防ぐ）
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