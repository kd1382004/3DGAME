#include "TitelMove.h"
#include "../../GameObject/Terrains/Map/MapManager.h"

void TitelMove::Update()
{
	std::shared_ptr<MapManager> spMap = m_wpMapManager.lock();
	if (!spMap) { return; }

	m_pos.y -= m_Gravity;
	m_Gravity += m_gravityPower;

	m_moveVec = {};

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
			}

			// 経路が見つからなかった、または既に同じノードにいる場合
			if (m_path.empty())
			{
				m_moveFlg = false;
				m_nowWati = m_watiMax;
				return;
			}
		}

		// 2. 経路に沿ってノードごとに移動
		if (m_pathIndex < static_cast<int>(m_path.size()))
		{
			Node* targetNode = m_path[m_pathIndex];
			Math::Vector3 targetPos = spMap->NodeToWorld(targetNode);

			Math::Vector3 toNode = targetPos - m_pos;
			toNode.y = 0.0f;

			float dot = toNode.Dot(m_moveVec);
			bool isNear = (toNode.Length() < 0.5f);
			bool isPassed = (m_pathIndex > 0 && dot < 0.0f);

			// ノードに近づいたか、通り過ぎた場合は次のノードへ
			if (isNear || isPassed)
			{
				m_pathIndex++;
				if (m_pathIndex < static_cast<int>(m_path.size()))
				{
					targetNode = m_path[m_pathIndex];
					targetPos = spMap->NodeToWorld(targetNode);
					toNode = targetPos - m_pos;
					toNode.y = 0.0f;
				}
			}

			// 移動処理
			if (toNode.LengthSquared() > 0.0001f)
			{
				toNode.Normalize();
				m_moveVec = toNode;
			}

			// 3. 最終ノードに到達した時の処理
			if (m_pathIndex >= static_cast<int>(m_path.size()))
			{
				m_moveFlg = false;
				m_nowWati = m_watiMax;
				m_path.clear();
				m_pathIndex = 0;
			}
		}
		else
		{
			m_moveFlg = false;
			m_nowWati = m_watiMax;
			m_path.clear();
			m_pathIndex = 0;
		}
	}
	else
	{
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
