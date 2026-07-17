#include "MapManager.h"
#include"MapBase.h"
#include"MapGenerate/MapGenerate.h"

void MapManager::Init()
{

	std::shared_ptr<MapGenerate>map = std::make_shared<MapGenerate>();
	map->Generate({ 100,50 }, 30, m_mapTileSiz,MapType::MapType_Grassland, &m_mapObj);

}

void MapManager::Update()
{
	for (auto mapObj : m_mapObj)
	{
		mapObj->Update();
	}


	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		m_mapObj.clear();
		Init();
	}
}

void MapManager::DrawLit()
{
	for (auto mapObj : m_mapObj)
	{
		mapObj->DrawLit();
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
