#include "Enemy_Goblin.h"
#include"Attack/Attack_Slamming/Attack_Slamming.h"

#include"../../../../Scene/SceneManager.h"

#include"../../../Camera/CameraBase.h"

void Goblin::Init()
{
	EnemyAmbush::Init();
	m_wanderRadius = 20;
	m_charaDefaultStatusFilePath = "Asset/Data/ObjeData/Character/Enemy/Goblin/Status/Status.json";
	if (!m_spCharaModel)
	{
		m_spCharaModel = std::make_shared<KdModelWork>();
		m_spCharaModel->SetModelData("Asset/Models/Character/Enemy/Goblin/goblin-FINAL.gltf");
		//アニメータの準備
		m_spAnimator = std::make_shared<KdAnimator>();
		m_spAnimator->SetAnimation(m_spCharaModel->GetAnimation(m_goblinAnimeName.m_idleAnime), true);



		if (!m_pCollider)
		{
			m_pCollider = std::make_unique<KdCollider>();
			m_pCollider->RegisterCollisionShape("Goblin", m_spCharaModel, KdCollider::TypeBump | KdCollider::TypeCameraOcclusion | KdCollider::TypeDamage);
		}

		//if (!m_pDebugWire)
		//{
		//	m_pDebugWire = std::make_unique<KdDebugWireFrame>();
		//}

		EnemyBase::Init();

		m_viewAngle = 90;



		if (!m_spWalk3DSE)
		{
			m_spWalk3DSE = KdAudioManager::Instance().Play3D("Asset/Sound/SE/Enemy/Goblin/Walk.wav", m_pos, false);
			if (m_spWalk3DSE)
			{
				m_spWalk3DSE->Stop();
				m_spWalk3DSE->SetCurveDistanceScaler(20.0f);
			}
		}
		if (!m_spDash3DSE)
		{
			// 走る音（ゴブリンの足音のピッチを上げて走る音にする）
			m_spDash3DSE = KdAudioManager::Instance().Play3D("Asset/Sound/SE/Enemy/Goblin/Walk.wav", m_pos, false);
			if (m_spDash3DSE)
			{
				m_spDash3DSE->Stop();
				m_spDash3DSE->SetPitch(0.4f);
				m_spDash3DSE->SetCurveDistanceScaler(20.0f);
			}
		}
		if (!m_spCrying3DSE)
		{
			// 攻撃時の叫び声SE
			m_spCrying3DSE = KdAudioManager::Instance().Play3D("Asset/Sound/SE/Enemy/Goblin/CryingSound.wav", m_pos, false);
			if (m_spCrying3DSE)
			{
				m_spCrying3DSE->Stop();
				m_spCrying3DSE->SetCurveDistanceScaler(20.0f);
			}
		}
		if (!m_spSlam3DSE)
		{
			// 叩きつけの音
			m_spSlam3DSE = KdAudioManager::Instance().Play3D("Asset/Sound/SE/Enemy/Goblin/Slam.wav", m_pos, false);
			if (m_spSlam3DSE)
			{
				m_spSlam3DSE->Stop();
				m_spSlam3DSE->SetCurveDistanceScaler(20.0f);
			}
		}
	}
	m_playerChaseDir = 2;
	m_playerChaseResumeDir = 2.5f;
	m_material = EnemyMaterial::Flesh;
}

void Goblin::Update()
{


	if (m_isDead)
	{
		if (!m_spAnimator) { return; }

		// 死亡時に出しっぱなしの攻撃判定を必ず消す
		if (auto spAttack = m_wpAttack.lock())
		{
			spAttack->FinishAttack_Slamming();
		}

		if (m_spAnimator->IsAnimationEnd())
		{
			m_isExpired = true;
		}
	}


	if (m_playerChaseFlg && !m_attackFlg)
	{
		m_moveVec = m_playerPos - m_pos;
		m_moveVec.y = 0;
		m_moveVec.Normalize();
	}

	EnemyAmbush::Update();

	Attack();

	if (m_animeChangeFlg)
	{
		ChangeAnime();
	}

	UpdateFootstepSE();
}

void Goblin::PreUpdate()
{
	EnemyBase::PreUpdate();
	HPPosPostUpdate();

	if (!m_playerChaseFlg && m_isVisionFlg)
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

void Goblin::Release()
{
	EnemyBase::Release();

	//中身あったら消す
	std::shared_ptr<AttackBase>spAttack = m_wpAttack.lock();
	if (spAttack)
	{
		spAttack->FinishAttack_Slamming();
		m_hitTriggered = false;
	}

	if (m_spWalk3DSE)
	{
		m_spWalk3DSE->Stop();
		m_spWalk3DSE = nullptr;
	}
	if (m_spDash3DSE)
	{
		m_spDash3DSE->Stop();
		m_spDash3DSE = nullptr;
	}
	if (m_spCrying3DSE)
	{
		m_spCrying3DSE->Stop();
		m_spCrying3DSE = nullptr;
	}
	if (m_spSlam3DSE)
	{
		m_spSlam3DSE->Stop();
		m_spSlam3DSE = nullptr;
	}
}

void Goblin::UpdateFootstepSE()
{
	if (m_isDead || m_attackFlg)
	{
		if (m_spWalk3DSE) { m_spWalk3DSE->Stop(); }
		if (m_spDash3DSE) { m_spDash3DSE->Stop(); }
		if (m_isDead && m_spCrying3DSE) { m_spCrying3DSE->Stop(); }
		return;
	}

	float seVol = VOLInfo::Instance().GetMSSEVol();

	// プレイヤーを発見して走っている時
	if (m_enemyAnimeMode == EnemyAnimeMode::EnemyAnimeMode_Run)
	{
		if (m_spWalk3DSE)
		{
			m_spWalk3DSE->Stop();
		}

		if (m_spDash3DSE)
		{
			m_spDash3DSE->SetPos(m_pos);
			m_spDash3DSE->SetVolume(seVol * 0.3);
			if (!m_spDash3DSE->IsPlaying())
			{
				m_spDash3DSE->Play();
			}
		}
	}
	// 歩いている時
	else if (m_enemyAnimeMode == EnemyAnimeMode::EnemyAnimeMode_Walk)
	{
		if (m_spDash3DSE)
		{
			m_spDash3DSE->Stop();
		}

		if (m_spWalk3DSE)
		{
			m_spWalk3DSE->SetPos(m_pos);
			m_spWalk3DSE->SetVolume(seVol * 0.3);
			if (!m_spWalk3DSE->IsPlaying())
			{
				m_spWalk3DSE->Play();
			}
		}
	}
	// 待機中など停止時
	else
	{
		if (m_spWalk3DSE)
		{
			m_spWalk3DSE->Stop();
		}
		if (m_spDash3DSE)
		{
			m_spDash3DSE->Stop();
		}
	}
}

void Goblin::ChangeAnime()
{
	if (!m_spAnimator) { return; }


	if (!m_isDead)
	{
		//攻撃中は変更しない
		if (m_attackFlg) { return; }
	}


	switch (m_enemyAnimeMode)
	{
	case EnemyBase::EnemyAnimeMode_Idle:
		m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_goblinAnimeName.m_idleAnime), 0.2f, true);
		break;
	case EnemyBase::EnemyAnimeMode_Walk:
		m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_goblinAnimeName.m_walkAnime), 0.2f, true);
		break;
	case EnemyBase::EnemyAnimeMode_Run:
		m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_goblinAnimeName.m_runAnime), 0.2f, true);
		break;
	case EnemyBase::EnemyAnimeMode_Attack:
		m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_goblinAnimeName.m_slammingDownAttackAnime), 0.1f, false);
		m_attackFlg = true;
		if (m_spCrying3DSE)
		{
			m_spCrying3DSE->SetPos(m_pos);
			m_spCrying3DSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
			m_spCrying3DSE->Play(false);
		}
		break;
	case EnemyBase::EnemyAnimeMode_Die:
		m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_goblinAnimeName.m_dieAnime), 0.2f, false);
		break;
	default:
		break;
	}

	m_animeChangeFlg = false;
}

void Goblin::Attack()
{
	if (m_isDead) { return; }

	//もし見失ってたら何もしない
	if (!m_playerChaseFlg)
	{
		//中身あったら消す
		std::shared_ptr<AttackBase>spAttack = m_wpAttack.lock();
		if (spAttack)
		{
			spAttack->FinishAttack_Slamming();
			m_hitTriggered = false;
		}


		m_attackFlg = false;
		return;
	}



	std::shared_ptr<AttackBase>spAttack = m_wpAttack.lock();
	if (spAttack)
	{
		if (m_attackFlg)
		{
			//腕の座標を攻撃に渡す
			auto* handNode = m_spCharaModel->FindNode("thumb_2_L");
			if (handNode)
			{
				Math::Matrix handMat = handNode->m_worldTransform * m_mWorld;
				spAttack->SetPos(handMat.Translation());
			}
			spAttack->SetKnockbackDir(m_mWorld.Backward());

			if (m_spCrying3DSE && m_spCrying3DSE->IsPlaying())
			{
				m_spCrying3DSE->SetPos(m_pos);
			}

			if (m_spAnimator)
			{
				//アニメーションが終わったら攻撃終了
				if (m_spAnimator->IsAnimationEnd())
				{
					m_attackFlg = false;
					if (spAttack)
					{
						spAttack->FinishAttack_Slamming();
						m_hitTriggered = false;
					}

					m_attackWait = m_attackWaitMax;

					m_animeChangeFlg = true;
					m_enemyAnimeMode = EnemyAnimeMode::EnemyAnimeMode_Run;
				}
			}
		}
	}



	AttacksSlammingDown();
}

void Goblin::AttacksSlammingDown()
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
				std::shared_ptr<AttackBase>spAttack = std::make_shared<Attack_Slamming>();
				m_wpAttack = spAttack;
				spAttack->SetPos(m_pos);
				spAttack->SetPlayer(m_wpPlayer.lock());
				SceneManager::Instance().AddObject(spAttack);

				m_enemyAnimeMode = EnemyBase::EnemyAnimeMode_Attack;
				m_animeChangeFlg = true;
			}
		}
	}

	if (m_attackFlg)
	{
		float progress = m_spAnimator->GetTime() / m_spAnimator->GetMaxLength();

		if (progress > m_slammingDownAttackAnimeHitTriggerTime && !m_hitTriggered)
		{
			std::shared_ptr<AttackBase>spAttack = m_wpAttack.lock();
			if (spAttack)
			{
				m_hitTriggered = true;
				spAttack->SetAttackFlg();
				spAttack->SetAttackPower(m_status.m_attack.m_nowAttack);
				
				// 叩きつけのエフェクト（地面の高さに出す）
			// ゴーレムの向いている方向に少し前（叩いた場所）
				const float rad = DirectX::XMConvertToRadians(m_angle);
				Math::Vector3 slamPos = m_pos + Math::Vector3{ sinf(rad), 0, cosf(rad) } * 1.5f;	// 1.5 は腕の長さに合わせて調整
				slamPos.y = m_pos.y;	// 地面の高さ
				KdEffekseerManager::GetInstance().Play("EnemyAttack/Goblin/Goblin_Slam.efkefc", slamPos, 2.0f, 1.0f, false);

				// 叩きつけの音
				if (m_spSlam3DSE)
				{
					m_spSlam3DSE->SetPos(slamPos);
					m_spSlam3DSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
					m_spSlam3DSE->Play(false);
				}

			}
		}
	}


	SetAttackGaugePercent(m_attackWait / m_attackWaitMax);
}
