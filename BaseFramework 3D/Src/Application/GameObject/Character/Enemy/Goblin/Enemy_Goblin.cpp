#include "Enemy_Goblin.h"
#include"Attack/Attack_Slamming/Attack_Slamming.h"

#include"../../../../Scene/SceneManager.h"
void Goblin::Init()
{
	EnemyAmbush::Init();
	m_charaStatusFilePath = "Asset/Data/ObjeData/Character/Enemy/Goblin/Status/Status.json";
	if (!m_spCharaModel)
	{
		m_spCharaModel = std::make_shared<KdModelWork>();
		m_spCharaModel->SetModelData("Asset/Models/Character/Enemy/Goblin/goblin-FINAL.gltf");
		//アニメータの準備
		m_spAnimetor = std::make_shared<KdAnimator>();
		m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_goblinAnimeName.IdleAnime), true);



		if (!m_pCollider)
		{
			m_pCollider = std::make_unique<KdCollider>();
			m_pCollider->RegisterCollisionShape("Goblin", m_spCharaModel, KdCollider::TypeBump | KdCollider::TypeCameraOcclusion);
		}

		//if (!m_pDebugWire)
		//{
		//	m_pDebugWire = std::make_unique<KdDebugWireFrame>();
		//}

		EnemyBase::Init();
	}

}

void Goblin::Update()
{
	m_pos.y -= m_Gravity;
	m_Gravity += m_gravityPower;

	EnemyAmbush::Update();

	Attack();

	////重力
	//m_pos.y -= m_Gravity;
	//m_Gravity += m_gravityPower;

	if (m_isDead)
	{
		if (!m_spAnimetor) { return; }
		if (m_spAnimetor->IsAnimationEnd())
		{
			m_isExpired = true;
		}
	}


	if (m_AnimeChangeFlg)
	{
		ChangeAnime();
	}
}

void Goblin::PreUpdate()
{
	EnemyBase::PreUpdate();
	HPPosPostUpdate();
}

void Goblin::ChangeAnime()
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
		m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_goblinAnimeName.IdleAnime), true);
		break;
	case EnemyBase::EnemyAnimeMode_Walk:
		m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_goblinAnimeName.WalkAnime), true);
		break;
	case EnemyBase::EnemyAnimeMode_Run:
		m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_goblinAnimeName.RunAnime), true);
		break;
	case EnemyBase::EnemyAnimeMode_Attck:
		m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_goblinAnimeName.SlammingDownAttackAnime), false);
		m_attackFlg = true;
		break;
	case EnemyBase::EnemyAnimeMode_Die:
		m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_goblinAnimeName.DieAnime), false);
		break;
	default:
		break;
	}

	m_AnimeChangeFlg = false;
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
			Math::Vector3 handPos = handNode->m_worldTransform.Translation() + m_pos;
			spAttack->SetPos(handPos);
			spAttack->SetKnockbackDir(m_mWorld.Backward());

			if (m_spAnimetor)
			{
				//アニメーションが終わったら攻撃終了
				if (m_spAnimetor->IsAnimationEnd())
				{
					m_attackFlg = false;
					if (spAttack)
					{
						spAttack->FinishAttack_Slamming();
						m_hitTriggered = false;
					}

					m_attackWait = m_attackWaitMax;

					m_AnimeChangeFlg = true;
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

				m_enemyAnimeMode = EnemyBase::EnemyAnimeMode_Attck;
				m_AnimeChangeFlg = true;

			}
		}
	}

	if (m_attackFlg)
	{
		float progress = m_spAnimetor->GetTime() / m_spAnimetor->GetMaxLength();

		if (progress > m_SlammingDownAttackAnimeHitTriggerTime && !m_hitTriggered)
		{
			std::shared_ptr<AttackBase>spAttack = m_wpAttack.lock();
			if (spAttack)
			{
				m_hitTriggered = true;
				spAttack->SetAttckFlg();
				spAttack->SetAttckPower(m_status.attck.nowAttck);
			}
		}
	}


	SetAttackGagePercent(m_attackWait/m_attackWaitMax);
}

