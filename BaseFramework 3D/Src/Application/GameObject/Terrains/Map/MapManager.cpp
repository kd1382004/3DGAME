#include "MapManager.h"
#include"MapBase.h"
#include"MapGenerate/MapGenerate.h"
#include"../../Camera/CameraBase.h"
#include"../../Character/Player/PlayerBase.h"
#include"../../Character/Enemy/EnemyBase.h"
#include"../../Character/Enemy/EnemyManager.h"
#include<random>

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

	mapDate = map->Generate({ mapH,mapW }, 50, m_mapTileSiz, MapType::MapType_Grassland, &m_mapObj, &m_playerSpawnPos);

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

	return Math::Vector3(
		node->pos.x * m_mapTileSiz,
		0.0f,
		node->pos.y * m_mapTileSiz
	);
}

Node* MapManager::WorldToNode(const Math::Vector3& worldPos)
{
	int x = (int)(worldPos.x / m_mapTileSiz);
	int y = (int)(worldPos.z / m_mapTileSiz);

	if (x < 0 || y < 0 || y >= m_nodes.size() || x >= m_nodes[0].size())
		return nullptr;

	return &m_nodes[y][x];
}
