#include "EnemyBase.h"
#include"../../Camera/CameraBase.h"

//プレイヤー
#include"../Player/PlayerBase.h"

//マップ
#include"../../Terrains/Map/MapManager.h"

//UI
#include"../../UI/UIManager.h"
#include"../../UI/HPBar/HPBar.h"
void EnemyBase::Init()
{

	LoadCharaStatus(m_charaStatusFilePath);

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
	HPPosPostUpdate();
}

void EnemyBase::PreDraw()
{
	//カメラに映ってるかどうか
	std::shared_ptr<CameraBase>spCamera = m_wpCamera.lock();
	if (spCamera)
	{
		m_isInView = CheckInScreen(spCamera->GetBoundingFrustum(), m_frustumBox);


		std::shared_ptr<HPBar>spHPBar = m_wpHPBar.lock();
		if (spHPBar)
		{
			if (m_isInView)
			{
				Math::Vector3 dist = m_playerPos - m_pos;

				if (dist.Length() < 20)
				{
					spHPBar->SetDrawFlg(true);
				}
				else 
				{
					spHPBar->SetDrawFlg(false);
				}
			}
			else
			{
				spHPBar->SetDrawFlg(false);
			}



		}
	}
}

void EnemyBase::SetSpawnPos(const Math::Vector3& _pos)
{

	m_spawnPos = _pos;
	m_pos = _pos;
	std::shared_ptr<MapManager>m_spMapManager = m_wpMapManager.lock();

	if (m_spMapManager)
	{
		Node* spawnNode = m_spMapManager->WorldToNode(m_spawnPos);
		if (spawnNode)
		{
			m_spawnPos = m_spMapManager->NodeToWorld(spawnNode);
		}
	}



}

void EnemyBase::SearchPlayer()
{
	Math::Vector3 dir = m_moveVec;
	Math::Vector3 toPlayer = (m_playerPos - m_pos);
	toPlayer.Normalize();

	float dot = dir.Dot(toPlayer);

	float cosView = cosf(m_viewAngle * 0.5f);

	if (dot > cosView)
	{
		// 視界に入っている
		float dist = Math::Vector3::Distance(m_pos, m_playerPos);

		if (dist < m_viewDistance)
		{
			m_playerChaseFlg = true;

			//アニメーション
			m_AnimeChangeFlg = true;
			m_enemyAnimeMode = EnemyAnimeMode::EnemyAnimeMode_Run;

		}
	}

	if (m_pDebugWire)
	{
		m_pDebugWire->AddDebugLine(m_pos, dir, m_viewDistance);
	}



}

void EnemyBase::PlayerChase()
{
	m_moveVec = m_playerPos - m_pos;
	m_moveVec.y = 0;
	m_moveVec.Normalize();


	float dist = (m_playerPos - m_pos).Length();

	if (dist > m_loseSightDistance)
	{

		if (m_stayTime > 0)
		{

			if (m_stayTime == m_lostSightWaitTime)
			{
				//アニメーション
				m_AnimeChangeFlg = true;
				m_enemyAnimeMode = EnemyAnimeMode::EnemyAnimeMode_Idel;
			}


			m_stayTime -= DeltaTime::Instance().GetGameDeltaTime();
			return;
		}
		else
		{
			m_playerChaseFlg = false;
			m_returnSpawnPosFlg = true;

			//アニメーション
			m_AnimeChangeFlg = true;
			m_enemyAnimeMode = EnemyAnimeMode::EnemyAnimeMode_Walk;
		}
	}
	else
	{
		if (m_stayTime != m_lostSightWaitTime)
		{
			//アニメーション
			m_AnimeChangeFlg = true;
			m_enemyAnimeMode = EnemyAnimeMode::EnemyAnimeMode_Run;
		}


		m_stayTime = m_lostSightWaitTime;

		//Chase範囲内なら座標更新

		if (dist > 1)
		{
			m_pos += m_moveVec * m_status.moveSpeed.nowSpeed * DeltaTime::Instance().GetGameDeltaTime();
		}



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

}

void EnemyBase::AddUIList(std::shared_ptr<UIManager> _spUIManager)
{
	if (_spUIManager)
	{
		std::shared_ptr<HPBar>spHPBar = std::make_shared<HPBar>();
		spHPBar->Init();
		m_wpHPBar = spHPBar;
		_spUIManager->AddUIObj(spHPBar);

	}
}

void EnemyBase::Release()
{
	std::shared_ptr<HPBar>spHPBar = m_wpHPBar.lock();
	if (spHPBar)
	{
		spHPBar->Delete();
	}
}

void EnemyBase::EnemyAnimeModeUpdate()
{
	if (!m_spAnimetor) { return; }
	if (!m_spCharaModel) { return; }
	if (!m_isInView) { return; }

	m_spAnimetor->AdvanceTime(m_spCharaModel->WorkNodes(), 60);
	m_spCharaModel->CalcNodeMatrices();
}

void EnemyBase::HPPosPostUpdate()
{
	std::shared_ptr<HPBar>spHPBar = m_wpHPBar.lock();
	if (spHPBar)
	{
		Math::Vector3 set2DPos;

		std::shared_ptr<CameraBase>spCamera = m_wpCamera.lock();
		if (spCamera)
		{
			Math::Vector3 set3DPos = m_pos;
			set3DPos.y += 5;
			spCamera->GetCamera()->ConvertWorldToScreenDetail(set3DPos, set2DPos);
		}
		spHPBar->Set2DPos({ set2DPos.x,set2DPos.y });

	}
}


void EnemyBase::ReturnSpawnPos()
{
	// スポーン地点に到達しているか判定
	Math::Vector3 toSpawn = m_spawnPos - m_pos;
	toSpawn.y = 0;
	if (toSpawn.Length() < wanderRadius)
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
				m_pos += m_moveVec * m_status.moveSpeed.nowSpeed * DeltaTime::Instance().GetGameDeltaTime();
			}
		}
		else
		{
			// ノードが見つからない場合はスポーン地点に向かって直線移動（フォールバック）
			if (toSpawn.LengthSquared() > 0.0001f)
			{
				toSpawn.Normalize();
				m_moveVec = toSpawn;
				m_pos += m_moveVec * m_status.moveSpeed.nowSpeed * DeltaTime::Instance().GetGameDeltaTime();
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

void EnemyBase::SetDead()
{
	m_AnimeChangeFlg = true;
	m_enemyAnimeMode = EnemyAnimeMode::EnemyAnimeMode_Die;
}
