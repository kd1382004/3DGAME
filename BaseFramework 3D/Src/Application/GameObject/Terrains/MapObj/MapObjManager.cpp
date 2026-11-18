#include "MapObjManager.h"

#include"MapObjBase.h"

#include"../Map/MapManager.h"

void MapObjManager::Init()
{}

void MapObjManager::PreUpdate()
{
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
	}

	{
		// オブジェクトリストの整理 ・・・ 無効なオブジェクトを削除
		auto it = m_mapObjUpdateList.begin();

		while (it != m_mapObjUpdateList.end())
		{
			if ((*it)->IsExpired())	// IsExpired() ・・・ 無効ならtrue
			{
				// 無効なオブジェクトをリストから削除
				it = m_mapObjUpdateList.erase(it);
			}
			else
			{
				++it;	// 次の要素へイテレータを進める
			}
		}
	}

	{
		// オブジェクトリストの整理 ・・・ 無効なオブジェクトを削除
		auto it = m_mapObjDrawList.begin();

		while (it != m_mapObjDrawList.end())
		{
			if ((*it)->IsExpired())	// IsExpired() ・・・ 無効ならtrue
			{
				// 無効なオブジェクトをリストから削除
				it = m_mapObjDrawList.erase(it);
			}
			else
			{
				++it;	// 次の要素へイテレータを進める
			}
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
	for (auto& obj : m_mapObjDrawList)
	{
		if (!obj) continue;
		if (!obj->IsInView()) continue; // 画面外スキップ

		obj->GenerateDepthMapFromLight();
	}
}

void MapObjManager::PreDraw()
{
	for (auto& obj : m_mapObjDrawList)
	{
		if (!obj) continue;
		obj->PreDraw();
	}
}

void MapObjManager::DrawLit()
{
	for (auto& obj : m_mapObjDrawList)
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

void MapObjManager::AddMapObj(std::shared_ptr<MapObjBase> _spObj)
{
	
		_spObj->SetGameScene(m_wpGameScene.lock());
		m_mapObjList.push_back(_spObj);
	
}

void MapObjManager::SetMapObjUpdateList(std::shared_ptr<MapManager> _spMapManager)
{
	if (!_spMapManager) { return; }

	m_mapObjUpdateList.clear();
	m_mapObjDrawList.clear();
	for (auto& obj : m_mapObjList)
	{
		Math::Vector2 chunkNum = obj->GetChunkNum();
		if (_spMapManager->GetChunksUpdate(chunkNum))
		{
			m_mapObjUpdateList.push_back(obj);
			obj->SetInChunk(true);
		}
		else
		{
			obj->SetInChunk(false);
		}
		
		if (_spMapManager->GetChunksDraw(chunkNum))
		{
			m_mapObjDrawList.push_back(obj);
		}
	}
}

void MapObjManager::SetMapObjDrawList(std::shared_ptr<MapManager> _spMapManager)
{
	if (!_spMapManager) { return; }

	m_mapObjDrawList.clear();
	for (auto& obj : m_mapObjList)
	{
		Math::Vector2 chunkNum = obj->GetChunkNum();
		if (_spMapManager->GetChunksUpdate(chunkNum))
		{
			obj->SetInChunk(true);
		}
		else
		{
			obj->SetInChunk(false);
		}
	}

}

void MapObjManager::ObjSetCamera(std::shared_ptr<CameraBase> _spCameraBase)
{
	for (auto& obj : m_mapObjList)
	{
		obj->SetCamera(_spCameraBase);
	}
}

void MapObjManager::MapObjHit(const std::shared_ptr<KdGameObject>& _spObj)
{
	if (!_spObj) { return; }

	const Math::Vector3 objPos = _spObj->GetPos();
	constexpr float hitCheckDistSq = 15.0f * 15.0f;

	for (const auto& mapObj : m_mapObjUpdateList)
	{
		if (!mapObj) continue;
		float distSq = Math::Vector3::DistanceSquared(mapObj->GetPos(), objPos);
		if (distSq <= hitCheckDistSq)
		{
			_spObj->RegistHitObject(mapObj);
		}
	}
}
