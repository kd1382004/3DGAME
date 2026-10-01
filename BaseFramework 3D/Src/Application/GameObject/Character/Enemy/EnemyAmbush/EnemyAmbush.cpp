#include "EnemyAmbush.h"
#include"../../../Terrains/Map/MapManager.h"

void EnemyAmbush::Init()
{
	wanderRadius = 15;

	m_isMovingToTarget = false;
}

void EnemyAmbush::Update()
{
	if (m_isDead) { return; }


	if (!m_playerChaseFlg)
	{
		m_status.moveSpeed.nowSpeed = m_status.moveSpeed.baseSpeed + m_status.moveSpeed.walkMovePowe;
		if (m_returnSpawnPosFlg)
		{
			//スポーン地点に戻る
			ReturnSpawnPos();
		}
		else
		{
			//徘徊
			Wander();
		}
	}
	else
	{
		m_status.moveSpeed.nowSpeed = m_status.moveSpeed.baseSpeed + m_status.moveSpeed.runMovePowe;
		PlayerChase();
	}



	AngeleUpdate();



	//座標行列を作る
	Math::Matrix tMat = Math::Matrix::CreateTranslation(m_pos);
	//回転行列
	Math::Matrix rMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angle));
	//行列の合成(S * R * T)
	m_mWorld = rMat * tMat;

	if (m_pDebugWire)
	{
		m_pDebugWire->AddDebugSphere(GetPos(), 1.0f);
		m_pDebugWire->AddDebugSphere(m_spawnPos, wanderRadius, { 1,1,1,1 });
	}

	if (!m_playerChaseFlg)
	{
		//プレイヤーが視界にいるかどうか
		SearchPlayer();
	}

}

void EnemyAmbush::Wander()
{
	if (!m_isMovingToTarget)
	{
		if (m_stayTime > 0)
		{
			m_stayTime -= DeltaTime::Instance().GetGameDeltaTime();
			m_moveVec = Math::Vector3::Zero;
			return;
		}

		std::shared_ptr<MapManager> spMapManager = m_wpMapManager.lock();
		if (!spMapManager) return;

		// 目的地に向かっていない場合、wanderRadius 内の歩行可能ノードをランダムに選んで A* 経路探索を行う
		Node* startNode = spMapManager->WorldToNode(m_pos);
		Node* goalNode = nullptr;

		// スポーン位置から wanderRadius 内にある歩行可能なノードを探す (最大 10 回試行)
		for (int i = 0; i < 10; i++)
		{
			Math::Vector3 nextDir = Math::Vector3(KdRandom::GetFloat(-1.0f, 1.0f), 0.0f, KdRandom::GetFloat(-1.0f, 1.0f));
			if (nextDir.LengthSquared() > 0.0001f)
			{
				nextDir.Normalize();
			}

			float distance = KdRandom::GetFloat(0.0f, wanderRadius);
			Math::Vector3 candidatePos = m_spawnPos + (nextDir * distance);

			Node* node = spMapManager->WorldToNode(candidatePos);
			if (node && node->walkable)
			{
				goalNode = node;
				break;
			}
		}

		if (startNode && goalNode && startNode != goalNode)
		{
			m_path = spMapManager->FindPath(startNode, goalNode);
			if (!m_path.empty())
			{
				m_pathIndex = 0;
				m_targetPos = spMapManager->NodeToWorld(m_path.back());
				m_isMovingToTarget = true;
				m_moveTimeoutTimer = m_moveTimeoutMax;

				// アニメーション
				m_AnimeChangeFlg = true;
				m_enemyAnimeMode = EnemyAnimeMode::EnemyAnimeMode_Walk;
				return;
			}
		}

		// 経路が見つからなかった場合は短時間待機して再試行
		m_stayTime = 0.5f;
	}
	else
	{
		std::shared_ptr<MapManager> spMapManager = m_wpMapManager.lock();

		// 経路に沿ってノードごとに移動する
		if (spMapManager && !m_path.empty() && m_pathIndex < static_cast<int>(m_path.size()))
		{
			Node* targetNode = m_path[m_pathIndex];
			Math::Vector3 nodeWorldPos = spMapManager->NodeToWorld(targetNode);

			Math::Vector3 toNode = nodeWorldPos - m_pos;
			toNode.y = 0;

			// 進行方向との内積
			float dot = toNode.Dot(m_moveVec);
			bool nearF = (toNode.Length() < 0.5f);
			bool passed = (m_pathIndex > 0 && dot < 0.0f);

			if (nearF || passed)
			{
				m_pathIndex++;
				if (m_pathIndex < static_cast<int>(m_path.size()))
				{
					targetNode = m_path[m_pathIndex];
					nodeWorldPos = spMapManager->NodeToWorld(targetNode);
					toNode = nodeWorldPos - m_pos;
					toNode.y = 0;
				}
			}

			if (toNode.LengthSquared() > 0.0001f)
			{
				toNode.Normalize();
				m_moveVec = toNode;
				m_pos += m_moveVec * m_status.moveSpeed.nowSpeed * DeltaTime::Instance().GetGameDeltaTime();
			}
		}

		m_moveTimeoutTimer -= DeltaTime::Instance().GetGameDeltaTime();

		// 目的地（最終ノード）到達またはタイムアウト時の処理
		if (m_pathIndex >= static_cast<int>(m_path.size()) || m_moveTimeoutTimer <= 0.0f)
		{
			m_isMovingToTarget = false;
			m_path.clear();
			m_pathIndex = 0;
			m_stayTime = m_arrivalWaitTime;

			// アニメーション
			m_AnimeChangeFlg = true;
			m_enemyAnimeMode = EnemyAnimeMode::EnemyAnimeMode_Idel;
		}

		// デバッグ描画
		if (m_pDebugWire && spMapManager)
		{
			for (size_t i = 0; i < m_path.size(); i++)
			{
				Math::Vector3 pos = spMapManager->NodeToWorld(m_path[i]);
				m_pDebugWire->AddDebugLine(pos, Math::Vector3(0, 1, 0), 5, { 0, 1, 1, 1 });
			}
		}
	}
}


