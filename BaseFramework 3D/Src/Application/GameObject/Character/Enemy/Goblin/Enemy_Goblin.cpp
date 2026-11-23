#include "Enemy_Goblin.h"
#include"Attack/Attack_Slamming/Attack_Slamming.h"

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

		//if (CheckInScreen(spCamera->GetBoundingFrustum(), m_pos, m_viewDistance))
		//{
		//	// 視界範囲描画
		//	Math::Vector3 pos = GetPos();
		//	Math::Vector3 eyePos = pos + Math::Vector3(0, 1.0f, 0);
		//	Math::Vector3 dir = m_mWorld.Backward();
		//	dir.Normalize();

		//	// 壁などの遮蔽物を考慮した照射距離を算出
		//	float lightDist = m_viewDistance;

		//	// スポットライトを追加 (色, 照射距離, 位置, 方向, 照射角度)
		//	KdShaderManager::Instance().WorkAmbientController().AddSpotLight(
		//		{ 0.5f, 0.0f, 0.0f },			// 光の色・強度
		//		lightDist,						// 遮蔽された照射距離
		//		m_pos,							// 位置
		//		dir,							// 前方方向
		//		m_viewAngle * 0.5f,				// 照射角 
		//		false
		//	);
		//}
	}

}

void Goblin::OnAttackHit(float _damage, float _knockbackDistance, const Math::Vector3& _knockbackDir, float _hitStunTime, bool _isCritical, float _ignoreRate, const Math::Vector3* _hitPos)
{
	//実際に減ったHPを知るために、当たる前のHPを覚えておく
	const float hpBefore = m_status.m_HP.m_nowHP;

	EnemyBase::OnAttackHit(_damage, _knockbackDistance, _knockbackDir, _hitStunTime, _isCritical, _ignoreRate, _hitPos);

	//攻撃していない・死んだ・ひるまない設定なら何もしない
	if (!m_attackFlg || m_isDead || m_flinchRate <= 0.0f) { return; }

	//攻撃中に受けたダメージをひるみ値にためる
	m_flinchValue += std::max(0.0f, hpBefore - m_status.m_HP.m_nowHP);

	//最大HPの m_flinchRate 以上たまったら、攻撃を止める(待ち時間とクールタイムは普通に入る)
	if (m_flinchValue >= m_status.m_HP.m_maxHP * m_flinchRate)
	{
		m_flinchValue = 0.0f;
		EndPattern();
	}
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

void Goblin::Attack()
{
	if (m_isDead) { return; }
	UpdatePatternCooldown();

	//もし見失ってたら攻撃をやめる
	if (!m_playerChaseFlg)
	{
		CancelPattern();
		return;
	}

	if (m_attackFlg)
	{
		//攻撃中は段を進める
		UpdateStep();
	}
	else if (!m_patternList.empty())
	{
		if (m_attackWait > 0)
		{
			//次の攻撃までの待ち
			m_attackWait -= DeltaTime::Instance().GetGameDeltaTime();
		}
		else
		{
			//距離・LV・重み・クールタイムで選ぶ(-1なら今は出せる技がない)
			const float distance = (m_pos - m_playerPos).Length();
			const int patternIndex = SelectPattern(distance);
			if (patternIndex >= 0)
			{
				StartPattern(patternIndex);
			}
		}
	}

	//今回の待ちの長さで割る(0では割らない)
	const float gauge = (m_attackWaitNowMax > 0.0f) ? m_attackWait / m_attackWaitNowMax : 0.0f;
	SetAttackGaugePercent(gauge);
}

void Goblin::StartPattern(int _patternIndex)
{
	if (_patternIndex < 0 || _patternIndex >= (int)m_patternList.size()) { return; }

	m_nowPatternIndex = _patternIndex;
	m_attackFlg = true;
	m_flinchValue = 0.0f;	//ひるみ値はパターンごとに数え直す
	m_enemyAnimeMode = EnemyBase::EnemyAnimeMode_Attack;

	//攻撃時の叫び声
	if (m_spCrying3DSE)
	{
		m_spCrying3DSE->SetPos(m_pos);
		m_spCrying3DSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
		m_spCrying3DSE->Play(false);
	}

	StartStep(0);
}

void Goblin::StartStep(int _stepIndex)
{
	FaceToPlayer();

	m_nowStepIndex = _stepIndex;
	m_hitOn = false;
	m_stepForwardDone = 0.0f;

	const ComboStep* step = GetNowStep();
	if (!step)
	{
		EndPattern();
		return;
	}

	//この段の再生速度
	m_animeSpeedRate = step->m_speed;

	//段のアニメに切り替える
	if (m_spAnimator && m_spCharaModel)
	{
		m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(step->m_animeName), 0.1f, false);
	}

	//前の段の当たり判定が残っていたら消して、新しく作る
	//(段ごとに作り直すと「もう当てた」がリセットされて、連撃が全部当たる)
	if (std::shared_ptr<AttackBase>spOld = m_wpAttack.lock())
	{
		spOld->FinishAttack_Slamming();
	}

	std::shared_ptr<AttackBase>spAttack = std::make_shared<Attack_Slamming>();
	m_wpAttack = spAttack;
	spAttack->SetPos(m_pos);
	spAttack->SetPlayer(m_wpPlayer.lock());
	SceneManager::Instance().AddObject(spAttack);
}

void Goblin::UpdateStep()
{
	const ComboStep* step = GetNowStep();
	if (!step)
	{
		EndPattern();
		return;
	}

	const float progress = GetNowStepProgress();

	//踏み込み(m_stepForward が0なら何もしない)
	UpdateStepForward(progress);

	std::shared_ptr<AttackBase>spAttack = m_wpAttack.lock();
	if (spAttack)
	{
		//当たり判定をこの段の骨の位置に置く
		const std::string hitNode = GetNowHitNode();
		if (!hitNode.empty() && m_spCharaModel)
		{
			auto* node = m_spCharaModel->FindNode(hitNode);
			if (node)
			{
				Math::Matrix nodeMat = node->m_worldTransform * m_mWorld;
				spAttack->SetPos(nodeMat.Translation());
			}
		}
		spAttack->SetKnockbackDir(m_mWorld.Backward());

		//当たり判定を出す
		if (progress > step->m_hitStart && !m_hitOn)
		{
			m_hitOn = true;
			spAttack->SetAttackFlg();
			spAttack->SetAttackPower(m_status.m_attack.m_nowAttack * step->m_damageRate);

			// 叩きつけのエフェクト（地面の高さに出す）(進め方7で m_events に置き換える)
			// ゴブリンの向いている方向に少し前（叩いた場所）
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

		//当たり判定を止める(判定の物自体は EndPattern で消す)
		if (progress > step->m_hitEnd)
		{
			spAttack->SetAttackFlg(false);
		}
	}

	//叫び声を敵に付いていかせる
	if (m_spCrying3DSE && m_spCrying3DSE->IsPlaying())
	{
		m_spCrying3DSE->SetPos(m_pos);
	}

	//最後の段でアニメが終わったらパターンを終わる
	//(次の段へ進む処理は進め方4で足す)
	if (IsLastStep())
	{
		if (m_spAnimator && m_spAnimator->IsAnimationEnd())
		{
			EndPattern();
		}
	}
	else
	{
		if (progress >= step->m_cancelPoint)
		{
			StartStep(m_nowStepIndex + 1);
			return;
		}
	}

}

void Goblin::EndPattern()
{
	//次の待ち時間(パターンが分からなければ今までどおり)
	m_attackWait = m_attackWaitMax;

	//使ったパターンを記録する(番号を-1に戻す前に)
	if (m_nowPatternIndex >= 0 && m_nowPatternIndex < (int)m_patternList.size())
	{
		AttackPattern& pattern = m_patternList[m_nowPatternIndex];

		//クールタイムを始める
		pattern.m_cooldownTimer = pattern.m_cooldown;

		//同じ技を続けないために覚えておく
		m_lastPatternIndex = m_nowPatternIndex;

		//m_waitAfter が入っていればその秒数だけ休む
		if (pattern.m_waitAfter > 0.0f)
		{
			m_attackWait = pattern.m_waitAfter;
		}
	}

	//今回の待ちの長さ(攻撃ゲージ用)
	m_attackWaitNowMax = m_attackWait;

	//当たり判定を消して、番号と再生速度を戻す
	CancelPattern();

	//走りに戻す
	m_animeChangeFlg = true;
	m_enemyAnimeMode = EnemyAnimeMode::EnemyAnimeMode_Run;
}

void Goblin::CancelPattern()
{
	//当たり判定が残っていたら消す
	if (std::shared_ptr<AttackBase>spAttack = m_wpAttack.lock())
	{
		spAttack->FinishAttack_Slamming();
	}

	m_attackFlg = false;
	m_hitOn = false;
	m_nowPatternIndex = -1;
	m_nowStepIndex = -1;
	m_animeSpeedRate = 1.0f;

	//アニメ・待ち時間・クールタイムはここでは触らない(EndPattern との違い)
}
