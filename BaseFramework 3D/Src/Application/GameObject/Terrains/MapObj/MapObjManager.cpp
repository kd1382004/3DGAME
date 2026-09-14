#include "MapObjManager.h"

#include"MapObjBase.h"

#include"../Map/MapManager.h"

void MapObjManager::Init()
{}

void MapObjManager::PreUpdate()
{
	// オブジェクトリストの整理 ・・・ 無効なオブジェクトを削除
	auto it = m_mapObjList.begin();

	while (it != m_mapObjList.end())
	{
		if ((*it)->IsExpired())	// IsExpired() ・・・ 無効ならtrue
		{
			// 無効なオブジェクトをリストから削除
			it = m_mapObjList.erase(it);
		}
		else
		{
			++it;	// 次の要素へイテレータを進める
		}
	}

	// オブジェクトリストの整理 ・・・ 無効なオブジェクトを削除
	auto it1 = m_mapObjUpdateList.begin();

	while (it1 != m_mapObjUpdateList.end())
	{
		if ((*it1)->IsExpired())	// IsExpired() ・・・ 無効ならtrue
		{
			// 無効なオブジェクトをリストから削除
			it1 = m_mapObjUpdateList.erase(it1);
		}
		else
		{
			++it1;	// 次の要素へイテレータを進める
		}
	}
}

void MapObjManager::Update()
{
	for (auto& obj : m_mapObjUpdateList)
	{
		obj->Update();
	}
}

void MapObjManager::PostUpdate()
{
	for (auto& obj : m_mapObjUpdateList)
	{
		obj->PostUpdate();
	}
}

void MapObjManager::GenerateDepthMapFromLight()
{
	for (auto& obj : m_mapObjUpdateList)
	{
		if (!obj) continue;
		if (!obj->IsInView()) continue; // 画面外スキップ

		obj->GenerateDepthMapFromLight();
	}
}

void MapObjManager::PreDraw()
{
	for (auto& obj : m_mapObjUpdateList)
	{
		if (!obj) continue;
		obj->PreDraw();
	}
}

void MapObjManager::DrawLit()
{
	for (auto& obj : m_mapObjUpdateList)
	{
		if (!obj) continue;
		if (!obj->IsInView()) continue; // 画面外スキップ

		obj->DrawLit();
	}
}

void MapObjManager::DrawEffect()
{
	for (auto& obj : m_mapObjUpdateList)
	{
		obj->DrawEffect();
	}
}

void MapObjManager::DrawDebug()
{
	for (auto& obj : m_mapObjUpdateList)
	{
		obj->DrawDebug();
	}
}

void MapObjManager::ObjSetCamera(std::shared_ptr<CameraBase> _spCameraBase)
{
	for (auto& obj : m_mapObjList)
	{
		obj->SetCamera(_spCameraBase);
	}
}

void MapObjManager::SetMapObjUpdateList(std::shared_ptr<MapManager> _spMapManager)
{
	if (!_spMapManager) { return; }

	if (!_spMapManager->GetIsChunkChanged()) { return; }


	m_mapObjUpdateList.clear();
	for (auto& obj : m_mapObjList)
	{
		Math::Vector2 chunkNum = obj->GetChunkNum();
		if (_spMapManager->GetChunksUpdate(chunkNum))
		{
			m_mapObjUpdateList.push_back(obj);
			obj->SetInHaunk(true);
		}
		else
		{
			obj->SetInHaunk(false);
		}
	}
}

void MapObjManager::MapObjHit(const std::shared_ptr<KdGameObject>& obj)
{
	if (!obj) { return; }

	const Math::Vector3 objPos = obj->GetPos();
	constexpr float hitCheckDistSq = 15.0f * 15.0f;

	for (const auto& mapObj : m_mapObjUpdateList)
	{
		if (!mapObj) continue;
		float distSq = Math::Vector3::DistanceSquared(mapObj->GetPos(), objPos);
		if (distSq <= hitCheckDistSq)
		{
			obj->RegistHitObject(mapObj);
		}
	}
}
