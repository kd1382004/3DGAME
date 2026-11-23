#include "EnemyBase.h"
#include"../../Camera/CameraBase.h"
#include"../../Weapon/ComboStep/ComboStepSaveLoad.h"
#include"Attack/AttackShape.h"
#include"../../../Scene/SceneManager.h"
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

	//実際に減ったHPを知るために、当たる前のHPを覚えておく(ひるみ用)
	const float hpBefore = m_status.m_HP.m_nowHP;

	CharacterBase::OnAttackHit(_damage, _knockbackDistance, _knockbackDir, _hitStunTime, _isCritical, _ignoreRate, _hitPos);

	//ひるみ：パターン攻撃中に、最大HPの m_flinchRate 以上のダメージを受けたら攻撃を止める
	if (m_attackFlg && !m_isDead && m_flinchRate > 0.0f)
	{
		m_flinchValue += std::max(0.0f, hpBefore - m_status.m_HP.m_nowHP);
		if (m_flinchValue >= m_status.m_HP.m_maxHP * m_flinchRate)
		{
			m_flinchValue = 0.0f;
			EndPattern();	//待ち時間とクールタイムは普通に入る
		}
	}

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
	//パターン攻撃の当たり判定が残っていたら消す
	CancelPattern();

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

	//m_cancelRootMotion の段は、アニメ自体の前後の動き(腰の骨の x・z)を打ち消す
	//(Giant の AttackJumpSlamAnimeUpdate と同じ。上下の動きは残す)
	if (m_attackFlg && !m_rootMotionNodeName.empty())
	{
		const AttackStepExtra* extra = GetNowStepExtra();
		if (extra && extra->m_cancelRootMotion)
		{
			auto* rootNode = m_spCharaModel->FindWorkNode(m_rootMotionNodeName);
			if (rootNode)
			{
				Math::Vector3 trans = rootNode->m_localTransform.Translation();
				trans.x = 0;
				trans.z = 0;
				rootNode->m_localTransform.Translation(trans);
			}
		}
	}

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

int EnemyBase::SelectPattern(float _distance, bool _ignoreCooldown)
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
		if (!_ignoreCooldown && list.m_cooldownTimer > 0)
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
	const std::string hitShape = _j.value("m_hitShape", std::string("Bone"));
	if (hitShape == "Circle") { extra.m_hitShape = AttackHitShape::Circle; }
	else if (hitShape == "Box") { extra.m_hitShape = AttackHitShape::Box; }
	extra.m_hitRadius = _j.value("m_hitRadius", extra.m_hitRadius);
	extra.m_hitForward = _j.value("m_hitForward", extra.m_hitForward);

	// 箱の大きさの半分 [横, 高さ, 奥行き]
	if (_j.contains("m_hitBoxSize") && _j["m_hitBoxSize"].is_array() && _j["m_hitBoxSize"].size() == 3)
	{
		const nlohmann::json& size = _j["m_hitBoxSize"];
		extra.m_hitBoxSize = { size[0].get<float>(), size[1].get<float>(), size[2].get<float>() };
	}
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

void EnemyBase::ReloadAttackPatterns()
{
	//パターンを使わない敵は何もしない
	if (m_patternFilePath.empty()) { return; }

	//攻撃中なら終わらせてから読み直す(番号がずれないように)
	if (m_attackFlg)
	{
		EndPattern();
	}

	LoadAttackPatterns();
}

void EnemyBase::UpdatePatternAttack()
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
			int patternIndex = SelectPattern(distance);

			//クールタイムのせいでこの距離の技が全部使えないときは、クールタイムを無視してもう一度選ぶ
			//(ゲージがたまったのに何もしない、をなくす。待ち時間があるので連発にはならない)
			if (patternIndex < 0)
			{
				patternIndex = SelectPattern(distance, true);
			}

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

void EnemyBase::StartPattern(int _patternIndex)
{
	if (_patternIndex < 0 || _patternIndex >= (int)m_patternList.size()) { return; }

	m_nowPatternIndex = _patternIndex;
	m_attackFlg = true;
	m_flinchValue = 0.0f;	//ひるみ値はパターンごとに数え直す
	m_enemyAnimeMode = EnemyBase::EnemyAnimeMode_Attack;

	//敵ごとの始めの処理(叫び声など)
	OnPatternStart();

	StartStep(0);
}

void EnemyBase::StartStep(int _stepIndex)
{
	FaceToPlayer();

	m_nowStepIndex = _stepIndex;
	m_hitOn = false;
	m_stepForwardDone = 0.0f;

	const ComboStep* step = GetNowStep();
	const AttackStepExtra* extra = GetNowStepExtra();
	if (!step || !extra)
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

	//イベントを「まだ出していない」に戻す
	m_eventDone.assign(extra->m_events.size(), false);

	//飛びかかりの始まりと着地点
	m_leapStartPos = m_pos;
	m_leapTargetPos = m_playerPos;

	//前の段の当たり判定が残っていたら消して、新しく作る
	//(段ごとに作り直すと「もう当てた」がリセットされて、連撃が全部当たる)
	if (std::shared_ptr<AttackShape> spOld = m_wpPatternAttack.lock())
	{
		spOld->FinishAttack();
	}

	std::shared_ptr<AttackShape> spAttack = std::make_shared<AttackShape>();
	m_wpPatternAttack = spAttack;
	spAttack->SetShape(extra->m_hitShape);
	spAttack->SetRadius(extra->m_hitRadius);
	spAttack->SetBoxSize(extra->m_hitBoxSize);
	spAttack->SetPos(GetStepHitCenter());
	spAttack->SetAngleY(m_angle);
	spAttack->SetPlayer(m_wpPlayer.lock());
	SceneManager::Instance().AddObject(spAttack);
}

void EnemyBase::UpdateStep()
{
	const ComboStep* step = GetNowStep();
	const AttackStepExtra* extra = GetNowStepExtra();
	if (!step || !extra)
	{
		EndPattern();
		return;
	}

	const float progress = GetNowStepProgress();

	//踏み込み(m_stepForward が0なら何もしない)
	UpdateStepForward(progress);

	//飛びかかり(LeapToTarget のときだけ)
	UpdateStepLeap(progress);

	//一品物の攻撃(m_specialType が入っている段だけ)
	if (!extra->m_specialType.empty())
	{
		UpdateSpecialStep(extra->m_specialType, progress);
	}

	std::shared_ptr<AttackShape> spAttack = m_wpPatternAttack.lock();
	if (spAttack)
	{
		//当たり判定の位置と向き
		spAttack->SetPos(GetStepHitCenter());
		spAttack->SetAngleY(m_angle);
		spAttack->SetKnockback(m_patternKnockbackDistance * step->m_knockbackRate, m_mWorld.Backward());

		//予告(Circle・Box)：m_warnStart から当たり判定が終わるまで
		const bool warn = (extra->m_hitShape != AttackHitShape::Bone) &&
			progress >= extra->m_warnStart && progress <= step->m_hitEnd;
		spAttack->SetWarnFlg(warn);

		//当たり判定を出す
		if (progress > step->m_hitStart && !m_hitOn)
		{
			m_hitOn = true;
			spAttack->SetAttackPower(m_status.m_attack.m_nowAttack * step->m_damageRate);
			spAttack->SetAttackFlg(true);
		}

		//当たり判定を止める(判定の物自体は次の段か CancelPattern で消す)
		if (progress > step->m_hitEnd)
		{
			spAttack->SetAttackFlg(false);
		}
	}

	//エフェクト・音・画面の揺れ
	UpdateStepEvents(progress);

	//最後の段はアニメの終わりで終了、それ以外は m_cancelPoint で次の段へ
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

void EnemyBase::EndPattern()
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

void EnemyBase::CancelPattern()
{
	//当たり判定が残っていたら消す
	if (std::shared_ptr<AttackShape> spAttack = m_wpPatternAttack.lock())
	{
		spAttack->FinishAttack();
	}

	m_attackFlg = false;
	m_hitOn = false;
	m_nowPatternIndex = -1;
	m_nowStepIndex = -1;
	m_animeSpeedRate = 1.0f;

	//アニメ・待ち時間・クールタイムはここでは触らない(EndPattern との違い)
}

Math::Vector3 EnemyBase::GetStepHitCenter() const
{
	const AttackStepExtra* extra = GetNowStepExtra();
	if (!extra) { return m_pos; }

	//骨に付ける
	if (extra->m_hitShape == AttackHitShape::Bone)
	{
		const std::string hitNode = GetNowHitNode();
		if (!hitNode.empty() && m_spCharaModel)
		{
			auto* node = m_spCharaModel->FindNode(hitNode);
			if (node)
			{
				return (node->m_worldTransform * m_mWorld).Translation();
			}
		}
		return m_pos;
	}

	//飛びかかりの円は着地点に出す
	if (extra->m_moveType == AttackMoveType::LeapToTarget)
	{
		Math::Vector3 target = m_leapTargetPos;
		target.y = m_pos.y;
		return target;
	}

	//円・箱は敵の前 m_hitForward に出す
	const float rad = DirectX::XMConvertToRadians(m_angle);
	const Math::Vector3 forward = { std::sin(rad), 0.0f, std::cos(rad) };
	return m_pos + forward * extra->m_hitForward;
}

void EnemyBase::UpdateStepEvents(float _progress)
{
	const AttackStepExtra* extra = GetNowStepExtra();
	if (!extra) { return; }

	//エフェクト・画面の揺れは当たり判定の中心(地面の高さ)に出す
	Math::Vector3 center = GetStepHitCenter();
	center.y = m_pos.y;

	for (int i = 0; i < (int)extra->m_events.size() && i < (int)m_eventDone.size(); i++)
	{
		const AttackEvent& ev = extra->m_events[i];
		if (m_eventDone[i] || _progress < ev.m_progress) { continue; }
		m_eventDone[i] = true;

		switch (ev.m_type)
		{
		case AttackEventType::Effect:
			if (!ev.m_name.empty())
			{
				KdEffekseerManager::GetInstance().Play(ev.m_name, center, 2.0f, 1.0f, false);
			}
			break;
		case AttackEventType::Sound:
			if (!ev.m_name.empty())
			{
				std::shared_ptr<KdSoundInstance3D> spSE = KdAudioManager::Instance().Play3D(ev.m_name, center, false);
				if (spSE)
				{
					spSE->SetCurveDistanceScaler(20.0f);
					spSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
				}
			}
			break;
		case AttackEventType::CameraShake:
			CameraShakeAt(center, ev.m_time, ev.m_power, 30.0f);
			break;
		default:
			break;
		}
	}
}

void EnemyBase::UpdateStepLeap(float _progress)
{
	const AttackStepExtra* extra = GetNowStepExtra();
	if (!extra || extra->m_moveType != AttackMoveType::LeapToTarget) { return; }

	//m_aimLockEnd までは、着地点をプレイヤーに合わせ続ける
	if (_progress < extra->m_aimLockEnd)
	{
		m_leapTargetPos = m_playerPos;
	}

	//m_moveStart〜m_moveEnd の間に、跳んだ位置から着地点へ動く(高さは重力に任せる)
	if (_progress < extra->m_moveStart) { return; }

	const float moveLength = std::max(extra->m_moveEnd - extra->m_moveStart, 0.01f);
	const float t = std::clamp((_progress - extra->m_moveStart) / moveLength, 0.0f, 1.0f);

	Math::Vector3 pos = m_leapStartPos + (m_leapTargetPos - m_leapStartPos) * t;
	pos.y = m_pos.y;
	m_pos = pos;

	//着地点の方を向く
	Math::Vector3 dir = m_leapTargetPos - m_leapStartPos;
	dir.y = 0;
	if (dir.LengthSquared() > 0.0001f)
	{
		dir.Normalize();
		m_moveVec = dir;
		m_angle = DirectX::XMConvertToDegrees(atan2(dir.x, dir.z));
		if (m_angle < 0.0f) { m_angle += 360.0f; }
	}
}
