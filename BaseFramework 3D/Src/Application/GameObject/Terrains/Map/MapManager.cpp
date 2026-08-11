#include "MapManager.h"
#include"MapBase.h"
#include"MapGenerate/MapGenerate.h"
#include"../../Camera/CameraBase.h"
#include"../../Character/Player/PlayerBase.h"
#include"../../Character/Enemy/EnemyBase.h"
#include"../../Character/Enemy/EnemyManager.h"

#include"../../UI/UIManager.h"
#include"../../UI/UIMap/UIMapManager.h"
#include"../../UI/UIMap/UIMap_Map/UIMap_Map.h"

void MapManager::Init()
{

}

void MapManager::Update()
{
	for (const auto& mapObj : m_mapObj)
	{
		mapObj->Update();
	}
}

void MapManager::PostUpdate()
{
	/*for (const auto& mapObj : m_mapObj)
	{
		mapObj->PostUpdate();
	}*/
}

void MapManager::DrawLit()
{
	for (const auto& mapObj : m_mapObj)
	{
		mapObj->DrawLit();
	}
}

void MapManager::PreDraw()
{
	for (const auto& mapObj : m_mapObj)
	{
		mapObj->PreDraw();
	}
}

void MapManager::GenerateDepthMapFromLight()
{
	for (const auto& mapObj : m_mapObj)
	{
		mapObj->GenerateDepthMapFromLight();
	}
}

void MapManager::MapHit(std::shared_ptr<KdGameObject> obj)
{
	if (!obj) { return; }

	//あたり判定リストをクリア
	obj->ClearHitObjectList();
	Math::Vector3 objPos = obj->GetPos();
	const float hitCheckDistSq = 15.0f * 15.0f;

	//近くにあるオブジェクトだけを新たに登録
	for (const auto& mapObj : m_mapObj)
	{
		if (!mapObj) continue;
		float distSq = Math::Vector3::DistanceSquared(mapObj->GetPos(), objPos);
		if (distSq <= hitCheckDistSq)
		{
			obj->RegistHitObject(mapObj);
		}
	}
}

void MapManager::MapHitEnemy(std::shared_ptr<EnemyBase> obj)
{
	if (!obj) { return; }

	//あたり判定リストをクリア
	obj->ClearHitObjectList();
	Math::Vector3 objPos = obj->GetPos();
	const float hitCheckDistSq = 15.0f * 15.0f;

	//近くにあるオブジェクトだけを新たに登録
	for (const auto& mapObj : m_mapObj)
	{
		if (!mapObj) continue;
		float distSq = Math::Vector3::DistanceSquared(mapObj->GetPos(), objPos);
		if (distSq <= hitCheckDistSq)
		{
			obj->RegistHitObject(mapObj);
		}
	}
}

void MapManager::SetCamera(std::shared_ptr<CameraBase> _spCamera)
{
	m_wpCamera = _spCamera;

	for (const auto& mapObj : m_mapObj)
	{
		mapObj->SetCamera(_spCamera);

		_spCamera->ResolveCameraOcclusionObject(mapObj);
	}
}

void MapManager::GenerateMap()
{
	m_mapObj.clear();

	std::shared_ptr<MapGenerate>map = std::make_shared<MapGenerate>();

	std::vector<std::vector<int>> mapDate;


	float mapW = 50;
	float mapH = 50;


	Math::Vector3 basePos;
	mapDate = map->Generate({ mapH,mapW }, 50, m_mapTileSiz, MapType::MapType_Grassland, &m_mapObj, &m_playerSpawnPos,&basePos);

	CreateNodeGrid(mapW, mapH,m_mapTileSiz);
	ApplyWalkableFromMap(mapDate);

	if (!m_wpCamera.expired())
	{
		SetCamera(m_wpCamera.lock());
	}

	std::shared_ptr<PlayerBase>spPlayerBase = m_wpPlayerBase.lock();

	if (spPlayerBase)
	{
		for (const auto& mapObj : m_mapObj)
		{
			mapObj->SetPlayer(spPlayerBase);
		}
	}

	//敵の生成
	std::shared_ptr<EnemyManager>spEnemyManager = m_wpEnemyManager.lock();
	if (spEnemyManager)
	{
		const auto& mapRoomList = map->GetRoomInfoList();
		std::random_device rd;
		std::mt19937 mt(rd());

		for (size_t i = 0; i < mapRoomList.size(); i++)
		{
			if (mapRoomList[i].empty()) { continue; }

			int roomEnemyNum = mapRoomList[i][0].m_roomEnemyNum;

			struct EnemySpawnList {
				Math::Vector3 m_pos;
				bool SpawnFlg = false;
			};

			std::vector<EnemySpawnList> enemySpawnList;
			enemySpawnList.reserve(mapRoomList[i].size());

			for (size_t j = 0; j < mapRoomList[i].size(); j++)
			{
				enemySpawnList.push_back({ mapRoomList[i][j].m_pos, false });
			}

			if (enemySpawnList.empty()) continue;

			// 敵数を安全に制限
			roomEnemyNum = std::min(roomEnemyNum, static_cast<int>(enemySpawnList.size()));

			// ランダムシャッフル
			std::shuffle(enemySpawnList.begin(), enemySpawnList.end(), mt);

			// 先頭から必要数だけスポーン
			for (int n = 0; n < roomEnemyNum; n++)
			{
				spEnemyManager->SpawnEnemy(RoomEnemy, enemySpawnList[n].m_pos);
			}
		}
	}



	//UIのマップ生成
	std::shared_ptr<UIManager>spUIManager = m_wpUIManager.lock();
	if (spUIManager)
	{
		

		std::shared_ptr<UIMapManager>spUIMapManager= spUIManager->GetUIMapManager();
		if (spUIMapManager)
		{
			spUIMapManager->SetBase3DPos(basePos);
			spUIMapManager->SetTileSiz(m_mapTileSiz);
			spUIMapManager->GetUIMap_Map()->PosListReset();

			for (const auto& mapObj : m_mapObj)
			{

				if (mapObj->GetMapObjType() == MapObjType::Ground)
				{
					spUIMapManager->GetUIMap_Map()->AddPosList(mapObj->GetPos(), m_mapTileSiz);
				}

				
			}
		}
	}
}

void MapManager::CreateNodeGrid(int width, int height, float tileSize)
{
	m_nodes.resize(height);
	for (int y = 0; y < height; y++)
	{
		m_nodes[y].resize(width);

		for (int x = 0; x < width; x++)
		{
			Node& node = m_nodes[y][x];

			// グリッド座標
			node.pos = Math::Vector2(x, y);

			// 初期値(通れないで初期化)
			node.walkable = false;

			// A* 用の初期化
			node.gCost = FLT_MAX;
			node.hCost = 0;
			node.parent = nullptr;
		}
	}
}

void MapManager::ApplyWalkableFromMap(const std::vector<std::vector<int>>& mapData)
{
	for (int y = 0; y < mapData.size(); y++)
	{
		for (int x = 0; x < mapData[y].size(); x++)
		{
			int tile = mapData[y][x];

			if (tile != (int)MapGenerate::TileType::None)
			{
				m_nodes[y][x].walkable = true;
			}
		}
	}

	int a = 0;
}

Math::Vector3 MapManager::NodeToWorld(const Node* node)
{
	if (!node) return Math::Vector3::Zero;

	float worldX = m_mapTileSiz * node->pos.x + m_mapTileSiz * 0.5f;
	float worldZ = -(m_mapTileSiz * node->pos.y + m_mapTileSiz * 0.5f);

	return Math::Vector3(worldX, 0.0f, worldZ);
}

Node* MapManager::WorldToNode(const Math::Vector3& worldPos)
{
	// X は右へプラス
	int x = (int)floor(worldPos.x / m_mapTileSiz);

	// Z は下へマイナス → -Z がタイル番号
	int y = (int)floor((-worldPos.z) / m_mapTileSiz);

	int width = (int)m_nodes[0].size();
	int height = (int)m_nodes.size();

	if (x < 0 || y < 0 || x >= width || y >= height)
	{
		return nullptr;
	}

	return &m_nodes[y][x];
}

std::vector<Node*> MapManager::FindPath(Node* start, Node* goal)
{

	for (auto& row : m_nodes)
	{
		for (auto& node : row)
		{
			node.gCost = FLT_MAX;
			node.hCost = 0;
			node.parent = nullptr;
		}
	}

	// openList = 探索候補
	std::vector<Node*> openList;

	// closedList = 探索済み
	std::vector<Node*> closedList;

	// 初期化
	start->gCost = 0.0f;
	start->hCost = Heuristic(start, goal);
	start->parent = nullptr;

	openList.push_back(start);

	while (!openList.empty())
	{
		// ★ openList の中で fCost が最小のノードを探す
		Node* current = openList[0];
		for (auto* node : openList)
		{
			if (node->fCost() < current->fCost() ||
				(node->fCost() == current->fCost() && node->hCost < current->hCost))
			{
				current = node;
			}
		}

		// openList から current を削除
		openList.erase(std::remove(openList.begin(), openList.end(), current), openList.end());
		closedList.push_back(current);

		// ★ ゴールに到達したら経路復元
		if (current == goal)
		{
			return BuildPath(goal);
		}

		// ★ 隣接ノードを取得
		auto neighbors = GetNeighbors(current);

		for (auto* neighbor : neighbors)
		{
			// 通れない or 探索済みならスキップ
			if (!neighbor->walkable ||
				std::find(closedList.begin(), closedList.end(), neighbor) != closedList.end())
			{
				continue;
			}

			float newCost = current->gCost + 1.0f; // タイル移動コスト

			// 新しいルートの方が安いなら更新
			if (newCost < neighbor->gCost ||
				std::find(openList.begin(), openList.end(), neighbor) == openList.end())
			{
				neighbor->gCost = newCost;
				neighbor->hCost = Heuristic(neighbor, goal);
				neighbor->parent = current;

				// openList に未登録なら追加
				if (std::find(openList.begin(), openList.end(), neighbor) == openList.end())
				{
					openList.push_back(neighbor);
				}
			}
		}
	}

	// 経路なし
	return {};
}

std::vector<Node*> MapManager::BuildPath(Node* goal)
{
	std::vector<Node*> path;
	Node* current = goal;

	while (current != nullptr)
	{
		path.push_back(current);
		current = current->parent;
	}

	std::reverse(path.begin(), path.end());
	return path;
}

std::vector<Node*> MapManager::GetNeighbors(Node* node)
{
	std::vector<Node*> neighbors;

	int x = (int)node->pos.x;
	int y = (int)node->pos.y;

	// 上
	if (y > 0)
	{
		neighbors.push_back(&m_nodes[y - 1][x]);
	}

	// 下
	if (y < m_nodes.size() - 1)
	{
		neighbors.push_back(&m_nodes[y + 1][x]);
	}

	// 左
	if (x > 0)
	{
		neighbors.push_back(&m_nodes[y][x - 1]);
	}

	// 右
	if (x < m_nodes[0].size() - 1)
	{
		neighbors.push_back(&m_nodes[y][x + 1]);
	}

	return neighbors;
}