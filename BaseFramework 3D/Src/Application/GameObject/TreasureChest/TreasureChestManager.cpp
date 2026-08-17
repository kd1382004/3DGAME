#include "TreasureChestManager.h"
#include"TreasureChest.h"

void TreasureChestManager::Init()
{}

void TreasureChestManager::PreUpdate()
{
	auto it = m_treasureChestList.begin();

	while (it != m_treasureChestList.end())
	{
		if ((*it)->IsExpired())	// IsExpired() ・・・ 無効ならtrue
		{
			// 無効なオブジェクトをリストから削除
			it = m_treasureChestList.erase(it);
		}
		else
		{
			++it;	// 次の要素へイテレータを進める
		}
	}
}

void TreasureChestManager::Update()
{
	for (auto TreasureChest : m_treasureChestList)
	{
		TreasureChest->Update();
	}
}

void TreasureChestManager::GenerateDepthMapFromLight()
{
	for (auto TreasureChest : m_treasureChestList)
	{
		TreasureChest->GenerateDepthMapFromLight();
	}
}

void TreasureChestManager::DrawLit()
{
	for (auto TreasureChest : m_treasureChestList)
	{
		TreasureChest->DrawLit();
	}
}

void TreasureChestManager::PreDraw()
{
	for (auto TreasureChest : m_treasureChestList)
	{
		TreasureChest->PreDraw();
	}
}

void TreasureChestManager::GenerateTreasureChest(std::list<Math::Vector3> _treasureChestPosList)
{
	m_treasureChestList.clear();

	std::shared_ptr<TreasureChest>spTreasureChest;
	for (auto pos : _treasureChestPosList)
	{
		spTreasureChest = std::make_shared<TreasureChest>();
		spTreasureChest->Init();
		spTreasureChest->SetPos(pos);
		spTreasureChest->SetPlayer(m_wpPlayer.lock());
		spTreasureChest->SetCamera(m_wpCamera.lock());
		m_treasureChestList.push_back(spTreasureChest);
	}
}

void TreasureChestManager::TreasureChestHit(const std::shared_ptr<KdGameObject>& obj)
{
	if (!obj) { return; }

	// 当たり判定リストをクリア
	const Math::Vector3 objPos = obj->GetPos();
	constexpr float hitCheckDistSq = 15.0f * 15.0f;

	// 近くにあるオブジェクトだけを新たに登録
	for (const auto& mapObj : m_treasureChestList)
	{
		if (!mapObj) continue;
		float distSq = Math::Vector3::DistanceSquared(mapObj->GetPos(), objPos);
		if (distSq <= hitCheckDistSq)
		{
			obj->RegistHitObject(mapObj);
		}
	}
}