//---- 自分のヘッダー ----
#include "Titan.h"											//ボス(タイタン)

//---- シーン ----
#include"../../../../../Scene/SceneManager.h"				//シーンの管理
#include"../../../../../Scene/GameScene/GameScene.h"		//ゲームシーン

//---- マップ ----
#include"../../../../Terrains/Map/MapManager.h"				//マップの管理・経路探索

//---- UI ----
#include"../../../../UI/HPBar/HPBar.h"						//HPバー
#include"../../../../UI/AttackGage/AttackGage.h"			//攻撃(溜め)ゲージ

//=============================================
// 基本の処理
//=============================================

void Titan::Init()
{
	m_charaDefaultStatusFilePath = "Asset/Data/ObjeData/Character/Enemy/Boss/Titan/Status/Status.json";
	m_patternFilePath = "Asset/Data/ObjeData/EnemyAttackPattern/Titan.json";
	m_rootMotionNodeName = "root";	//ジャンプ攻撃などで動く一番上の骨
	m_flinchRate = 0;
	m_attackWaitMax = 3;
	if (!m_spCharaModel)
	{
		m_spCharaModel = std::make_shared<KdModelWork>();
		m_spCharaModel->SetModelData("Asset/Models/Character/Enemy/Factory/Titan/Titan.gltf");

		m_spAnimator = std::make_shared<KdAnimator>();
		m_spAnimator->SetAnimation(m_spCharaModel->GetAnimation(m_titanAnimeName.m_idleAnime), true);

		m_angle = 0;

		m_loseSightDistance = 1000;

		if (!m_pCollider)
		{
			m_pCollider = std::make_unique<KdCollider>();
			m_pCollider->RegisterCollisionShape("Titan", m_spCharaModel, KdCollider::TypeBump | KdCollider::TypeCameraOcclusion | KdCollider::TypeDamage);
		}

		EnemyBase::Init();
		m_playerChaseFlg = true;
		m_attackFlg = false;
		m_playerChaseDir = 5.5f;		//体が大きいので、少し遠くで止まる
		m_playerChaseResumeDir = 6.0f;

		//効果音は専用のものがまだないので、ジャイアントのものを仮で使う
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

	//最初の攻撃までの待ち(秒)。攻撃はパターン(EnemyBase)に切り替え中
	m_attackWait = 5;
	m_attackWaitNowMax = 5;
	m_material = EnemyMaterial::Metal;	//ロボットなので金属のヒットエフェクトと音
	LoadAttackPatterns();
}

void Titan::PreUpdate()
{
	EnemyBase::PreUpdate();


}

void Titan::Update()
{

	if (m_isDead)
	{
		if (m_spRun3DSE) { m_spRun3DSE->Stop(); }
		if (m_spCrying3DSE) { m_spCrying3DSE->Stop(); }
		if (!m_spAnimator) { return; }
		CancelPattern();
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

	//プレイヤー追跡(攻撃中は止まる)
	if (m_playerChaseFlg && !m_attackFlg)
	{
		PlayerChase();
	}

	//角度更新
	AngleUpdate();

	//アニメーション更新
	EnemyAnimeModeUpdate();


	//重力更新
	ApplyGravity();

	//パターン攻撃(待ち時間・選ぶ・段を進める・攻撃ゲージ。EnemyBase)
	UpdatePatternAttack();

	//3D足音
	UpdateFootstepSE();

	//座標行列を作る
	Math::Matrix tMat = Math::Matrix::CreateTranslation(m_pos);
	//回転行列
	Math::Matrix rMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angle));
	//行列の合成(S * R * T)
	m_mWorld = rMat * tMat;
}

void Titan::PostUpdate()
{
	CharacterBase::PostUpdate();
	ChangeAnime();
}

void Titan::PreDraw()
{
	EnemyBase::PreDraw();
	std::shared_ptr<HPBar>spHPBar = m_wpHPBar.lock();
	if (spHPBar)
	{
		Math::Vector2 pos = { 0,300 };
		spHPBar->Set2DPos(pos);
		spHPBar->SetSize(3);
		spHPBar->SetDrawFlg(true);
	}


	std::shared_ptr<AttackGage>spAttackGauge = m_wpAttackGauge.lock();
	if (spAttackGauge)
	{
		Math::Vector2 pos = { 0,250 };
		spAttackGauge->Set2DPos(pos);
		spAttackGauge->SetSize(3);
		spAttackGauge->SetDrawFlg(true);
	}
}

void Titan::GenerateDepthMapFromLight()
{
	if (m_spCharaModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spCharaModel, m_mWorld);
	}
}

void Titan::DrawLit()
{
	if (m_spCharaModel)
	{

		UAEffectShaderManager::Instance().WriteCBCircleEffectEnable(false);
		UAEffectShaderManager::Instance().WriteCBBoxEffectEnable(false);

		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spCharaModel, m_mWorld);


		UAEffectShaderManager::Instance().WriteCBCircleEffectEnable(true);
		UAEffectShaderManager::Instance().WriteCBBoxEffectEnable(true);


	}
}

//=============================================
// その他の処理
//=============================================

void Titan::OnAttackHit(float _damage, float _knockbackDistance, const Math::Vector3& _knockbackDir, float _hitStunTime, bool _isCritical, float _ignoreRate, const Math::Vector3* _hitPos)
{
	CharacterBase::OnAttackHit(_damage, _knockbackDistance, _knockbackDir, _hitStunTime, _isCritical, _ignoreRate, _hitPos);
}

//=============================================
// 基本の処理
//=============================================

void Titan::Release()
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

//=============================================
// その他の処理
//=============================================

void Titan::ChangeAnime()
{
	if (!m_spAnimator) { return; }
	if (!m_animeChangeFlg) { return; }

	//攻撃中は変更しない
	if (m_attackFlg && !m_isDead) { return; }

	switch (m_enemyAnimeMode)
	{
	case EnemyBase::EnemyAnimeMode_Idle:
		m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_titanAnimeName.m_idleAnime), 0.2f, true);
		break;
	case EnemyBase::EnemyAnimeMode_Walk:
		m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_titanAnimeName.m_walkAnime), 0.2f, true);
		break;
	case EnemyBase::EnemyAnimeMode_Run:
		m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_titanAnimeName.m_runAnime), 0.2f, true);
		break;
	case EnemyBase::EnemyAnimeMode_Attack:
		//攻撃のアニメは段を始めるとき(EnemyBase::StartStep)に切り替える
		break;
	case EnemyBase::EnemyAnimeMode_Die:
		m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_titanAnimeName.m_dieAnime), 0.2f, false);
		break;
	default:
		break;
	}

	m_animeChangeFlg = false;
}

void Titan::UpdateFootstepSE()
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
