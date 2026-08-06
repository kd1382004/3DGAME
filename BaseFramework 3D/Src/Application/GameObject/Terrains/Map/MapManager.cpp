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

	Math::Vector3 objPos = obj->GetPos();

	for (const auto& mapObj : m_mapObj)
	{
		if (!mapObj) continue;

		obj->RegistHitObject(mapObj);
	}
}

void MapManager::MapHitEnemy(std::shared_ptr<EnemyBase> obj)
{
	if (!obj) { return; }

	for (const auto& mapObj : m_mapObj)
	{
		if (!mapObj) continue;


		obj->RegistHitObject(mapObj);
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
	map->Generate({ 50,50 }, 50, m_mapTileSiz, MapType::MapType_Grassland, &m_mapObj, &m_playerSpawnPos);

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

		// 乱数生成器はループ外で作成
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
