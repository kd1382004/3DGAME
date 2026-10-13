#include "TitelMove.h"
#include "../../GameObject/Terrains/Map/MapManager.h"

void TitelMove::Update()
{
	std::shared_ptr<MapManager> spMap = m_wpMapManager.lock();
	if (!spMap) { return; }

	m_pos.y -= m_Gravity;
	m_Gravity += m_gravityPower;

	if (m_moveFlg)
	{
		// 1. 経路が未計算の場合、m_nextPosへの経路を探索
		if (m_path.empty())
		{
			Node* startNode = spMap->WorldToNode(m_pos);
			Node* goalNode = spMap->WorldToNode(m_nextPos);

			if (startNode && goalNode && startNode != goalNode)
			{
				m_path = spMap->FindPath(startNode, goalNode);
				m_pathIndex = 0;

				// スタートノードは現在位置のノードなので、すでに近接/通過している場合は次のノードから開始
				if (m_path.size() > 1)
				{
					Math::Vector3 p0 = spMap->NodeToWorld(m_path[0]);
					Math::Vector3 toP0 = p0 - m_pos;
					toP0.y = 0.0f;
					if (toP0.Length() < m_nodeNearDistance)
					{
						m_pathIndex = 1;
					}
				}
			}

			// 経路が見つからなかった、または既に同じノードにいる場合
			if (m_path.empty())
			{
				m_moveFlg = false;
				m_nowWati = m_watiMax;
				m_moveVec = Math::Vector3::Zero;
				return;
			}
		}

		// 2. 経路に沿ってノードごとに移動（通過済みノードはすべてスキップして前方のノードを探す）
		while (m_pathIndex < static_cast<int>(m_path.size()))
		{
			Node* targetNode = m_path[m_pathIndex];
			Math::Vector3 targetPos = spMap->NodeToWorld(targetNode);

			Math::Vector3 toNode = targetPos - m_pos;
			toNode.y = 0.0f;

			float dist = toNode.Length();
			float dot = (m_moveVec.LengthSquared() > 0.0001f) ? toNode.Dot(m_moveVec) : 1.0f;

			bool isNear = (dist < m_nodeNearDistance);
			bool isPassed = (m_pathIndex > 0 && dot < 0.0f); // 進行方向に対して背後になった

			// 近づいたか、通り過ぎている場合は次のノードへ進める
			if (isNear || isPassed)
			{
				m_pathIndex++;
			}
			else
			{
				break; // 前方の未到達ノードが見つかったら探索終了
			}
		}

		// 前方のターゲットノードへ向かって移動ベクトルを設定
		if (m_pathIndex < static_cast<int>(m_path.size()))
		{
			Node* targetNode = m_path[m_pathIndex];
			Math::Vector3 targetPos = spMap->NodeToWorld(targetNode);
			Math::Vector3 toNode = targetPos - m_pos;
			toNode.y = 0.0f;

			if (toNode.LengthSquared() > 0.0001f)
			{
				toNode.Normalize();
				m_moveVec = toNode;
			}
		}
		else
		{
			// 全ノード到達（目的地到着）
			m_moveFlg = false;
			m_nowWati = m_watiMax;
			m_path.clear();
			m_pathIndex = 0;
			m_moveVec = Math::Vector3::Zero;
		}
	}
	else
	{
		m_moveVec = Math::Vector3::Zero;
		m_nowWati -= DeltaTime::Instance().GetGameDeltaTime();
		if (m_nowWati < 0.0f)
		{
			m_moveFlg = true;
			// ※ 次の目的地(m_nextPos)を設定してから移動を開始する場合はここに記述
			SetNextPos(spMap->GetRandomPos());
		}
	}


	m_pos += m_moveVec * m_speed * DeltaTime::Instance().GetGameDeltaTime();
	SetPos(m_pos);
}

void TitelMove::PostUpdate()
{
	CharacterBase::PostUpdate();
}
