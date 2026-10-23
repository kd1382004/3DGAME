#include "TitelMove.h"
#include "../../GameObject/Terrains/Map/MapManager.h"

TitelMove::TitelMove()
{

}

void TitelMove::Update()
{
	auto spMap = m_wpMapManager.lock();
	if (!spMap) return;

	const float dt = DeltaTime::Instance().GetGameDeltaTime();

	//========================
	// 重力
	//========================
	m_pos.y -= m_Gravity * dt;
	m_Gravity += m_gravityPower * dt;

	if (m_moveFlg)
	{
		//--------------------------------------
		// 経路未生成ならA*
		//--------------------------------------
		if (m_path.empty())
		{
			Node* startNode = spMap->WorldToNode(m_pos);
			Node* goalNode = spMap->WorldToNode(m_nextPos);

			if (startNode &&
				goalNode &&
				startNode != goalNode)
			{
				m_path = spMap->FindPath(startNode, goalNode);
				m_pathIndex = 0;

				// 現在ノードを飛ばす
				if (m_path.size() > 1)
				{
					Math::Vector3 p0 =
						spMap->NodeToWorld(m_path[0]);

					Math::Vector3 toP0 = p0 - m_pos;
					toP0.y = 0;

					if (toP0.Length() < m_nodeNearDistance)
					{
						m_pathIndex = 1;
					}
				}
			}

			if (m_path.empty())
			{
				m_moveFlg = false;
				m_nowWati = m_watiMax;
				m_moveVec = Math::Vector3::Zero;
				return;
			}
		}

		//--------------------------------------
		// 経路追従
		//--------------------------------------
		while (m_pathIndex < (int)m_path.size())
		{
			Node* targetNode = m_path[m_pathIndex];

			Math::Vector3 targetPos =
				spMap->NodeToWorld(targetNode);

			Math::Vector3 toNode =
				targetPos - m_pos;

			toNode.y = 0;

			float dist = toNode.Length();

			float dot = 1.0f;

			if (
				m_moveVec.LengthSquared() > 0.0001f &&
				toNode.LengthSquared() > 0.0001f)
			{
				Math::Vector3 dirToNode = toNode;
				dirToNode.Normalize();

				Math::Vector3 moveDir = m_moveVec;
				moveDir.Normalize();

				dot = dirToNode.Dot(moveDir);
			}

			bool isNear =
				(dist < m_nodeNearDistance);

			bool isPassed =
				(m_pathIndex > 0 && dot < -0.2f);

			if (isNear || isPassed)
			{
				m_pathIndex++;
			}
			else
			{
				break;
			}
		}

		//--------------------------------------
		// 次ノードへ移動
		//--------------------------------------
		if (m_pathIndex < (int)m_path.size())
		{
			Node* targetNode =
				m_path[m_pathIndex];

			Math::Vector3 targetPos =
				spMap->NodeToWorld(targetNode);

			Math::Vector3 moveDir =
				targetPos - m_pos;

			moveDir.y = 0;

			if (moveDir.LengthSquared() > 0.0001f)
			{
				moveDir.Normalize();
				m_moveVec = moveDir;
			}
		}
		else
		{
			//--------------------------------------
			// 到着
			//--------------------------------------
			m_moveFlg = false;
			m_nowWati = m_watiMax;

			m_moveVec = Math::Vector3::Zero;

			m_path.clear();
			m_pathIndex = 0;
		}
	}
	else
	{
		m_moveVec = Math::Vector3::Zero;

		m_nowWati -= dt;

		if (m_nowWati <= 0.0f)
		{
			m_moveFlg = true;

			SetNextPos(
				spMap->GetRandomPos());

			m_path.clear();
			m_pathIndex = 0;
		}
	}

	//--------------------------------------
	// 移動
	//--------------------------------------
	m_pos += m_moveVec * m_speed * dt;

	SetPos(m_pos);
}

void TitelMove::PostUpdate()
{
	CharacterBase::PostUpdate();
}
