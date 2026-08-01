#include "MapManager.h"
#include"MapBase.h"
#include"MapGenerate/MapGenerate.h"
#include"../../Camera/CameraBase.h"
#include"../../Character/Player/PlayerBase.h"

void MapManager::Init()
{

}

void MapManager::Update()
{
	for (auto mapObj : m_mapObj)
	{
		mapObj->Update();
	}
}

void MapManager::PostUpdate()
{
	/*for (auto mapObj : m_mapObj)
	{
		mapObj->PostUpdate();
	}*/
}

void MapManager::DrawLit()
{
	for (auto mapObj : m_mapObj)
	{
		mapObj->DrawLit();
	}
}

void MapManager::PreDraw()
{
	for (auto mapObj : m_mapObj)
	{
		mapObj->PreDraw();
	}
}

void MapManager::GenerateDepthMapFromLight()
{
	for (auto mapObj : m_mapObj)
	{
		mapObj->GenerateDepthMapFromLight();
	}
}

void MapManager::MapHit(std::shared_ptr<KdGameObject> obj)
{

	if (!obj) { return; }

	for (auto mapObj : m_mapObj)
	{
		obj->RegistHitObject(mapObj);
	}

}

void MapManager::SetCamera(std::shared_ptr<CameraBase> _spCamera)
{
	m_wpCamera = _spCamera;

	for (auto mapObj : m_mapObj)
	{
		mapObj->SetCamera(_spCamera);

		_spCamera->ResolveCameraOcclusionObject(mapObj);
	}
}

void MapManager::GenerateMap()
{
	m_mapObj.clear();

	std::shared_ptr<MapGenerate>map = std::make_shared<MapGenerate>();
	map->Generate({ 30,30 }, 50, m_mapTileSiz, MapType::MapType_Grassland, &m_mapObj,&m_playerSpawnPos);

	if (!m_wpCamera.expired())
	{
		SetCamera(m_wpCamera.lock());
	}

	std::shared_ptr<PlayerBase>spPlayerBase=m_wpPlayerBase.lock();

	if (spPlayerBase)
	{
		for (auto mapObj : m_mapObj)
		{
			mapObj->SetPlayer(spPlayerBase);
		}
	}
}
