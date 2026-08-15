#include "TreasureChestManager.h"
#include"TreasureChest.h"

void TreasureChestManager::Init()
{}

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
		m_treasureChestList.push_back(spTreasureChest);
	}
}
