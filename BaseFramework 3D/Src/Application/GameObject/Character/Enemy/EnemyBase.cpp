#include "EnemyBase.h"
#include"../../Camera/CameraBase.h"

#include"../../../Scene/GameScene/GameScene.h"

//プレイヤー
#include"../Player/PlayerBase.h"

//マップ
#include"../../Terrains/Map/MapManager.h"

//UI
#include"../../UI/UIManager.h"
#include"../../UI/HPBar/HPBar.h"
#include"../../UI/AttackGage/AttackGage.h"


#include"EnemyCrowdController/EnemyCrowdController.h"

void EnemyBase::Init()
{
	if (!m_spCrowdController)
	{
		m_spCrowdController = std::make_shared<EnemyCrowdController>();
	}



	LoadCharaStatus(m_charaDefaultStatusFilePath);
	m_baseStatus = m_status;

	if (!m_spCharaModel) { return; }

	auto spData = m_spCharaModel->GetData();

	if (!spData) { return; }

	const auto& meshNodeIndices = spData->GetDrawMeshNodeIndices();

	if (meshNodeIndices.empty()) { return; }

	// 全メッシュの頂点をまとめる（複数メッシュ対応）
	std::vector<Math::Vector3> allPositions;
	for (int nodeIndex : meshNodeIndices)
	{
		const auto& node = spData->GetOriginalNodes()[nodeIndex];
		if (!node.m_spMesh) { continue; }
		const auto& positions = node.m_spMesh->GetVertexPositions();
		allPositions.insert(allPositions.end(), positions.begin(), positions.end());
	}

	if (allPositions.empty()) { return; }

	// ローカル空間の OBB を生成
	DirectX::BoundingOrientedBox localOBB;
	DirectX::BoundingOrientedBox::CreateFromPoints(
		localOBB,
		allPositions.size(),
		allPositions.data(),
		sizeof(Math::Vector3)
	);

	m_frustumBox = KdCollider::BoxInfo(0, localOBB);
}

void EnemyBase::PreUpdate()
{
	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayer.lock();
	if (spPlayer)
	{
		m_playerPos = spPlayer->GetPos();
	}
}

void EnemyBase::Update()
{



}

void EnemyBase::PostUpdate()
{
	CharacterBase::PostUpdate();
	EnemyAnimeModeUpdate();



	//if (!m_isInView) { return; }
	//// プレイヤーとの距離が一定以内の場合のみ、またはボスのみライトを追加
	//Math::Vector3 diff = m_playerPos - m_pos;
	//if (diff.LengthSquared() < 25.0f * 25.0f) // 25m以内のみ
	//{
	//	KdShaderManager::Instance().WorkAmbientController().AddPointLight(
	//		{ 10, 10, 10 },
	//		10,
	//		m_pos + Math::Vector3(0, 1, 0)
	//	);
	//}
}

void EnemyBase::PreDraw()
{
	//カメラに映ってるかどうか
	std::shared_ptr<CameraBase>spCamera = m_wpCamera.lock();
	if (spCamera)
	{
		m_isInView = CheckInScreen(spCamera->GetBoundingFrustum(), m_frustumBox);


		std::shared_ptr<HPBar>spHPBar = m_wpHPBar.lock();
		if (!spHPBar) { return; }

		std::shared_ptr<AttackGage>spAttackGauge = m_wpAttackGauge.lock();
		if (!spAttackGauge) { return; }

		if (m_isDead)
		{
			spHPBar->SetDrawFlg(false);
			spAttackGauge->SetDrawFlg(false);
			return;
		}


		if (m_isInView)
		{
			Math::Vector3 dist = m_playerPos - m_pos;

			if (dist.Length() < 20)
			{
				spHPBar->SetDrawFlg(true);
				spAttackGauge->SetDrawFlg(true);
			}
			else
			{
				spHPBar->SetDrawFlg(false);
				spAttackGauge->SetDrawFlg(false);
			}
		}
		else
		{
			spHPBar->SetDrawFlg(false);
			spAttackGauge->SetDrawFlg(false);
		}
	}
}

void EnemyBase::SetSpawnPos(const Math::Vector3& _pos)
{

	m_spawnPos = _pos;
	m_pos = _pos;



	std::shared_ptr<MapManager>spMapManager = m_wpMapManager.lock();

	if (spMapManager)
	{
		Node* spawnNode = spMapManager->WorldToNode(m_spawnPos);
		if (spawnNode)
		{
			m_spawnPos = spMapManager->NodeToWorld(spawnNode);
		}
	}


	m_mWorld.Translation(m_pos);
}

void EnemyBase::AddUIList(std::shared_ptr<UIManager> _spUIManager)
{
	if (_spUIManager)
	{
		std::shared_ptr<HPBar>spHPBar = std::make_shared<HPBar>();
		spHPBar->Init();
		spHPBar->SetLV(m_status.m_LV);
		m_wpHPBar = spHPBar;
		_spUIManager->AddUIObj(spHPBar);



		std::shared_ptr<AttackGage>spAttackGauge = std::make_shared<AttackGage>();
		spAttackGauge->Init();
		m_wpAttackGauge = spAttackGauge;
		_spUIManager->AddUIObj(spAttackGauge);
		SetAttackGaugePercent(1);
	}


	HPPosPostUpdate();
}

void EnemyBase::SetLV(int _lv)
{

	_lv += KdRandom::GetInt(-1, 1);

	m_status.m_LV = std::max(1, _lv);
	ApplyLevelStatus();
	auto spHPBar = m_wpHPBar.lock();
	if (spHPBar)
	{
		spHPBar->SetLV(m_status.m_LV);
		spHPBar->SetHPBarTexPercent(m_status.m_HP.m_nowHP / m_status.m_HP.m_maxHP);
	}
}

void EnemyBase::SearchPlayer()
{
	Math::Vector3 dir = m_mWorld.Backward();
	dir.Normalize();

	Math::Vector3 toPlayer = (m_playerPos - m_pos);
	toPlayer.Normalize();

	float dot = dir.Dot(toPlayer);

	float cosView = cosf(DirectX::XMConvertToRadians(m_viewAngle * 0.5f));

	if (dot > cosView)
	{
		// 視界に入っている
		float dist = Math::Vector3::Distance(m_pos, m_playerPos);

		if (dist < m_viewDistance)
		{
			// 敵の目線からプレイヤーへの遮蔽物（壁）チェック
			Math::Vector3 eyePos = m_pos + Math::Vector3(0, 1.0f, 0);
			Math::Vector3 toPlayerDir = (m_playerPos + Math::Vector3(0, 1.0f, 0)) - eyePos;
			float playerDist = toPlayerDir.Length();
			toPlayerDir.Normalize();
			KdCollider::RayInfo sightRay(
				KdCollider::TypeBump | KdCollider::TypeCameraOcclusion,
				eyePos,
				toPlayerDir,
				playerDist
			);
			bool isBlocked = false;
			for (auto& wpObj : m_wpHitObjectList)
			{
				if (auto spObj = wpObj.lock())
				{
					if (spObj->Intersects(sightRay, nullptr))
					{
						isBlocked = true; // 壁で遮られている！
						break;
					}
				}
			}
			// 壁で遮られていない時だけ追跡開始
			if (!isBlocked)
			{
				m_playerChaseFlg = true;
				
				//アニメーション
				m_animeChangeFlg = true;
				m_enemyAnimeMode = EnemyAnimeMode::EnemyAnimeMode_Run;
			}
		}
	}

	if (m_pDebugWire)
	{
		m_pDebugWire->AddDebugLine(m_pos, dir, m_viewDistance);
	}
}

void EnemyBase::PlayerChase()
{
	float dist = (m_playerPos - m_pos).Length();

	if (dist > m_loseSightDistance)
	{

		if (m_stayTime > 0)
		{

			if (m_stayTime == m_lostSightWaitTime)
			{
				//アニメーション
				m_animeChangeFlg = true;
				m_enemyAnimeMode = EnemyAnimeMode::EnemyAnimeMode_Idle;
			}


			m_stayTime -= DeltaTime::Instance().GetGameDeltaTime();
			return;
		}
		else
		{
			m_playerChaseFlg = false;
			m_returnSpawnPosFlg = true;

			//アニメーション
			m_animeChangeFlg = true;
			m_enemyAnimeMode = EnemyAnimeMode::EnemyAnimeMode_Walk;
		}
	}
	else
	{
		if (m_stayTime != m_lostSightWaitTime)
		{
			//アニメーション
			m_animeChangeFlg = true;
			m_enemyAnimeMode = EnemyAnimeMode::EnemyAnimeMode_Run;
		}


		m_stayTime = m_lostSightWaitTime;
	}



	if (m_pDebugWire)
	{
		m_pDebugWire->AddDebugLine(m_pos, m_moveVec, m_loseSightDistance);
	}

	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayer.lock();
	if (spPlayer)
	{
		spPlayer->SetIsDetectedByEnemy(m_playerChaseFlg);
	}


	if (m_playerChaseFlg)
	{
		if (!m_spCrowdController) { return; }

		Math::Vector3 dir = m_playerPos - m_pos;

		if (m_playerChaseDir > dir.Length())
		{

			if (m_enemyAnimeMode == EnemyAnimeMode::EnemyAnimeMode_Run)
			{

				m_animeChangeFlg = true;
				m_enemyAnimeMode = EnemyAnimeMode::EnemyAnimeMode_Idle;
			}
			return;
		}
		else
		{
			if (m_enemyAnimeMode == EnemyAnimeMode::EnemyAnimeMode_Idle)
			{

				m_animeChangeFlg = true;
				m_enemyAnimeMode = EnemyAnimeMode::EnemyAnimeMode_Run;
			}
		}

		m_spCrowdController->Update(this, DeltaTime::Instance().GetGameDeltaTime());
		m_pos += m_moveVec * m_status.m_moveSpeed.m_nowSpeed * DeltaTime::Instance().GetGameDeltaTime();
	}

}

void EnemyBase::OnAttackHit(float _damage, float _knockbackDistance, const Math::Vector3& _knockbackDir, float _hitStunTime, bool _isCritical, float _ignoreRate, const Math::Vector3* _hitPos)
{
	m_playerChaseFlg = true;

	CharacterBase::OnAttackHit(_damage, _knockbackDistance, _knockbackDir, _hitStunTime, _isCritical, _ignoreRate, _hitPos);

	// エフェクトを出す位置：当たった位置。分からないときはキャラの座標
	const Math::Vector3 effectPos = _hitPos ? *_hitPos : m_pos;

	switch (m_material)
	{
	case EnemyMaterial::Flesh:
		KdEffekseerManager::GetInstance().Play("Hit/Material/Flesh/Hit_Flesh.efkefc", effectPos, 3.0f, 1.0f, false);
		break;
	case EnemyMaterial::Stone:
		KdEffekseerManager::GetInstance().Play("Hit/Material/Stone/Hit_Stone.efkefc", effectPos, 3.0f, 1.0f, false);
		break;
	case EnemyMaterial::Num:
		break;
	default:
		break;
	}

	// 当たった音（質感ごと。武器の当たる音に重ねて鳴らす）
	PlayMaterialSE("Asset/Sound/SE/Hit/", 0.7f);

	

}

void EnemyBase::Release()
{
	std::shared_ptr<HPBar>spHPBar = m_wpHPBar.lock();
	if (spHPBar)
	{
		spHPBar->Delete();
	}

	std::shared_ptr<AttackGage>spAttackGauge = m_wpAttackGauge.lock();
	if (spAttackGauge)
	{
		spAttackGauge->Delete();
	}
}

void EnemyBase::SetDead()
{
	m_animeChangeFlg = true;
	m_enemyAnimeMode = EnemyAnimeMode::EnemyAnimeMode_Die;


	std::shared_ptr<GameScene>spGameScene = m_wpGameScene.lock();
	if (spGameScene)
	{
		spGameScene->AddEnemyKill();
	}


	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayer.lock();
	if (spPlayer)
	{
		spPlayer->AddExp(10);
	}

	switch (m_material)
	{
	case EnemyMaterial::Flesh:
		KdEffekseerManager::GetInstance().Play("Death/Flesh/Death_Flesh.efkefc", m_pos, 3.0f, 1.0f, false);
		break;
	case EnemyMaterial::Stone:
		KdEffekseerManager::GetInstance().Play("Death/Stone/Death_Stone.efkefc", m_pos, 3.0f, 1.0f, false);
		break;
	case EnemyMaterial::Num:
		break;
	default:
		break;
	}

	// 倒れた音（質感ごと）
	PlayMaterialSE("Asset/Sound/SE/Enemy/Death/", 1.0f);
}

void EnemyBase::SetAttackGaugePercent(float _percent)
{
	std::shared_ptr<AttackGage>spAttackGauge = m_wpAttackGauge.lock();
	if (!spAttackGauge) { return; }
	spAttackGauge->SetAttackGaugeTexPercent(_percent);
}

void EnemyBase::ApplyLevelStatus()
{
	if (m_status.m_LV <= 1) { return; }

	// 階層（LV）ごとにステータスを強化
	float hpScale = 1.0f + (m_status.m_LV - 1) * 0.20f;
	float attackScale = 1.0f + (m_status.m_LV - 1) * 0.1f;
	float defenseAdd = static_cast<float>(m_status.m_LV - 1) * 0.3f;

	m_status.m_HP.m_maxHP = std::round(m_baseStatus.m_HP.m_baseHP * hpScale);
	m_status.m_HP.m_nowHP = m_status.m_HP.m_maxHP;

	m_status.m_attack.m_baseAttackPower = std::round(m_baseStatus.m_attack.m_baseAttackPower * attackScale);
	m_status.m_attack.m_nowAttack = m_status.m_attack.m_baseAttackPower + m_status.m_attack.m_addAttack;

	m_status.m_defense.m_baseDefensePower += defenseAdd;
	m_status.m_defense.m_nowDefense = m_status.m_defense.m_baseDefensePower + m_status.m_defense.m_addDefense;
}

void EnemyBase::EnemyAnimeModeUpdate()
{
	if (!m_spAnimator) { return; }
	if (!m_spCharaModel) { return; }

	m_spAnimator->AdvanceTime(m_spCharaModel->WorkNodes(), 60);
	m_spCharaModel->CalcNodeMatrices();
}

void EnemyBase::HPPosPostUpdate()
{

	Math::Vector3 set2DPos;

	std::shared_ptr<CameraBase>spCamera = m_wpCamera.lock();
	if (!spCamera) { return; }

	Math::Vector3 set3DPos = m_pos;
	set3DPos.y += 5;
	spCamera->GetCamera()->ConvertWorldToScreenDetail(set3DPos, set2DPos);

	std::shared_ptr<HPBar>spHPBar = m_wpHPBar.lock();
	if (spHPBar)
	{

		set2DPos.y += 40;
		std::shared_ptr<CameraBase>spCamera = m_wpCamera.lock();
		spHPBar->Set2DPos({ set2DPos.x,set2DPos.y });
	}


	//攻撃ゲージ
	std::shared_ptr<AttackGage>spAttackGauge = m_wpAttackGauge.lock();
	if (spAttackGauge)
	{
		set2DPos.y -= 20;
		spAttackGauge->Set2DPos({ set2DPos.x,set2DPos.y });
	}
}

float EnemyBase::CalcSightLightDistance(const Math::Vector3& _eyePos, const Math::Vector3& _dir, float _maxDist, float _halfAngleDeg)
{
	float hitDist = _maxDist;

	// レイを飛ばす方向（正面、左右の視野方向）
	std::vector<Math::Vector3> rayDirs;
	rayDirs.push_back(_dir); // 正面

	// 左右の視野端へのレイ（壁に対して斜めの時にも突き抜けを防ぐ）
	float rad = DirectX::XMConvertToRadians(_halfAngleDeg * 0.7f);
	Math::Matrix rotL = Math::Matrix::CreateRotationY(rad);
	Math::Matrix rotR = Math::Matrix::CreateRotationY(-rad);
	rayDirs.push_back(Math::Vector3::TransformNormal(_dir, rotL));
	rayDirs.push_back(Math::Vector3::TransformNormal(_dir, rotR));

	for (const auto& rDir : rayDirs)
	{
		KdCollider::RayInfo ray(
			KdCollider::TypeBump | KdCollider::TypeCameraOcclusion,
			_eyePos,
			rDir,
			_maxDist
		);

		for (auto& wpObj : m_wpHitObjectList)
		{
			if (auto spObj = wpObj.lock())
			{
				std::list<KdCollider::CollisionResult> results;
				if (spObj->Intersects(ray, &results))
				{
					for (const auto& ret : results)
					{
						float d = _maxDist - ret.m_overlapDistance;
						if (d < hitDist)
						{
							hitDist = d;
						}
					}
				}
			}
		}
	}

	return std::max(0.1f, hitDist);
}

void EnemyBase::ReturnSpawnPos()
{
	// スポーン地点に到達しているか判定
	Math::Vector3 toSpawn = m_spawnPos - m_pos;
	toSpawn.y = 0;
	if (toSpawn.Length() < m_wanderRadius)
	{
		m_returnSpawnPosFlg = false;
		m_path.clear();
		m_pathIndex = 0;
		return;
	}

	std::shared_ptr<MapManager> spMapManager = m_wpMapManager.lock();
	if (spMapManager)
	{
		m_repathTimer -= DeltaTime::Instance().GetGameDeltaTime();

		// 経路が空、末尾到達、またはタイマー満了で経路再計算
		if (m_repathTimer <= 0.0f || m_path.empty() || m_pathIndex >= m_path.size())
		{
			m_repathTimer = 0.3f;
			Node* start = spMapManager->WorldToNode(m_pos);
			Node* goal = spMapManager->WorldToNode(m_spawnPos);

			if (start && goal)
			{
				m_path = spMapManager->FindPath(start, goal);
				m_pathIndex = 0;
			}
		}

		if (!m_path.empty() && m_pathIndex < m_path.size())
		{
			Node* targetNode = m_path[m_pathIndex];
			Math::Vector3 targetPos = spMapManager->NodeToWorld(targetNode);

			Math::Vector3 dir = targetPos - m_pos;
			dir.y = 0;
			float distToNode = dir.Length();

			// ノード方向ベクトル
			Math::Vector3 toNode = targetPos - m_pos;
			toNode.y = 0;

			// 敵の進行方向とノード方向の内積
			float dot = toNode.Dot(m_moveVec);

			// 1. 距離が近い
			bool nearF = false;

			if (toNode.Length() < 0.5f)
			{
				nearF = true;
			}

			// 2. ノードを通り過ぎた（進行方向と逆向きになった）
			bool passed = false;
			if (dot < 0.0f)
			{
				passed = true;
			}

			if (nearF || passed)
			{
				m_pathIndex++;

				if (m_pathIndex < m_path.size())
				{
					targetNode = m_path[m_pathIndex];
					targetPos = spMapManager->NodeToWorld(targetNode);

					dir = targetPos - m_pos;
					dir.y = 0;
				}
			}

			if (dir.LengthSquared() > 0.0001f)
			{
				dir.Normalize();
				m_moveVec = dir; // 進行方向を更新
				m_pos += m_moveVec * m_status.m_moveSpeed.m_nowSpeed * DeltaTime::Instance().GetGameDeltaTime();
			}
		}
		else
		{
			// ノードが見つからない場合はスポーン地点に向かって直線移動（フォールバック）
			if (toSpawn.LengthSquared() > 0.0001f)
			{
				toSpawn.Normalize();
				m_moveVec = toSpawn;
				m_pos += m_moveVec * m_status.m_moveSpeed.m_nowSpeed * DeltaTime::Instance().GetGameDeltaTime();
			}
		}
	}

	if (m_pDebugWire)
	{
		m_pDebugWire->AddDebugLine(m_pos, m_moveVec, m_loseSightDistance);


		// ノード位置に点を描く
		for (auto* node : m_path)
		{
			Math::Vector3 pos = spMapManager->NodeToWorld(node);

			// 小さな点（短い線）
			m_pDebugWire->AddDebugLine(pos, Math::Vector3(0, 1, 0), 10, { 0,0,0,1 });
		}

		// ノード間の線を描く
		for (size_t i = 0; i < m_path.size() - 1; i++)
		{
			Math::Vector3 a = spMapManager->NodeToWorld(m_path[i]);
			Math::Vector3 b = spMapManager->NodeToWorld(m_path[i + 1]);

			m_pDebugWire->AddDebugLine(a, b - a, (b - a).Length(), { 0,1,0,1 });
		}
	}
}

void EnemyBase::PlayMaterialSE(const std::string& _folder, float _volRate)
{
	// 質感ごとのファイル名（Flesh.wav / Stone.wav）
	std::string file;
	switch (m_material)
	{
	case EnemyMaterial::Flesh:	file = "Flesh.wav";	break;
	case EnemyMaterial::Stone:	file = "Stone.wav";	break;
	default:					return;
	}

	// 1回鳴らすだけなので持っておかない（鳴り終わったら KdAudioManager が片付ける）
	std::shared_ptr<KdSoundInstance3D> spSE = KdAudioManager::Instance().Play3D(_folder + file, m_pos, false);
	if (spSE)
	{
		spSE->SetCurveDistanceScaler(20.0f);
		spSE->SetVolume(VOLInfo::Instance().GetMSSEVol() * _volRate);
	}
}
