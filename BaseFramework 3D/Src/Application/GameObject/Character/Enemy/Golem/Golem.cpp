#include "Golem.h"
#include"../../../Camera/CameraBase.h"

//攻撃
#include"Attack/SmashAttack/SmashAttack.h"

#include"../../../../Scene/SceneManager.h"
void Golem::Init()
{
	EnemyAmbush::Init();
	wanderRadius = 50;
	m_charaStatusFilePath = "Asset/Data/ObjeData/Character/Enemy/Golem/Status/Status.json";
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

		m_viewAngle = 100;
	}
}

void Golem::Update()
{

	if (m_isDead)
	{
		if (!m_spAnimetor) { return; }

		// 死亡時に出しっぱなしの攻撃判定を必ず消す
		if (auto spAttack = m_wpAttack.lock())
		{
			spAttack->FinishAttack_Slamming();
		}

		if (m_spAnimetor->IsAnimationEnd())
		{
			m_isExpired = true;
		}
	}

	EnemyAmbush::Update();

	Attack();

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
			Math::Vector3 dir = m_mWorld.Backward();
			dir.Normalize();
			Math::Vector3 pos = GetPos();
			Math::Vector3 eyePos = pos + Math::Vector3(0, 1.0f, 0) + dir * 2;

			// 壁などの遮蔽物を考慮した照射距離を算出
			float lightDist = m_viewDistance;

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
		m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_golemAnimeName.DashAnime), true);
		break;
	case EnemyBase::EnemyAnimeMode_Attck:
		m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_golemAnimeName.smashAnime), false);
		m_attackFlg = true;
		break;
	case EnemyBase::EnemyAnimeMode_Die:
		m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_golemAnimeName.DieAnime), false);
		break;
	default:
		break;
	}

	m_AnimeChangeFlg = false;
}

void Golem::Attack()
{
	if (m_isDead) { return; }

	//もし見失ってたら何もしない
	if (!m_playerChaseFlg)
	{
		m_attackFlg = false;

		//中身あったら消す
		std::shared_ptr<AttackBase>spAttack = m_wpAttack.lock();
		if (spAttack)
		{
			spAttack->FinishAttack_Slamming();
			m_hitTriggered = false;
		}

		return;
	}




	if (m_attackFlg)
	{
		std::shared_ptr<AttackBase>spAttack = m_wpAttack.lock();

		if (m_spAnimetor)
		{
			//アニメーションが終わったら攻撃終了
			if (m_spAnimetor->IsAnimationEnd())
			{
				m_attackFlg = false;
				m_attackWait = m_attackWaitMax;

				m_AnimeChangeFlg = true;
				m_enemyAnimeMode = EnemyAnimeMode::EnemyAnimeMode_Run;
				m_hitTriggered = false;

				if (spAttack)
				{
					spAttack->FinishAttack_Slamming();
				}
			}
			else
			{
				if (spAttack && !m_hitTriggered)
				{
					spAttack->SetAngleY(m_angle);
				}

				float progress = m_spAnimetor->GetTime() / m_spAnimetor->GetMaxLength();

				if (progress > m_smasAttackAnimeHitTriggerTime && !m_hitTriggered)
				{

					if (spAttack)
					{
						m_hitTriggered = true;
						spAttack->SetAttckFlg(true);
						spAttack->SetAttckPower(NowAttack());
					}
				}

			}
		}

	

	}
	else
	{
		//プレイヤーとの距離が近かったら攻撃開始
		if ((m_pos - m_playerPos).Length() < 5)
		{
			if (!m_attackFlg)
			{
				if (m_attackWait > 0)
				{
					m_attackWait -= DeltaTime::Instance().GetGameDeltaTime();
				}
				else
				{
					m_enemyAnimeMode = EnemyBase::EnemyAnimeMode_Attck;
					m_AnimeChangeFlg = true;

					std::shared_ptr<SmashAttack>spAttack = std::make_shared<SmashAttack>();
					m_wpAttack = spAttack;
					spAttack->SetPos(m_pos);
					spAttack->SetPlayer(m_wpPlayer.lock());
					SceneManager::Instance().AddObject(spAttack);

				}
			}
		}

	}






	SetAttackGagePercent(m_attackWait / m_attackWaitMax);
}

