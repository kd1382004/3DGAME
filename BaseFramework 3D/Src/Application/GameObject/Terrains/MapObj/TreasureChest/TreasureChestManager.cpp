//---- 自分のヘッダー ----
#include "TreasureChestManager.h"					//宝箱の管理

//---- マップオブジェクト ----
#include"TreasureChest.h"							//宝箱
#include"LootTableManager/LootTableManager.h"		//宝箱のドロップテーブル管理

//=============================================
// 基本の処理
//=============================================

void TreasureChestManager::Init()
{
	if (!m_spLootTableManager)
	{
		m_spLootTableManager = std::make_shared<LootTableManager>();
	}
}

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
	for (auto treasureChest : m_treasureChestList)
	{
		treasureChest->Update();
	}
}

void TreasureChestManager::PostUpdate()
{
	for (auto treasureChest : m_treasureChestList)
	{
		treasureChest->PostUpdate();
	}
}

void TreasureChestManager::GenerateDepthMapFromLight()
{
	for (auto treasureChest : m_treasureChestList)
	{
		treasureChest->GenerateDepthMapFromLight();
	}
}

void TreasureChestManager::DrawLit()
{
	for (auto treasureChest : m_treasureChestList)
	{
		treasureChest->DrawLit();
	}
}

void TreasureChestManager::PreDraw()
{
	for (auto treasureChest : m_treasureChestList)
	{
		treasureChest->PreDraw();
	}
}

//=============================================
// その他の処理
//=============================================

void TreasureChestManager::GenerateTreasureChest(std::list<Math::Vector3> _treasureChestPosList)
{

	m_treasureChestList.clear();

	std::shared_ptr<TreasureChest>spTreasureChest;
	for (auto pos : _treasureChestPosList)
	{
		spTreasureChest = std::make_shared<TreasureChest>();
		spTreasureChest->Init();
		spTreasureChest->SetPos(pos);
		spTreasureChest->SetUIManager(m_wpUIManager.lock());
		spTreasureChest->SetPlayer(m_wpPlayer.lock());
		spTreasureChest->SetCamera(m_wpCamera.lock());
		spTreasureChest->SetLootTableManager(m_spLootTableManager);
		m_treasureChestList.push_back(spTreasureChest);
	}

	int a = 0;
}

void TreasureChestManager::TreasureChestHit(const std::shared_ptr<KdGameObject>& _spObj)
{
	if (!_spObj) { return; }

	// 当たり判定リストをクリア
	const Math::Vector3 objPos = _spObj->GetPos();
	constexpr float hitCheckDistSq = 15.0f * 15.0f;

	// 近くにあるオブジェクトだけを新たに登録
	for (const auto& mapObj : m_treasureChestList)
	{
		if (!mapObj) continue;
		float distSq = Math::Vector3::DistanceSquared(mapObj->GetPos(), objPos);
		if (distSq <= hitCheckDistSq)
		{
			_spObj->RegisterHitObject(mapObj);
		}
	}
}
