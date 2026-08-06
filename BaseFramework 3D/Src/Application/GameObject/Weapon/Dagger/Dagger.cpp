#include "Dagger.h"

void Dagger::Init()
{
	m_WeaponStatusFilePath = "Asset/Data/ObjeData/Weapon/Dagger/BaseWeaponStatus.json";
	WeaponBase::Init();

	if (!m_spWeaponModel)
	{
		m_spWeaponModel = std::make_shared<KdModelWork>();
		m_spWeaponModel->SetModelData("Asset/Models/Weapon/Dagger/Rogue_Dagger.gltf");
	}

	if (!m_pDebugWire)
	{
		m_pDebugWire = std::make_unique<KdDebugWireFrame>();
	}

	m_localPos = Math::Vector3(0.0f, 1.2f, 0.0f);
}

void Dagger::Update()
{
	WeaponBase::Update();

	if (!m_pDebugWire) { return; }

	m_pDebugWire->AddDebugSphere(GetPos(), 0.1f, Math::Color(1, 0, 0, 1));

	//武器の先端を取る
	Math::Matrix finalMat = m_weponParentMat;
	Math::Vector3 worldPos = Math::Vector3::Transform(tipLocalPos, finalMat);

	// デバッグ表示
	m_pDebugWire->AddDebugSphere(worldPos, 0.1f, Math::Color(1, 0, 0, 1));
}

void Dagger::DrawLit()
{
	if (!m_spWeaponModel) { return; }
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spWeaponModel, m_mWorld);

}
