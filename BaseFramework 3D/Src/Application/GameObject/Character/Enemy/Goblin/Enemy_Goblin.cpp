#include "Enemy_Goblin.h"

#include"../../../../Scene/SceneManager.h"

#include"../../../Camera/CameraBase.h"

void Goblin::Init()
{
	EnemyAmbush::Init();
	m_wanderRadius = 20;
	m_charaDefaultStatusFilePath = "Asset/Data/ObjeData/Character/Enemy/Goblin/Status/Status.json";
	m_patternFilePath = "Asset/Data/ObjeData/EnemyAttackPattern/Goblin.json";
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

	m_attackWaitNowMax = m_attackWaitMax;
	LoadAttackPatterns();
}

void Goblin::Update()
{
	if (m_isDead)
	{
		if (!m_spAnimator) { return; }

		// 死亡時に出しっぱなしの攻撃判定を必ず消す(再生速度も戻るので、死んだアニメは普通の速さ)
		CancelPattern();

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

	if (!m_attackFlg)
	{
		EnemyAmbush::Update();
	}



	//パターン攻撃(待ち時間・選ぶ・段を進める・攻撃ゲージ。EnemyBase)
	UpdatePatternAttack();

	//攻撃中は EnemyAmbush::Update を呼ばないので、向き直し・踏み込みをここで行列に反映する
	if (m_attackFlg)
	{
		Math::Matrix tMat = Math::Matrix::CreateTranslation(m_pos);
		Math::Matrix rMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angle));
		m_mWorld = rMat * tMat;

		if (m_spCrying3DSE && m_spCrying3DSE->IsPlaying())
		{
			m_spCrying3DSE->SetPos(m_pos);
		}
	}

	if (m_animeChangeFlg)
	{
		ChangeAnime();
	}

	UpdateFootstepSE();




	//叫び声を敵に付いていかせる
	if (m_spCrying3DSE && m_spCrying3DSE->IsPlaying())
	{
		m_spCrying3DSE->SetPos(m_pos);
	}
}

void Goblin::PreUpdate()
{
	EnemyBase::PreUpdate();
	HPPosPostUpdate();
}

void Goblin::Release()
{
	EnemyBase::Release();

	//攻撃の当たり判定が残っていたら消す
	CancelPattern();

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
		break;
	case EnemyBase::EnemyAnimeMode_Die:
		m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_goblinAnimeName.m_dieAnime), 0.2f, false);
		break;
	default:
		break;
	}

	m_animeChangeFlg = false;
}

void Goblin::OnPatternStart()
{
	//攻撃時の叫び声
	if (m_spCrying3DSE)
	{
		m_spCrying3DSE->SetPos(m_pos);
		m_spCrying3DSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
		m_spCrying3DSE->Play(false);
	}
}
