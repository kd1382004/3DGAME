#include "Golem.h"
#include"../../../Camera/CameraBase.h"
void Golem::Init()
{
	EnemyAmbush::Init();
	wanderRadius = 40;
	m_charaStatusFilePath = "Asset/Data/ObjeData/Character/Enemy/Goblin/Status/Status.json";
	if (!m_spCharaModel)
	{
		m_spCharaModel = std::make_shared<KdModelWork>();
		m_spCharaModel->SetModelData("Asset/Models/Character/Enemy/Golem/golem_clean.gltf");
		//アニメータの準備
		m_spAnimetor = std::make_shared<KdAnimator>();
		m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_golemAnimeName.WalkAnime), true);



		if (!m_pCollider)
		{
			m_pCollider = std::make_unique<KdCollider>();
			m_pCollider->RegisterCollisionShape("Golem", m_spCharaModel, KdCollider::TypeBump | KdCollider::TypeCameraOcclusion | KdCollider::TypeDamage);
		}

		EnemyBase::Init();

		m_viewAngle = 150;
	}
}

void Golem::Update()
{
	EnemyAmbush::Update();
	
	if (m_AnimeChangeFlg)
	{
		ChangeAnime();
	}
}

void Golem::PreUpdate()
{
	EnemyBase::PreUpdate();


	HPPosPostUpdate();

	if (!m_playerChaseFlg)
	{
		std::shared_ptr<CameraBase>spCamera = m_wpCamera.lock();
		if (!spCamera) { return; }


		if (CheckInScreen(spCamera->GetBoundingFrustum(), m_pos, m_viewDistance))
		{

			// 視界範囲描画
			Math::Vector3 pos = GetPos();
			Math::Vector3 eyePos = pos + Math::Vector3(0, 1.0f, 0);
			Math::Vector3 dir = m_mWorld.Backward();
			dir.Normalize();

			// 壁などの遮蔽物を考慮した照射距離を算出
			float lightDist = CalcSightLightDistance(eyePos, dir, m_viewDistance, m_viewAngle * 0.5f);

			// スポットライトを追加 (色, 照射距離, 位置, 方向, 照射角度)
			KdShaderManager::Instance().WorkAmbientController().AddSpotLight(
				{ 0.5f, 0.0f, 0.0f },			// 光の色・強度
				lightDist,						// 遮蔽された照射距離
				m_pos,							// 位置
				dir,							// 前方方向
				m_viewAngle * 0.5f,				// 照射角 
				false
			);
		}
	}
}

void Golem::Release()
{

}

void Golem::ChangeAnime()
{
	if (!m_spAnimetor) { return; }


	if (!m_isDead)
	{
		//攻撃中は変更しない
		if (m_attackFlg) { return; }
	}


	switch (m_enemyAnimeMode)
	{
	case EnemyBase::EnemyAnimeMode_Idel:
		m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_golemAnimeName.IdleAnime), true);
		break;
	case EnemyBase::EnemyAnimeMode_Walk:
		m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_golemAnimeName.WalkAnime), true);
		break;
	case EnemyBase::EnemyAnimeMode_Run:
		m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_golemAnimeName.WalkAnime), true);
		break;
	case EnemyBase::EnemyAnimeMode_Attck:
		m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_golemAnimeName.smashAnime), false);
		m_attackFlg = true;
		break;
	case EnemyBase::EnemyAnimeMode_Die:
		break;
	default:
		break;
	}

	m_AnimeChangeFlg = false;
}
