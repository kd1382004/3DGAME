#include "Giant.h"

#include"../../../../UI/HPBar/HPBar.h"
#include"../../../../UI/AttackGage/AttackGage.h"

#include"Attack/AttackJumpSlam/AttackJumpSlam.h"
#include"Attack/AttackLeftPunch/AttackLeftPunch.h"

#include"../../../../../Scene/SceneManager.h"
#include"../../../../../Scene/GameScene/GameScene.h"

#include"../../../../Terrains/Map/MapManager.h"

void Giant::Init()
{
	m_charaDefaultStatusFilePath = "Asset/Data/ObjeData/Character/Enemy/Boss/Giant/Status/Status.json";
	if (!m_spCharaModel)
	{
		m_spCharaModel = std::make_shared<KdModelWork>();
		m_spCharaModel->SetModelData("Asset/Models/Character/Enemy/Boss/Giant/Giantblend.gltf");

		m_spAnimator = std::make_shared<KdAnimator>();
		m_spAnimator->SetAnimation(m_spCharaModel->GetAnimation(m_giantAnimeName.m_runAnime), true);

		m_angle = 0;

		m_loseSightDistance = 1000;

		if (!m_pCollider)
		{
			m_pCollider = std::make_unique<KdCollider>();
			m_pCollider->RegisterCollisionShape("Giant", m_spCharaModel, KdCollider::TypeBump | KdCollider::TypeCameraOcclusion | KdCollider::TypeDamage);
		}

		EnemyBase::Init();
		m_playerChaseFlg = true;
		m_attackFlg = false;
		m_playerChaseDir = 4;
		m_playerChaseResumeDir = 4.5f;

		if (!m_spRun3DSE)
		{
			m_spRun3DSE = KdAudioManager::Instance().Play3D("Asset/Sound/SE/Enemy/Giant/Walk.wav", m_pos, false);
			if (m_spRun3DSE)
			{
				m_spRun3DSE->Stop();
				m_spRun3DSE->SetCurveDistanceScaler(30.0f);
			}
		}
		if (!m_spPunch3DSE)
		{
			m_spPunch3DSE = KdAudioManager::Instance().Play3D("Asset/Sound/SE/Enemy/Giant/Punch.wav", m_pos, false);
			if (m_spPunch3DSE)
			{
				m_spPunch3DSE->Stop();
				m_spPunch3DSE->SetCurveDistanceScaler(30.0f);
			}
		}
		if (!m_spCrying3DSE)
		{
			m_spCrying3DSE = KdAudioManager::Instance().Play3D("Asset/Sound/SE/Enemy/Giant/CryingSound.wav", m_pos, false);
			if (m_spCrying3DSE)
			{
				m_spCrying3DSE->Stop();
				m_spCrying3DSE->SetCurveDistanceScaler(30.0f);
			}
		}
		if (!m_spDon3DSE)
		{
			m_spDon3DSE = KdAudioManager::Instance().Play3D("Asset/Sound/SE/Enemy/Giant/Don.wav", m_pos, false);
			if (m_spDon3DSE)
			{
				m_spDon3DSE->Stop();
				m_spDon3DSE->SetCurveDistanceScaler(30.0f);
			}
		}
	}

	m_attackCoolTime = 5;
	m_material = EnemyMaterial::Flesh;
}

void Giant::PreUpdate()
{
	EnemyBase::PreUpdate();


}

void Giant::Update()
{

	if (m_isDead)
	{
		if (m_spRun3DSE) { m_spRun3DSE->Stop(); }
		if (m_spCrying3DSE) { m_spCrying3DSE->Stop(); }
		if (!m_spAnimator) { return; }
		EnemyAnimeModeUpdate();
		if (m_spAnimator->IsAnimationEnd())
		{
			std::shared_ptr<GameScene>spGameScene = m_wpGameScene.lock();
			std::shared_ptr<MapManager>spMapManager = m_wpMapManager.lock();
			if (spGameScene && spMapManager)
			{
				Math::Vector3 nextPos = spMapManager->GetNextWarpGatePos();
				Math::Vector3 escapePos = spMapManager->GetEscapeWarpGatePos();


				spGameScene->WarpGateInit(nextPos, escapePos);
			}
			m_isExpired = true;
		}
		return;
	}
	else
	{
		if (m_playerChaseFlg && !m_attackFlg)
		{
			m_moveVec = m_playerPos - m_pos;
			m_moveVec.y = 0;
			m_moveVec.Normalize();
		}
	}

	//プレイヤー追跡
	if (m_playerChaseFlg)
	{
		PlayerChase();
	}

	//角度更新
	AngleUpdate();

	//アニメーション更新
	EnemyAnimeModeUpdate();


	//重力更新
	ApplyGravity();


	if (GetAsyncKeyState('V'))
	{
		m_attackFlg = false;
	}


	AttackMode();


	if (m_attackFlg)
	{
		AttackUpdate();
	}

	UpdateFootstepSE();

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
	ChangeAnime();
}

void Giant::PreDraw()
{
	EnemyBase::PreDraw();
	std::shared_ptr<HPBar>spHPBar = m_wpHPBar.lock();
	if (spHPBar)
	{
		Math::Vector2 pos = { 0,300 };
		spHPBar->Set2DPos(pos);
		spHPBar->SetSiz(3);
		spHPBar->SetDrawFlg(true);
	}


	std::shared_ptr<AttackGage>spAttackGauge = m_wpAttackGauge.lock();
	if (spAttackGauge)
	{
		Math::Vector2 pos = { 0,250 };
		spAttackGauge->Set2DPos(pos);
		spAttackGauge->SetSiz(3);
		spAttackGauge->SetDrawFlg(true);
	}
}

void Giant::GenerateDepthMapFromLight()
{
	if (m_spCharaModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spCharaModel, m_mWorld);
	}
}

void Giant::DrawLit()
{
	if (m_spCharaModel)
	{

		UAEffectShaderManager::Instance().WriteCBCircleEffectEnable(false);
		UAEffectShaderManager::Instance().WriteCBBoxEffectEnable(false);

		//モデルが暗すぎるため無理やり明るく
		Math::Color color = { 5,5,5,1 };
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spCharaModel, m_mWorld, color);


		UAEffectShaderManager::Instance().WriteCBCircleEffectEnable(true);
		UAEffectShaderManager::Instance().WriteCBBoxEffectEnable(true);


	}
}

void Giant::OnAttackHit(float _damage, float _knockbackDistance, const Math::Vector3& _knockbackDir, float _hitStunTime, bool _isCritical, float _ignoreRate, const Math::Vector3* _hitPos)
{
	CharacterBase::OnAttackHit(_damage, _knockbackDistance, _knockbackDir, _hitStunTime, _isCritical, _ignoreRate, _hitPos);
}

void Giant::Release()
{
	EnemyBase::Release();

	if (m_spRun3DSE)
	{
		m_spRun3DSE->Stop();
		m_spRun3DSE = nullptr;
	}
	if (m_spPunch3DSE)
	{
		m_spPunch3DSE->Stop();
		m_spPunch3DSE = nullptr;
	}
	if (m_spCrying3DSE)
	{
		m_spCrying3DSE->Stop();
		m_spCrying3DSE = nullptr;
	}
	if (m_spDon3DSE)
	{
		m_spDon3DSE->Stop();
		m_spDon3DSE = nullptr;
	}

	auto spGameScene = m_wpGameScene.lock();
	if (spGameScene)
	{
		spGameScene->AddEnemyKill();
	}
}

Math::Matrix Giant::GetBoneWorldMatrix(std::string _nodeName)
{

	if (!m_spCharaModel) { return Math::Matrix::Identity; }

	auto* handNode = m_spCharaModel->FindNode(_nodeName);
	Math::Matrix handWorld;
	if (handNode)
	{
		handWorld = handNode->m_worldTransform * m_mWorld;
	}

	return handWorld;
}

void Giant::AttackMode()
{
	if (!m_attackFlg)
	{
		//攻撃の種類を選択
		if (m_attackCoolTime >= 0)
		{
			m_attackCoolTime -= DeltaTime::Instance().GetGameDeltaTime();

			std::shared_ptr<AttackGage>spAttackGauge = m_wpAttackGauge.lock();
			if (spAttackGauge)
			{
				spAttackGauge->SetAttackGaugeTexPercent(m_attackCoolTime / m_attackCoolTimeMax);
			}

			return;
		}
		else
		{

			if (KdRandom::GetInt(1, 100) < 20)
			{
				m_giantAttackMode = GiantAttackMode::JumpSlamAttack;
			}
			else
			{
				m_giantAttackMode = GiantAttackMode::LeftPunchAttack;
			}


			m_moveVec = m_playerPos - m_pos;
			m_moveVec.Normalize();
		}
		m_giantAttackMode = GiantAttackMode::JumpSlamAttack;
		switch (m_giantAttackMode)
		{
		case Giant::LeftPunchAttack:
			m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_giantAnimeName.m_leftPunchAttackAnime), 0.1f, false);
			m_attackFlg = true;
			m_attackCoolTime = m_attackCoolTimeMax;
			break;
		case Giant::RightAttack:
			m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_giantAnimeName.m_rightAttackAnime), 0.1f, false);
			break;
		case Giant::JumpSlamAttack:
			m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_giantAnimeName.m_jumpSlamAttackAnime), 0.1f, false);
			m_isAttackJumpSlamHITFlg = false;
			m_jumpStartPos = m_pos;
			m_jumpSlamAttackMode = JumpSlamAttackMode_JumpSlamAttack;
			m_attackFlg = true;
			m_attackCoolTime = m_attackCoolTimeMax;

			if (m_spCrying3DSE)
			{
				m_spCrying3DSE->SetPos(m_pos);
				m_spCrying3DSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
				m_spCrying3DSE->Play(false);
			}


			m_jumpSlam_StartEfkefcFlg = true;
			m_jumpSlam_EndEfkefcFlg = true;
			break;
		default:
			break;
		}

	}
	else
	{

	}
}

void Giant::ChangeAttackAnime()
{
	switch (m_giantAttackMode)
	{
	case Giant::LeftPunchAttack:
		m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_giantAnimeName.m_leftPunchAttackAnime), 0.1f, false);
		break;
	case Giant::RightAttack:
		m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_giantAnimeName.m_rightAttackAnime), 0.1f, false);
		break;
	case Giant::JumpSlamAttack:
		m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_giantAnimeName.m_jumpSlamAttackAnime), 0.1f, false);
		break;
	default:
		break;
	}
}

void Giant::AttackUpdate()
{
	switch (m_giantAttackMode)
	{
	case Giant::LeftPunchAttack:
		LeftAttackUpdate();
		break;
	case Giant::RightAttack:
		break;
	case Giant::JumpSlamAttack:
		AttackJumpSlamUpdate();
		break;
	default:
		break;
	}
}

void Giant::LeftAttackUpdate()
{
	std::shared_ptr<AttackLeftPunch>spLeftAttack = m_wpLeftAttack.lock();
	if (spLeftAttack)
	{
		float p = m_spAnimator->GetAnimeProgress();
		if (!m_isAttackleftHITFlg)
		{
			if (p >= m_leftAttackHitStart)
			{
				spLeftAttack->SetAttackFlg(true);
				m_isAttackleftHITFlg = true;

				if (m_spPunch3DSE)
				{
					m_spPunch3DSE->SetPos(m_pos);
					m_spPunch3DSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
					m_spPunch3DSE->Play(false);
				}

				{
					const float yawRad = DirectX::XMConvertToRadians(m_angle);
					Math::Vector3 dir = { sinf(yawRad), 0.0f, cosf(yawRad) };
					Math::Vector3 punchPos = m_pos + dir * 5;
					punchPos.y += 2.0f;		// 拳の高さ（巨人の大きさに合わせて調整）

					KdEffekseerManager::GetInstance().Play("EnemyAttack/Giant/Giant_Punch.efkefc", punchPos, 2.5f, 1.0f, false);

					// パンチで画面を揺らす（近いほど強い）
					CameraShakeAt(punchPos, 0.3f, 0.15f, 30.0f);
				}
			}
			m_playerChaseFlg = false;
			m_moveVec = {};
		}
		else
		{
			if (p >= m_leftAttackHitEnd)
			{
				spLeftAttack->SetAttackFlg(false);
			}
		}


		float yawRad = DirectX::XMConvertToRadians(m_angle);

		Math::Vector3 dir;
		dir.x = sinf(yawRad);
		dir.y = 0.0f;
		dir.z = cosf(yawRad);
		Math::Vector3 attackPos = m_pos + dir * 5;

		spLeftAttack->SetTargetCenterPos(attackPos);
		spLeftAttack->SetAngleY(m_angle);
		spLeftAttack->SetKnockbackDir(m_mWorld.Backward());
		spLeftAttack->AttackLeftPunchUpdate();


		if (!m_spAnimator) { return; }

		if (m_spAnimator->IsAnimationEnd())
		{
			m_attackFlg = false;
			m_enemyAnimeMode = EnemyBase::EnemyAnimeMode_Run;
			m_animeChangeFlg = true;
			m_isAttackleftHITFlg = false;
			m_playerChaseFlg = true;
		}

	}
	else
	{

		std::shared_ptr<AttackLeftPunch>spAttackJumpSlam = std::make_shared<AttackLeftPunch>();
		m_wpLeftAttack = spAttackJumpSlam;
		spAttackJumpSlam->SetPlayer(m_wpPlayer.lock());
		spAttackJumpSlam->SetAttackPower(m_status.m_attack.m_baseAttackPower);
		SceneManager::Instance().AddObject(spAttackJumpSlam);
	}
}

void Giant::JumpSlamAttackMode_JumpSlamAttackUpdate()
{
	std::shared_ptr<AttackJumpSlam>spAttackJumpSlam = m_wpAttackJumpSlam.lock();

	//モデルのアニメーション事態に動きがあるためそれをなくす(x.z成分)
	AttackJumpSlamAnimeUpdate();
	float p = m_spAnimator->GetAnimeProgress();
	if (p == 1)
	{
		m_jumpSlamAttackMode = JumpSlamAttackMode_Idle;
		m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_giantAnimeName.m_idleAnime), 0.2f, true);
		m_attackJumpSlamIdleNow = m_attackJumpSlamIdleMax;
		return;
	}



	if (spAttackJumpSlam)
	{
		if (p < 0.3)
		{
			m_attackJumpSlamTargetPos = m_playerPos;
		}

		if (m_jumpSlam_StartEfkefcFlg && p >= 0.14)
		{
			m_jumpSlam_StartEfkefcFlg = false;
			KdEffekseerManager::GetInstance().Play("AttackJumpSlam/Start/JumpSlam_Start.efkefc", m_pos, 2.0f, 1.0f, false);
		}



		if (m_jumpSlam_EndEfkefcFlg && p >= 0.4)
		{
			m_jumpSlam_EndEfkefcFlg = false;
			auto effect= KdEffekseerManager::GetInstance().Play("AttackJumpSlam/End/JumpSlam_End.efkefc", m_pos, 2.0f, 1.0f, false);
			int i = effect.lock()->GetHandle();
			KdEffekseerManager::GetInstance().SetRotation(i, Math::Vector3(0, 1, 0), DirectX::XMConvertToRadians(m_angle));


		}

		spAttackJumpSlam->SetRadiusDrawFlg(true);
		if (!m_isAttackJumpSlamHITFlg)
		{

			if (p >= m_jumpSlamHitStart)
			{
				spAttackJumpSlam->SetAttackFlg(true);
				m_isAttackJumpSlamHITFlg = true;

				if (m_spDon3DSE)
				{
					m_spDon3DSE->SetPos(m_pos);
					m_spDon3DSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
					m_spDon3DSE->Play(false);
				}

				// ジャンプの着地で画面を大きく揺らす（近いほど強い）
				CameraShakeAt(m_pos, 0.5f, 0.3f, 40.0f);

				
			}
			else
			{
				//0.0 ~ 1.0 に正規化
				float t = std::clamp(p / 0.3f, 0.0f, 1.0f);

				Math::Vector3 startPos = m_jumpStartPos;   // ジャンプ開始地点

				Math::Vector3 pos = startPos + (m_attackJumpSlamTargetPos - startPos) * t;  // t=0でstart, t=1でplayer
				m_pos = pos;

				if (m_spCrying3DSE && m_spCrying3DSE->IsPlaying())
				{
					m_spCrying3DSE->SetPos(m_pos);
				}

				m_moveVec = (m_attackJumpSlamTargetPos - startPos);
				m_moveVec.Normalize();
				AngleUpdate();
			}
		}
		else
		{
			if (p >= m_jumpSlamHitEnd)
			{
				spAttackJumpSlam->SetAttackFlg(false);
				m_playerChaseFlg = false;


				spAttackJumpSlam->SetRadiusDrawFlg(false);



			}
		}



		spAttackJumpSlam->SetRPos(m_attackJumpSlamTargetPos);
		spAttackJumpSlam->SetKnockbackDir(m_mWorld.Backward());
		spAttackJumpSlam->AttackJumpSlamUpdate();
	}
	else
	{
		std::shared_ptr<AttackJumpSlam>spAttackJumpSlam = std::make_shared<AttackJumpSlam>();
		m_wpAttackJumpSlam = spAttackJumpSlam;
		m_isAttackJumpSlamHITFlg = false;
		spAttackJumpSlam->SetAttackFlg(m_isAttackJumpSlamHITFlg);
		spAttackJumpSlam->SetAttackPower(m_status.m_attack.m_baseAttackPower);


		spAttackJumpSlam->SetPlayer(m_wpPlayer.lock());
		SceneManager::Instance().AddObject(spAttackJumpSlam);
	}
}

void Giant::JumpSlamAttackMode_IdleUpdate()
{
	m_attackJumpSlamIdleNow -= DeltaTime::Instance().GetGameDeltaTime();
	if (m_attackJumpSlamIdleNow <= 0)
	{
		m_attackFlg = false;
		m_enemyAnimeMode = EnemyBase::EnemyAnimeMode_Run;
		m_animeChangeFlg = true;
		m_playerChaseFlg = true;
	}

}

void Giant::AttackJumpSlamUpdate()
{

	switch (m_jumpSlamAttackMode)
	{
	case Giant::JumpSlamAttackMode_JumpSlamAttack:
		JumpSlamAttackMode_JumpSlamAttackUpdate();
		break;
	case Giant::JumpSlamAttackMode_Idle:
		//待機へのクロスフェード中もジャンプアニメの前進が混ざるので打ち消す
		AttackJumpSlamAnimeUpdate();
		JumpSlamAttackMode_IdleUpdate();
		break;
	default:
		break;
	}


}

void Giant::AttackJumpSlamAnimeUpdate()
{
	const std::string kBoneHips = "mixamorigHips";
	auto* hips = m_spCharaModel->FindWorkNode(kBoneHips);
	if (hips)
	{
		Math::Vector3 t = hips->m_localTransform.Translation();

		t.x = 0;
		t.z = 0;

		//ジャンプ・着地などの上下動は残す
		hips->m_localTransform.Translation(t);
		//
		m_spCharaModel->CalcNodeMatrices();
	}
}

void Giant::ChangeAnime()
{
	if (!m_spAnimator) { return; }
	if (!m_animeChangeFlg) { return; }

	//攻撃中は変更しない
	if (m_attackFlg && !m_isDead) { return; }

	switch (m_enemyAnimeMode)
	{
	case EnemyBase::EnemyAnimeMode_Idle:
		m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_giantAnimeName.m_idleAnime), 0.2f, true);
		break;
	case EnemyBase::EnemyAnimeMode_Walk:
	case EnemyBase::EnemyAnimeMode_Run:
		m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_giantAnimeName.m_runAnime), 0.2f, true);
		break;
	case EnemyBase::EnemyAnimeMode_Attack:
		ChangeAttackAnime();
		m_attackFlg = true;
		break;
	case EnemyBase::EnemyAnimeMode_Die:
		m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_giantAnimeName.m_dieAnime), 0.2f, false);
		break;
	default:
		break;
	}

	m_animeChangeFlg = false;
}

void Giant::UpdateFootstepSE()
{
	if (m_isDead || m_attackFlg || !m_playerChaseFlg)
	{
		if (m_spRun3DSE) { m_spRun3DSE->Stop(); }
		return;
	}

	float seVol = VOLInfo::Instance().GetMSSEVol();

	if (m_spRun3DSE)
	{
		m_spRun3DSE->SetPos(m_pos);
		m_spRun3DSE->SetVolume(seVol);
		if (!m_spRun3DSE->IsPlaying())
		{
			m_spRun3DSE->Play();
		}
	}
}


