#include "Giant.h"

#include"../../../../UI/HPBar/HPBar.h"

#include"Attack/AttackJumpSlam/AttackJumpSlam.h"
#include"Attack/AttackLeftPunch/AttackLeftPunch.h"

#include"../../../../../Scene/SceneManager.h"
#include"../../../../../Scene/GameScene/GameScene.h"
void Giant::Init()
{
	m_charaStatusFilePath = "Asset/Data/ObjeData/Character/Enemy/Goblin/Status/Status.json";
	if (!m_spCharaModel)
	{
		m_spCharaModel = std::make_shared<KdModelWork>();
		m_spCharaModel->SetModelData("Asset/Models/Character/Enemy/Boss/Giant/Giantblend.gltf");

		m_spAnimetor = std::make_shared<KdAnimator>();
		m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_giantAnimeName.RunAnime), true);

		m_angle = 0;

		m_loseSightDistance = 1000;

		if (!m_pCollider)
		{
			m_pCollider = std::make_unique<KdCollider>();
			m_pCollider->RegisterCollisionShape("Goblin", m_spCharaModel, KdCollider::TypeBump | KdCollider::TypeCameraOcclusion);
		}

		EnemyBase::Init();
		m_playerChaseFlg = true;
		m_attackFlg = false;
	}
}

void Giant::PreUpdate()
{
	EnemyBase::PreUpdate();


}

void Giant::Update()
{

	if (m_isDead)
	{
		if (!m_spAnimetor) { return; }
		EnemyAnimeModeUpdate();
		if (m_spAnimetor->IsAnimationEnd())
		{
			std::shared_ptr<GameScene>spGameScene = m_wpGameScene.lock();
			if (spGameScene)
			{
				spGameScene->WarpGateInit(m_pos);
			}
			m_isExpired = true;
		}
		return;
	}

	//プレイヤー追跡
	if (m_playerChaseFlg)
	{
		PlayerChase();
	}

	//角度更新
	AngeleUpdate();

	//アニメーション更新
	EnemyAnimeModeUpdate();


	//重力更新
	m_pos.y -= m_Gravity;
	m_Gravity += m_gravityPower;


	if (GetAsyncKeyState('V'))
	{
		m_attackFlg = false;
	}


	AttackMode();


	if (m_attackFlg)
	{
		AttackUpdate();
	}

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
		//モデルが暗すぎるため無理やり明るく
		Math::Color color = { 5,5,5,1 };
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spCharaModel, m_mWorld, color);
	}
}

void Giant::OnAttackHit(float _damage, float _knockbackDistance, const Math::Vector3& _knockbackDir, float _hitStunTime, bool _isCritical, float _ignoreRate)
{
	CharacterBase::OnAttackHit(_damage, _knockbackDistance, _knockbackDir, _hitStunTime, _isCritical, _ignoreRate);
}

void Giant::AttackMode()
{
	if (!m_attackFlg)
	{
		//攻撃の種類を選択


		if (m_attackCoolTime >= 0)
		{
			m_attackCoolTime -= DeltaTime::Instance().GetGameDeltaTime();
			
			return;
		}
		else
		{

			if (KdRandom::GetInt(1, 100) < 10)
			{
				m_giantAttackMode = GiantAttackMode::JumpSlamAttack;
			}
			else
			{
				m_giantAttackMode = GiantAttackMode::LeftPunchAttack;
			}

		}


		switch (m_giantAttackMode)
		{
		case Giant::LeftPunchAttack:
			m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_giantAnimeName.LeftPunchAttackAnime), false);
			m_attackFlg = true;
			m_attackCoolTime = 3;
			break;
		case Giant::RightAttack:
			m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_giantAnimeName.RightAttackAnime), false);
			break;
		case Giant::JumpSlamAttack:
			m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_giantAnimeName.JumpSlamAttackAnime), false);
			m_IsAttackJumpSlamHITFlg = false;
			m_jnpStartPos = m_pos;
			m_JumpSlamAttackMode = JumpSlamAttackMode_JumpSlamAttack;
			m_attackFlg = true;
			m_attackCoolTime = 3;
			break;
		default:
			break;
		}

	}
}

void Giant::ChangeAttackAnime()
{
	switch (m_giantAttackMode)
	{
	case Giant::LeftPunchAttack:
		m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_giantAnimeName.LeftPunchAttackAnime), false);
		break;
	case Giant::RightAttack:
		m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_giantAnimeName.RightAttackAnime), false);
		break;
	case Giant::JumpSlamAttack:
		m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_giantAnimeName.JumpSlamAttackAnime), false);
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
	std::shared_ptr<AttackLeftPunch>spleftAttack = m_leftAttack.lock();
	if (spleftAttack)
	{
		float p = m_spAnimetor->GetAnimeProgress();
		if (!m_IsAttackleftHITFlg)
		{
			if (p >= m_leftAttackHitStart)
			{
				spleftAttack->SetAttckFlg(true);
				m_IsAttackleftHITFlg = true;
			}
			else
			{
				Math::Vector3 startPos = m_jnpStartPos;   // ジャンプ開始地点
				Math::Vector3 targetPos = m_playerPos; // プレイヤー位置

				m_moveVec = (targetPos - startPos);
				m_moveVec.Normalize();
				AngeleUpdate();
			}
		}
		else
		{
			if (p >= m_leftAttackHitEnd)
			{
				spleftAttack->SetAttckFlg(false);
			}
		}

		Math::Matrix m = GetBoneWorldMatrix(BONE_LEFT_HAND);
		spleftAttack->SetLPos(m.Translation());
		spleftAttack->SetKnockbackDir(m_mWorld.Backward());
		spleftAttack->AttackLeftPunchUpdate();


		if (!m_spAnimetor) { return; }

		if (m_spAnimetor->IsAnimationEnd())
		{
			m_attackFlg = false;
			m_enemyAnimeMode = EnemyBase::EnemyAnimeMode_Run;
			m_AnimeChangeFlg = true;
			m_IsAttackleftHITFlg = false;
		}

	}
	else
	{

		std::shared_ptr<AttackLeftPunch>spAttackJumpSlam = std::make_shared<AttackLeftPunch>();
		m_leftAttack = spAttackJumpSlam;
		spAttackJumpSlam->SetPlayer(m_wpPlayer.lock());
		spAttackJumpSlam->SetAttckPower(m_status.attck.baseAttckPowe);
		SceneManager::Instance().AddObject(spAttackJumpSlam);
	}
}

void Giant::JumpSlamAttackMode_JumpSlamAttackUpdate()
{
	//モデルのアニメーション事態に動きがあるためそれをなくす(x.z成分)
	AttackJumpSlamAnimeUpdate();
	float p = m_spAnimetor->GetAnimeProgress();
	if (p == 1)
	{
		m_JumpSlamAttackMode = JumpSlamAttackMode_Idle;
		m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_giantAnimeName.IdleAnime), true);
		m_attackJumpSlamIdleNow = m_attackJumpSlamIdleMax;
		return;
	}


	std::shared_ptr<AttackJumpSlam>spAttackJumpSlam = m_wpAttackJumpSlam.lock();
	if (spAttackJumpSlam)
	{
		if (!m_IsAttackJumpSlamHITFlg)
		{

			if (p >= m_JumpSlamHitStart)
			{
				spAttackJumpSlam->SetAttckFlg(true);
				m_IsAttackJumpSlamHITFlg = true;
			}
			else
			{
				// 0.0 ~ 0.5 を 0.0 ~ 1.0 に正規化
				float t = std::clamp(p / 0.5f, 0.0f, 1.0f);

				Math::Vector3 startPos = m_jnpStartPos;   // ジャンプ開始地点
				Math::Vector3 targetPos = m_playerPos; // プレイヤー位置

				Math::Vector3 pos = startPos + (targetPos - startPos) * t;  // t=0でstart, t=1でplayer
				m_pos = pos;

				m_moveVec = (targetPos - startPos);
				m_moveVec.Normalize();
				AngeleUpdate();
			}
		}
		else
		{
			if (p >= m_JumpSlamHitEnd)
			{
				spAttackJumpSlam->SetAttckFlg(false);
				m_playerChaseFlg = false;
			}
		}



		Math::Matrix handWorld = GetBoneWorldMatrix(BONE_LEFT_HAND);
		spAttackJumpSlam->SetLPos(handWorld.Translation());

		handWorld = GetBoneWorldMatrix(BONE_RIGHT_HAND);
		spAttackJumpSlam->SetRPos(handWorld.Translation());
		spAttackJumpSlam->SetKnockbackDir(m_mWorld.Backward());
		spAttackJumpSlam->AttackJumpSlamUpdate();

	}
	else
	{
		std::shared_ptr<AttackJumpSlam>spAttackJumpSlam = std::make_shared<AttackJumpSlam>();
		m_wpAttackJumpSlam = spAttackJumpSlam;
		m_IsAttackJumpSlamHITFlg = false;
		spAttackJumpSlam->SetAttckFlg(m_IsAttackJumpSlamHITFlg);
		spAttackJumpSlam->SetAttckPower(m_status.attck.baseAttckPowe);


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
		m_AnimeChangeFlg = true;
		m_playerChaseFlg = true;
	}

}

void Giant::AttackJumpSlamUpdate()
{

	switch (m_JumpSlamAttackMode)
	{
	case Giant::JumpSlamAttackMode_JumpSlamAttack:
		JumpSlamAttackMode_JumpSlamAttackUpdate();
		break;
	case Giant::JumpSlamAttackMode_Idle:
		JumpSlamAttackMode_IdleUpdate();
		break;
	default:
		break;
	}


}

void Giant::AttackJumpSlamAnimeUpdate()
{
	const std::string BONE_HIPS = "mixamorigHips";
	auto* hips = m_spCharaModel->FindWorkNode(BONE_HIPS);
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
	if (!m_spAnimetor) { return; }
	if (!m_AnimeChangeFlg) { return; }

	//攻撃中は変更しない
	if (m_attackFlg&&!m_isDead) { return; }

	switch (m_enemyAnimeMode)
	{
	case EnemyBase::EnemyAnimeMode_Idel:
		m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_giantAnimeName.IdleAnime), true);
		break;
	case EnemyBase::EnemyAnimeMode_Walk:
	case EnemyBase::EnemyAnimeMode_Run:
		m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_giantAnimeName.RunAnime), true);
		break;
	case EnemyBase::EnemyAnimeMode_Attck:
		ChangeAttackAnime();
		m_attackFlg = true;
		break;
	case EnemyBase::EnemyAnimeMode_Die:
		m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_giantAnimeName.DieAnime), false);
		break;
	default:
		break;
	}

	m_AnimeChangeFlg = false;
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

