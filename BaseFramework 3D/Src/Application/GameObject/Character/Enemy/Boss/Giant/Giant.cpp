#include "Giant.h"

void Giant::Init()
{
	m_charaStatusFilePath = "Asset/Data/ObjeData/Character/Enemy/Goblin/Status/Status.json";
	if (!m_spCharaModel)
	{
		m_spCharaModel = std::make_shared<KdModelWork>();
		m_spCharaModel->SetModelData("Asset/Models/Character/Enemy/Boss/Giant/Giantblend.gltf");

		m_spAnimetor = std::make_shared<KdAnimator>();
		m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation("idle"), true);

		m_angle = 0;
	}
}

void Giant::PreUpdate()
{
}

void Giant::Update()
{	
	EnemyAnimeModeUpdate();

	m_pos.y -= m_Gravity;
	m_Gravity += m_gravityPower;

	//座標行列を作る
	Math::Matrix tMat = Math::Matrix::CreateTranslation(m_pos);
	//回転行列
	Math::Matrix rMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angle));
	//行列の合成(S * R * T)
	m_mWorld = rMat * tMat;
}

void Giant::PostUpdate()
{
	CharacterBase::PostUpdate();
	EnemyAnimeModeUpdate();
	HPPosPostUpdate();
}
