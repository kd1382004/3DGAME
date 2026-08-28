#include "MapManager.h"
#include "MapBase.h"
#include "MapGenerate/MapGenerate.h"
#include "../../Camera/CameraBase.h"
#include "../../Character/Player/PlayerBase.h"
#include "../../Character/Enemy/EnemyBase.h"
#include "../../Character/Enemy/EnemyManager.h"
#include "../../UI/UIManager.h"
#include "../../UI/UIMap/UIMapManager.h"
#include "../../UI/UIMap/UIMap_Map/UIMap_Map.h"
#include"../../TreasureChest/TreasureChestManager.h"

#include <algorithm>
#include <cmath>
#include <cfloat>
#include <random>

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

void MapManager::MapHit(const std::shared_ptr<KdGameObject>& obj)
{
	if (!obj) { return; }

	// 当たり判定リストをクリア
	obj->ClearHitObjectList();
	const Math::Vector3 objPos = obj->GetPos();
	constexpr float hitCheckDistSq = 15.0f * 15.0f;

	// 近くにあるオブジェクトだけを新たに登録
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

void MapManager::MapHitEnemy(const std::shared_ptr<EnemyBase>& obj)
{
	MapHit(obj);
}

void MapManager::SetCamera(const std::shared_ptr<CameraBase>& spCamera)
{
	m_wpCamera = spCamera;

	for (const auto& mapObj : m_mapObj)
	{
		mapObj->SetCamera(spCamera);
		spCamera->ResolveCameraOcclusionObject(mapObj);
	}
}

void MapManager::GenerateMap(Math::Vector2 _mapSiz, int roomNum, MapType _MapType)
{
	m_mapObj.clear();

	std::shared_ptr<MapGenerate> map = std::make_shared<MapGenerate>();

	map->SetMapObjManager(m_wpMapObjManager.lock());

	//敵が歩ける一覧
	std::vector<std::vector<bool>> mapData;
	Math::Vector3 basePos;
	mapData = map->Generate(_mapSiz, roomNum, m_mapTileSiz, _MapType, &m_mapObj, &m_playerSpawnPos, &basePos);
	auto mapRoomList = map->GetRoomInfoList();

	//A*の初期化
	CreateNodeGrid(static_cast<int>(_mapSiz.x), static_cast<int>(_mapSiz.y), m_mapTileSiz);
	//A*の設定
	ApplyWalkableFromMap(mapData);


	//プレイヤーを設定
	std::shared_ptr<PlayerBase> spPlayerBase = m_wpPlayerBase.lock();
	if (spPlayerBase)
	{
		for (const auto& mapObj : m_mapObj)
		{
			mapObj->SetPlayer(spPlayerBase);
		}
	}


	////////////////////////////////////////////////////
	// ミニマップ生成
	std::shared_ptr<UIManager> spUIManager = m_wpUIManager.lock();
	if (spUIManager)
	{
		std::shared_ptr<UIMapManager> spUIMapManager = spUIManager->GetUIMapManager();
		if (spUIMapManager)
		{
			spUIMapManager->SetBase3DPos(basePos);
			spUIMapManager->SetTileSiz(m_mapTileSiz);
			spUIMapManager->GetUIMap_Map()->PosListReset();
			spUIMapManager->ResetTreasureChest();

			for (const auto& mapObj : m_mapObj)
			{
				if (mapObj->GetMapObjType() == MapObjType::Ground|| mapObj->GetMapObjType() == MapObjType::TypeSlope)
				{

					if (mapObj->GetRoomType() == RoomType::RoomType_NORoom)
					{
						spUIMapManager->GetUIMap_Map()->AddPosList(mapObj->GetPos(), m_mapTileSiz);
					}
					else
					{
						spUIMapManager->GetUIMap_Map()->AddPosList(mapObj->GetPos(), m_mapTileSiz, mapObj->GetRoomID());
					}

				
				}

				if (mapObj->GetMapObjType() == MapObjType::Stairs)
				{
					spUIMapManager->GetUIMap_Map()->AddStairsPos(mapObj->GetPos(), m_mapTileSiz, mapObj->GetRoomID());
					spUIMapManager->GetUIMap_Map()->SetIsStairsMine(true);
				}
			}
		}
	}

	////////////////////////////////////////////////////
	// 敵の生成
	std::shared_ptr<EnemyManager> spEnemyManager = m_wpEnemyManager.lock();
	if (spEnemyManager)
	{

		std::random_device rd;
		std::mt19937 mt(rd());

		for (size_t i = 0; i < mapRoomList.size(); i++)
		{
			if (mapRoomList[i].empty()) { continue; }

			int roomEnemyNum = mapRoomList[i][0].m_roomEnemyNum;

			struct EnemySpawnList {
				Math::Vector3 m_pos;
				int roomID = 0;
				int floorNum = 0;
			};

			std::vector<EnemySpawnList> enemySpawnList;
			enemySpawnList.reserve(mapRoomList[i].size());

			for (size_t j = 0; j < mapRoomList[i].size(); j++)
			{
				//何か設置されてたらスキップ
				if (mapRoomList[i][j].m_Installation) { continue; }

				EnemySpawnList enemySpawn;
				enemySpawn.m_pos = mapRoomList[i][j].m_pos;
				enemySpawn.roomID = mapRoomList[i][j].m_roomID;
				enemySpawn.floorNum = j;
				enemySpawnList.push_back(enemySpawn);
			}

			if (enemySpawnList.empty()) { continue; }

			// 敵数を制限
			roomEnemyNum = std::min(roomEnemyNum, static_cast<int>(enemySpawnList.size()));

			// ランダムシャッフル
			std::shuffle(enemySpawnList.begin(), enemySpawnList.end(), mt);

			// 先頭から必要数だけスポーン
			for (int n = 0; n < roomEnemyNum; n++)
			{
				spEnemyManager->SpawnEnemy(RoomEnemy, enemySpawnList[n].m_pos);
				mapRoomList[enemySpawnList[n].roomID][enemySpawnList[n].floorNum].m_Installation = true;
			}
		}
	}


	////////////////////////////////////////////////////
	//宝箱生成
	std::shared_ptr<TreasureChestManager>spTreasureChestManager = m_wpTreasureChestManager.lock();
	if (spTreasureChestManager)
	{
		std::list<Math::Vector3> TreasureChestPosList;

		for (size_t i = 0; i < mapRoomList.size(); i++)
		{
			if (mapRoomList[i].empty()) { continue; }
			int chestNum = mapRoomList[i][0].m_roomTreasuerChestNum;
			int roomType = mapRoomList[i][0].m_roomType;

			while (true)
			{

				int LoomNum = KdRandom::GetInt(0, mapRoomList[i].size() - 1);
				if (mapRoomList[i][LoomNum].m_Installation)
				{
					continue;
				}

				float spawnRate = 0;

				if (roomType == RoomType::RoomType_TreasureChestRoom || roomType == RoomType::RoomType_SafeRoom)
				{
					spawnRate = 1;
				}
				else
				{
					spawnRate = 0.4;
				}


				if (KdRandom::GetFloat(0.0f, 1.0f) <= spawnRate)
				{
					Math::Vector3 pos = mapRoomList[i][LoomNum].m_pos;
					mapRoomList[i][LoomNum].m_Installation = true;

					int x = mapRoomList[i][LoomNum].m_xy.x;
					int y = mapRoomList[i][LoomNum].m_xy.y;
					mapData[y][x] = false;
					TreasureChestPosList.push_back(pos);
				}


				chestNum--;
				if (chestNum <= 0)
				{
					break;
				}
			}
		}

		spTreasureChestManager->GenerateTreasureChest(TreasureChestPosList);
	}








	//カメラセット
	if (!m_wpCamera.expired())
	{
		SetCamera(m_wpCamera.lock());
	}

}

void MapManager::GenerateBossMap(Math::Vector2 _mapSiz, MapType _type)
{

	m_mapObj.clear();

	std::shared_ptr<MapGenerate> map = std::make_shared<MapGenerate>();

	//敵が歩ける一覧
	std::vector<std::vector<bool>> mapData;

	Math::Vector3 basePos;
	mapData = map->GenerateBoss(_mapSiz, m_mapTileSiz, (int)_type, &m_mapObj, &m_playerSpawnPos, &basePos);

	//A*の初期化
	CreateNodeGrid(static_cast<int>(_mapSiz.x), static_cast<int>(_mapSiz.y), m_mapTileSiz);
	//A*の設定
	ApplyWalkableFromMap(mapData);

	//プレイヤーを設定
	std::shared_ptr<PlayerBase> spPlayerBase = m_wpPlayerBase.lock();
	if (spPlayerBase)
	{
		for (const auto& mapObj : m_mapObj)
		{
			mapObj->SetPlayer(spPlayerBase);
		}
	}
	////////////////////////////////////////////////////
	//ボス生成
	std::shared_ptr<EnemyManager> spEnemyManager = m_wpEnemyManager.lock();
	if (spEnemyManager)
	{
		spEnemyManager->SpawnBoss(map->GetBossSpawnPos());
	}


	////////////////////////////////////////////////////
	// ミニマップ生成
	std::shared_ptr<UIManager> spUIManager = m_wpUIManager.lock();
	if (spUIManager)
	{
		std::shared_ptr<UIMapManager> spUIMapManager = spUIManager->GetUIMapManager();
		if (spUIMapManager)
		{
			spUIMapManager->SetBase3DPos(basePos);
			spUIMapManager->SetTileSiz(m_mapTileSiz);
			spUIMapManager->GetUIMap_Map()->PosListReset();
			spUIMapManager->ResetTreasureChest();

			for (const auto& mapObj : m_mapObj)
			{
				if (mapObj->GetMapObjType() == MapObjType::Ground)
				{
					spUIMapManager->GetUIMap_Map()->AddPosList(mapObj->GetPos(), m_mapTileSiz);
				}
			}
		}
	}

	////////////////////////////////////////////////////
	//カメラセット
	if (!m_wpCamera.expired())
	{
		SetCamera(m_wpCamera.lock());
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
			node.pos = Math::Vector2(static_cast<float>(x), static_cast<float>(y));

			// 初期値(通れないで初期化)
			node.walkable = false;

			// A* 用の初期化
			node.gCost = FLT_MAX;
			node.hCost = 0;
			node.parent = nullptr;
		}
	}
}

void MapManager::ApplyWalkableFromMap(const std::vector<std::vector<bool>>& mapData)
{
	for (size_t y = 0; y < mapData.size(); y++)
	{
		for (size_t x = 0; x < mapData[y].size(); x++)
		{
			if (mapData[y][x])
			{
				m_nodes[y][x].walkable = true;
			}
		}
	}
}

Math::Vector3 MapManager::NodeToWorld(const Node* node) const
{
	if (!node) return Math::Vector3::Zero;

	float worldX = m_mapTileSiz * node->pos.x + m_mapTileSiz * 0.5f;
	float worldZ = -(m_mapTileSiz * node->pos.y + m_mapTileSiz * 0.5f);

	return Math::Vector3(worldX, 0.0f, worldZ);
}

Node* MapManager::WorldToNode(const Math::Vector3& worldPos)
{
	// X は右へプラス
	int x = static_cast<int>(floor(worldPos.x / m_mapTileSiz));

	// Z は下へマイナス → -Z がタイル番号
	int y = static_cast<int>(floor((-worldPos.z) / m_mapTileSiz));

	if (m_nodes.empty() || m_nodes[0].empty()) return nullptr;

	int width = static_cast<int>(m_nodes[0].size());
	int height = static_cast<int>(m_nodes.size());

	if (x < 0 || y < 0 || x >= width || y >= height)
	{
		return nullptr;
	}

	return &m_nodes[y][x];
}

std::vector<Node*> MapManager::FindPath(Node* start, Node* goal)
{
	if (!start || !goal) return {};

	for (auto& row : m_nodes)
	{
		for (auto& node : row)
		{
			node.gCost = FLT_MAX;
			node.hCost = 0.0f;
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
		// openList の中で fCost が最小のノードを探す
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

		// ゴールに到達したら経路復元
		if (current == goal)
		{
			return BuildPath(goal);
		}

		// 隣接ノードを取得
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

std::vector<Node*> MapManager::BuildPath(Node* goal) const
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
	if (!node) return neighbors;

	int x = static_cast<int>(node->pos.x);
	int y = static_cast<int>(node->pos.y);

	// 上
	if (y > 0)
	{
		neighbors.push_back(&m_nodes[y - 1][x]);
	}

	// 下
	if (y < static_cast<int>(m_nodes.size()) - 1)
	{
		neighbors.push_back(&m_nodes[y + 1][x]);
	}

	// 左
	if (x > 0)
	{
		neighbors.push_back(&m_nodes[y][x - 1]);
	}

	// 右
	if (x < static_cast<int>(m_nodes[0].size()) - 1)
	{
		neighbors.push_back(&m_nodes[y][x + 1]);
	}

	return neighbors;
}
