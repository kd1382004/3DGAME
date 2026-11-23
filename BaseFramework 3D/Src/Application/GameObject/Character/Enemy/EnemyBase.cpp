#include "EnemyBase.h"
#include"../../Camera/CameraBase.h"
#include"../../Weapon/ComboStep/ComboStepSaveLoad.h"
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

#include"../../DropCoin/DropCoin.h"

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

		// 止まっているときは「走り出す距離」、走っているときは「止まる距離」と比べる
		bool isStopped = (m_enemyAnimeMode == EnemyAnimeMode::EnemyAnimeMode_Idle);
		float stopDist = isStopped ? m_playerChaseResumeDir : m_playerChaseDir;

		if (stopDist > dir.Length())
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



	//---- コインを落とす処理 ----

	// LV1 で m_coin、1上がるごとに +15%
	float coinScale = 1.0f + (m_status.m_LV - 1) * 0.15f;
	int totalCoin = static_cast<int>(std::round(m_coin * coinScale));  // 合計額

	// 0コインなら何も出さない
	if (totalCoin <= 0) { return; }

	const int baseCoinVal = 10;   // LV1 のときの1枚あたりの額

	// 1枚あたりの額も同じ倍率で増やす → 枚数はレベルに関係なくほぼ一定
	int coinVal = std::max(1, static_cast<int>(std::round(baseCoinVal * coinScale)));
	int coinNum = std::max(1, totalCoin / coinVal);
	// 枚数が決まったので、割り切れなかった余りを出す
	int remainder = totalCoin - coinVal * coinNum;

	for (int i = 0; i < coinNum; i++)
	{
		std::shared_ptr<DropCoin> spDropCoin = std::make_shared<DropCoin>();
		spDropCoin->Init();
		spDropCoin->SetPos(m_pos);
		// 余りは最後の1枚に足して、合計がずれないようにする
		spDropCoin->SetValue(i == coinNum - 1 ? coinVal + remainder : coinVal);
		spDropCoin->SetPlayer(m_wpPlayer.lock());
		SceneManager::Instance().AddObject(spDropCoin);
	}

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

	//攻撃中は段の m_speed の速さで再生する(攻撃していなければ1)
	m_spAnimator->AdvanceTime(m_spCharaModel->WorkNodes(), 60 * m_animeSpeedRate);
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

bool EnemyBase::LoadAttackPatterns()
{
	m_patternList.clear();
	m_nowPatternIndex = -1;
	m_nowStepIndex = -1;
	m_lastPatternIndex = -1;

	// パスが空ならパターンを使わない敵
	if (m_patternFilePath.empty()) { return false; }

	std::ifstream file(m_patternFilePath);
	if (!file)
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] パターン一覧が開けません : %s\n"), m_patternFilePath.c_str());
		return false;
	}

	nlohmann::json data = nlohmann::json::parse(file, nullptr, false);
	if (data.is_discarded() || !data.is_object() || !data.contains("m_patterns") || !data["m_patterns"].is_array())
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] パターン一覧の形が違います : %s\n"), m_patternFilePath.c_str());
		return false;
	}

	for (const nlohmann::json& j : data["m_patterns"])
	{
		if (!j.is_object()) { continue; }

		AttackPattern pattern;
		pattern.m_name = j.value("m_name", std::string());
		pattern.m_comboFilePath = j.value("m_comboFilePath", std::string());
		pattern.m_minLV = j.value("m_minLV", 1);
		pattern.m_minDistance = j.value("m_minDistance", 0.0f);
		pattern.m_maxDistance = j.value("m_maxDistance", 5.0f);
		pattern.m_weight = j.value("m_weight", 1.0f);
		pattern.m_cooldown = j.value("m_cooldown", 0.0f);
		pattern.m_waitAfter = j.value("m_waitAfter", 0.0f);

		// 段の並びはコンボファイルから読む
		ComboFileData combo;
		if (!ComboStepSaveLoad::Load(pattern.m_comboFilePath, combo) || combo.m_steps.empty())
		{
			KdDebugGUI::Instance().AddLog(U8("[Error] パターン %s : コンボファイルが読めないので飛ばします : %s\n"),
				pattern.m_name.c_str(), pattern.m_comboFilePath.c_str());
			continue;
		}
		pattern.m_steps = combo.m_steps;
		const int stepNum = (int)pattern.m_steps.size();

		// 段ごとの当たり判定の骨
		if (j.contains("m_hitNodes") && j["m_hitNodes"].is_array())
		{
			for (const nlohmann::json& node : j["m_hitNodes"])
			{
				pattern.m_hitNodes.push_back(node.is_string() ? node.get<std::string>() : std::string());
			}
		}

		// 段ごとの特別な設定
		if (j.contains("m_stepExtras") && j["m_stepExtras"].is_array())
		{
			for (const nlohmann::json& extra : j["m_stepExtras"])
			{
				pattern.m_stepExtras.push_back(StepExtraFromJson(extra));
			}
		}

		// 段の数に合わせる(足りない骨は1つ前の段の骨、足りない設定は普通の単発)
		pattern.m_hitNodes.resize(stepNum);
		for (int i = 1; i < stepNum; i++)
		{
			if (pattern.m_hitNodes[i].empty()) { pattern.m_hitNodes[i] = pattern.m_hitNodes[i - 1]; }
		}
		pattern.m_stepExtras.resize(stepNum);

		// 打ち間違いの確認(見つからなくても読み込みは続ける)
		if (m_spCharaModel)
		{
			for (int i = 0; i < stepNum; i++)
			{
				const std::string& anime = pattern.m_steps[i].m_animeName;
				if (!m_spCharaModel->GetAnimation(anime))
				{
					KdDebugGUI::Instance().AddLog(U8("[Warning] パターン %s %d段目 : アニメ %s がモデルにありません\n"),
						pattern.m_name.c_str(), i + 1, anime.c_str());
				}

				const std::string& node = pattern.m_hitNodes[i];
				if (!node.empty() && !m_spCharaModel->FindNode(node))
				{
					KdDebugGUI::Instance().AddLog(U8("[Warning] パターン %s %d段目 : 骨 %s がモデルにありません\n"),
						pattern.m_name.c_str(), i + 1, node.c_str());
				}
			}
		}

		KdDebugGUI::Instance().AddLog(U8("パターン %s : %d段 (LV%d〜)\n"), pattern.m_name.c_str(), stepNum, pattern.m_minLV);
		m_patternList.push_back(pattern);
	}

	return !m_patternList.empty();
}

void EnemyBase::UpdatePatternCooldown()
{

	float dt = DeltaTime::Instance().GetGameDeltaTime();

	for (auto& list : m_patternList)
	{
		list.m_cooldownTimer -= dt;

		if (list.m_cooldownTimer <= 0)
		{
			list.m_cooldownTimer = 0;
		}
	}
}

int EnemyBase::SelectPattern(float _distance)
{
	std::vector<int> Candidate;

	int i = -1;
	for (const auto& list : m_patternList)
	{
		i++;
		if (list.m_weight <= 0)
		{
			continue;
		}
		if (list.m_minLV > GetLV())
		{
			continue;
		}
		if (_distance < list.m_minDistance || _distance > list.m_maxDistance)
		{
			continue;
		}
		if (list.m_cooldownTimer > 0)
		{
			continue;
		}

		Candidate.push_back(i);
	}

	//候補が2つ以上あるなら、前に使ったパターンは外す(同じ技が続かないように)
	if (Candidate.size() >= 2)
	{
		Candidate.erase(std::remove(Candidate.begin(), Candidate.end(), m_lastPatternIndex), Candidate.end());
	}

	if (Candidate.size() == 0)
	{
		return -1;
	}


	float weight = 0;
	for (int i = 0;i < Candidate.size();i++)
	{
		const int Index = Candidate[i];
		weight += m_patternList[Index].m_weight;
	}
	float weightMax = KdRandom::GetFloat(0, weight);
	for (int i = 0;i < Candidate.size();i++)
	{
		const int Index = Candidate[i];
		weightMax -= m_patternList[Index].m_weight;

		if (weightMax <= 0)
		{
			return Index;
		}
	}


	return Candidate[Candidate.size() - 1];
}

const AttackPattern* EnemyBase::GetNowPattern() const
{
	if (m_nowPatternIndex < 0 || m_nowPatternIndex >= (int)m_patternList.size()) { return nullptr; }
	return &m_patternList[m_nowPatternIndex];
}

const ComboStep* EnemyBase::GetNowStep() const
{
	const AttackPattern* pattern = GetNowPattern();
	if (!pattern) { return nullptr; }
	if (m_nowStepIndex < 0 || m_nowStepIndex >= (int)pattern->m_steps.size()) { return nullptr; }
	return &pattern->m_steps[m_nowStepIndex];
}

const AttackStepExtra* EnemyBase::GetNowStepExtra() const
{
	const AttackPattern* pattern = GetNowPattern();
	if (!pattern) { return nullptr; }
	if (m_nowStepIndex < 0 || m_nowStepIndex >= (int)pattern->m_stepExtras.size()) { return nullptr; }
	return &pattern->m_stepExtras[m_nowStepIndex];
}

std::string EnemyBase::GetNowHitNode() const
{
	const AttackPattern* pattern = GetNowPattern();
	if (!pattern) { return std::string(); }
	if (m_nowStepIndex < 0 || m_nowStepIndex >= (int)pattern->m_hitNodes.size()) { return std::string(); }
	return pattern->m_hitNodes[m_nowStepIndex];
}

float EnemyBase::GetNowStepProgress() const
{
	if (!m_spAnimator) { return 0.0f; }

	const float length = m_spAnimator->GetMaxLength();
	if (length <= 0.0f) { return 0.0f; }

	return m_spAnimator->GetTime() / length;
}

bool EnemyBase::IsLastStep() const
{
	const AttackPattern* pattern = GetNowPattern();
	if (!pattern) { return true; }
	return m_nowStepIndex >= (int)pattern->m_steps.size() - 1;
}

AttackStepExtra EnemyBase::StepExtraFromJson(const nlohmann::json& _j)
{
	AttackStepExtra extra;
	if (!_j.is_object()) { return extra; }

	// 動き方
	const std::string moveType = _j.value("m_moveType", std::string("None"));
	if (moveType == "Step") { extra.m_moveType = AttackMoveType::Step; }
	else if (moveType == "LeapToTarget") { extra.m_moveType = AttackMoveType::LeapToTarget; }

	extra.m_aimLockEnd = _j.value("m_aimLockEnd", extra.m_aimLockEnd);
	extra.m_moveStart = _j.value("m_moveStart", extra.m_moveStart);
	extra.m_moveEnd = _j.value("m_moveEnd", extra.m_moveEnd);
	extra.m_cancelRootMotion = _j.value("m_cancelRootMotion", extra.m_cancelRootMotion);

	// 当たり判定の形
	if (_j.value("m_hitShape", std::string("Bone")) == "Circle") { extra.m_hitShape = AttackHitShape::Circle; }
	extra.m_hitRadius = _j.value("m_hitRadius", extra.m_hitRadius);
	extra.m_warnStart = _j.value("m_warnStart", extra.m_warnStart);

	// イベント
	if (_j.contains("m_events") && _j["m_events"].is_array())
	{
		for (const nlohmann::json& e : _j["m_events"])
		{
			if (!e.is_object()) { continue; }

			AttackEvent ev;
			ev.m_progress = e.value("p", 0.0f);

			const std::string type = e.value("type", std::string("Effect"));
			if (type == "Sound") { ev.m_type = AttackEventType::Sound; }
			else if (type == "CameraShake") { ev.m_type = AttackEventType::CameraShake; }

			ev.m_name = e.value("name", std::string());
			ev.m_power = e.value("power", 0.0f);
			ev.m_time = e.value("time", 0.0f);
			extra.m_events.push_back(ev);
		}
	}

	extra.m_specialType = _j.value("m_specialType", std::string());
	return extra;
}

void EnemyBase::FaceToPlayer()
{
	Math::Vector3 dir = m_playerPos - m_pos;
	dir.y = 0;

	//プレイヤーと同じ位置なら向きを変えない
	if (dir.LengthSquared() < 0.0001f) { return; }

	dir.Normalize();

	//前が (sin, 0, cos) になる角度(度)
	m_angle = DirectX::XMConvertToDegrees(atan2(dir.x, dir.z));
	if (m_angle < 0.0f) { m_angle += 360.0f; }

	//AngleUpdate が古い向きに戻さないように
	m_moveVec = dir;
}

void EnemyBase::UpdateStepForward(float _progress)
{
	const ComboStep* step = GetNowStep();
	if (!step) { return; }

	// 踏み込み：段の最初から当たり判定が始まるまでに、m_stepForward だけ前へ進む
	if (step->m_stepForward <= 0.0f) { return; }

	const float endRate = std::max(step->m_hitStart, 0.05f);
	const float target = step->m_stepForward * std::clamp(_progress / endRate, 0.0f, 1.0f);
	const float delta = target - m_stepForwardDone;
	if (delta <= 0.0f) { return; }

	// 向いている方向（m_angle は atan2(x, z) で作っているので、前は (sin, 0, cos)）
	const float rad = DirectX::XMConvertToRadians(m_angle);
	const Math::Vector3 forward = { std::sin(rad), 0.0f, std::cos(rad) };
	const Math::Vector3 nextPos = m_pos + forward * delta;

	// 壁の向こうへは進まない
	if (!RaycastFromTo(nextPos))
	{
		m_pos = nextPos;
	}
	m_stepForwardDone = target;
}
