//---- 自分のヘッダー ----
#include "MapObjManager.h"			//マップオブジェクトの管理

//---- マップ ----
#include"../Map/MapManager.h"		//マップの管理・経路探索

//---- マップオブジェクト ----
#include"MapObjBase.h"				//マップオブジェクトの基底クラス

//=============================================
// 基本の処理
//=============================================

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
			// 無効なもの・消え終わったものを外す
			if ((*it)->IsExpired() || (*it)->GetFader().IsHidden())
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

		obj->GenerateDepthMapFromLight();
	}
}

void MapObjManager::PreDraw()
{
	float deltaTime = DeltaTime::Instance().GetRealDeltaTime();

	for (auto& obj : m_mapObjDrawList)
	{
		if (!obj) continue;

		//出現・消失のフェードを進める
		obj->GetFader().Update(deltaTime);

		obj->PreDraw();
	}
}

void MapObjManager::DrawLit()
{
	for (auto& obj : m_mapObjDrawList)
	{
		if (!obj) continue;

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

//=============================================
// 設定
//=============================================

void MapObjManager::AddMapObj(std::shared_ptr<MapObjBase> _spObj)
{
	
		_spObj->SetGameScene(m_wpGameScene.lock());
		m_mapObjList.push_back(_spObj);
	
}

void MapObjManager::SetMapObjUpdateList(std::shared_ptr<MapManager> _spMapManager)
{
	if (!_spMapManager) { return; }

	m_mapObjUpdateList.clear();

	// 今回の描画範囲内のもの
	std::unordered_set<std::shared_ptr<MapObjBase>> inDrawRangeSet;

	for (auto& obj : m_mapObjList)
	{
		Math::Vector2 chunkNum = obj->GetChunkNum();

		// 自分のチャンクか、追加のチャンク(部屋がまたがるチャンク)のどれかが範囲内か
		bool inUpdate = _spMapManager->GetChunksUpdate(chunkNum);
		bool inDraw   = _spMapManager->GetChunksDraw(chunkNum);
		for (const auto& chunk : obj->GetChunkNumList())
		{
			if (inUpdate && inDraw) { break; }
			if (!inUpdate && _spMapManager->GetChunksUpdate(chunk)) { inUpdate = true; }
			if (!inDraw   && _spMapManager->GetChunksDraw(chunk))   { inDraw   = true; }
		}

		if (inUpdate)
		{
			m_mapObjUpdateList.push_back(obj);
			obj->SetInChunk(true);
		}
		else
		{
			obj->SetInChunk(false);
		}

		if (inDraw)
		{
			inDrawRangeSet.insert(obj);
		}
	}

	// 描画範囲内 → 出現させる
	std::list<std::shared_ptr<MapObjBase>> newDrawList;
	for (auto& obj : m_mapObjList)
	{
		if (!inDrawRangeSet.count(obj)) { continue; }

		if (m_isFirstDraw)
		{
			// 生成直後はフェードなしでいきなり表示
			obj->GetFader().SetVisibleImmediate();
		}
		else
		{
			// 表示中・出現中なら何もしない
			obj->GetFader().FadeIn();
		}
		newDrawList.push_back(obj);
	}
	m_isFirstDraw = false;

	// 前回描画していて範囲外になったもの → 消失させて、消え終わるまで描画リストに残す
	for (auto& obj : m_mapObjDrawList)
	{
		if (inDrawRangeSet.count(obj)) { continue; }
		if (obj->GetFader().IsHidden()) { continue; }

		obj->GetFader().FadeOut();
		newDrawList.push_back(obj);
	}

	m_mapObjDrawList = std::move(newDrawList);
}

//=============================================
// その他の処理
//=============================================

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
			_spObj->RegisterHitObject(mapObj);
		}
	}
}
