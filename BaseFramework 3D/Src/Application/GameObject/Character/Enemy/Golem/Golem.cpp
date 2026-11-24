//---- 自分のヘッダー ----
#include "Golem.h"								//敵(ゴーレム)

//---- シーン ----
#include"../../../../Scene/SceneManager.h"		//シーンの管理

//---- カメラ ----
#include"../../../Camera/CameraBase.h"			//カメラの基底クラス

//=============================================
// 基本の処理
//=============================================

void Golem::Init()
{
	EnemyAmbush::Init();
	m_wanderRadius = 50;
	m_charaDefaultStatusFilePath = "Asset/Data/ObjeData/Character/Enemy/Golem/Status/Status.json";
	m_patternFilePath = "Asset/Data/ObjeData/EnemyAttackPattern/Golem.json";
	if (!m_spCharaModel)
	{
		m_spCharaModel = std::make_shared<KdModelWork>();
		m_spCharaModel->SetModelData("Asset/Models/Character/Enemy/Golem/golem_clean.gltf");
		//アニメータの準備
		m_spAnimator = std::make_shared<KdAnimator>();
		m_spAnimator->SetAnimation(m_spCharaModel->GetAnimation(m_golemAnimeName.m_walkAnime), true);



		if (!m_pCollider)
		{
			m_pCollider = std::make_unique<KdCollider>();

			// 歩きのポーズでノード行列を計算してから読む（基本姿勢だと腕を広げた形のため）
			m_spAnimator->AdvanceTime(m_spCharaModel->WorkNodes());
			m_spCharaModel->CalcNodeMatrices();

			auto addSphere = [&](std::string_view _nodeName, UINT _type)
				{
					const KdModelWork::Node* node = m_spCharaModel->FindNode(_nodeName);
					if (!node) { return; }
					const Math::Matrix& m = node->m_worldTransform;
					// エンプティの位置 = 球の中心、エンプティのスケール = 半径
					m_pCollider->RegisterCollisionShape(_nodeName, m.Translation(), m.Right().Length(), _type);
				};

			const UINT bodyType = KdCollider::TypeDamage | KdCollider::TypeCameraOcclusion;
			addSphere("Col_Bump", KdCollider::TypeBump);
			addSphere("Col_Legs", bodyType);
			addSphere("Col_Waist", bodyType);
			addSphere("Col_Chest", bodyType);
			addSphere("Col_Head", bodyType);
		}

		EnemyBase::Init();

		m_viewAngle = 100;

		//---------- 3D効果音 ----------
		auto loadSE = [&](std::shared_ptr<KdSoundInstance3D>& _spSE, const std::string& _path)
			{
				if (_spSE) { return; }
				_spSE = KdAudioManager::Instance().Play3D(_path, m_pos, false);
				if (_spSE)
				{
					_spSE->Stop();
					_spSE->SetCurveDistanceScaler(25.0f);
				}
			};
		loadSE(m_spWalk3DSE, "Asset/Sound/SE/Enemy/Golem/Walk.wav");
		loadSE(m_spCrying3DSE, "Asset/Sound/SE/Enemy/Golem/CryingSound.wav");
	}

	m_playerChaseDir = 3;
	m_playerChaseResumeDir = 3.5f;
	m_material = EnemyMaterial::Stone;

	//攻撃パターン
	m_attackWaitNowMax = m_attackWaitMax;
	LoadAttackPatterns();
}

void Golem::Update()
{

	if (m_isDead)
	{
		if (!m_spAnimator) { return; }

		// 死亡時に出しっぱなしの攻撃判定を必ず消す(再生速度も戻る)
		CancelPattern();

		if (m_spAnimator->IsAnimationEnd())
		{
			m_isExpired = true;
		}
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
}

void Golem::PreUpdate()
{
	EnemyBase::PreUpdate();


	HPPosPostUpdate();
}

void Golem::Release()
{
	//攻撃の当たり判定が残っていたら消す
	CancelPattern();

	if (m_spWalk3DSE) { m_spWalk3DSE->Stop(); m_spWalk3DSE = nullptr; }
	if (m_spCrying3DSE) { m_spCrying3DSE->Stop(); m_spCrying3DSE = nullptr; }
}

//=============================================
// その他の処理
//=============================================

void Golem::UpdateFootstepSE()
{
	// 死んでいる・攻撃中は足音を止める
	if (m_isDead || m_attackFlg)
	{
		if (m_spWalk3DSE) { m_spWalk3DSE->Stop(); }
		if (m_isDead && m_spCrying3DSE) { m_spCrying3DSE->Stop(); }
		return;
	}

	if (!m_spWalk3DSE) { return; }

	// 歩き・走りのときだけ、鳴り終わるたびに次の1歩を鳴らす
	bool isMove = (m_enemyAnimeMode == EnemyAnimeMode::EnemyAnimeMode_Walk ||
		m_enemyAnimeMode == EnemyAnimeMode::EnemyAnimeMode_Run);

	if (isMove)
	{
		m_spWalk3DSE->SetPos(m_pos);
		m_spWalk3DSE->SetVolume(VOLInfo::Instance().GetMSSEVol() * 0.5f);
		if (!m_spWalk3DSE->IsPlaying())
		{
			m_spWalk3DSE->Play();
		}
	}
	else
	{
		m_spWalk3DSE->Stop();
	}
}

void Golem::ChangeAnime()
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
		m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_golemAnimeName.m_idleAnime), 0.2f, true);
		break;
	case EnemyBase::EnemyAnimeMode_Walk:
		m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_golemAnimeName.m_walkAnime), 0.2f, true);
		break;
	case EnemyBase::EnemyAnimeMode_Run:
		m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_golemAnimeName.m_dashAnime), 0.2f, true);
		break;
	case EnemyBase::EnemyAnimeMode_Attack:
		//攻撃のアニメは段を始めるとき(EnemyBase::StartStep)に切り替える
		break;
	case EnemyBase::EnemyAnimeMode_Die:
		m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_golemAnimeName.m_dieAnime), 0.2f, false);
		break;
	default:
		break;
	}

	m_animeChangeFlg = false;
}

void Golem::OnPatternStart()
{
	// 攻撃を始めるときのうなり声
	if (m_spCrying3DSE)
	{
		m_spCrying3DSE->SetPos(m_pos);
		m_spCrying3DSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
		m_spCrying3DSE->Play(false);
	}
}
