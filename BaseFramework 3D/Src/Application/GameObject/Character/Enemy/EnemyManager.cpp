#include "EnemyManager.h"
#include"EnemyBase.h"

#include"EnemyAmbush/EnemyAmbush.h"

#include"../../Terrains/Map/MapManager.h"
void EnemyManager::Init()
{
	SetEnemyListPlayer();
}

void EnemyManager::PreUpdate()
{
	for (const auto& enemy : m_enemyList)
	{
		enemy->PreUpdate();
	}
}

void EnemyManager::Update()
{
	for (const auto& enemy : m_enemyList)
	{
		enemy->Update();
	}
}

void EnemyManager::PostUpdate()
{
	for (const auto& enemy : m_enemyList)
	{
		enemy->PostUpdate();
	}
}

void EnemyManager::PreDraw()
{
	for (const auto& enemy : m_enemyList)
	{
		enemy->PreDraw();
	}
}

void EnemyManager::DrawLit()
{
	for (const auto& enemy : m_enemyList)
	{
		enemy->DrawLit();
	}
}

void EnemyManager::GenerateDepthMapFromLight()
{
	for (const auto& enemy : m_enemyList)
	{
		enemy->GenerateDepthMapFromLight();
	}
}

void EnemyManager::DrawDebug()
{
	for (const auto& enemy : m_enemyList)
	{
		enemy->DrawDebug();
	}
}

void EnemyManager::SetPlayer(std::shared_ptr<PlayerBase> _spPalyer)
{
	m_wpPlayer = _spPalyer;
	for (const auto& enemy : m_enemyList)
	{
		enemy->SetPlayer(m_wpPlayer.lock());
	}
}



void EnemyManager::SetEnemyListPlayer()
{
	for (const auto& enemy : m_enemyList)
	{
		enemy->SetPlayer(m_wpPlayer.lock());
	}
}



void EnemyManager::SpawnEnemy(EnemyType _enemyType, Math::Vector3 _spawnPos)
{
	std::shared_ptr<EnemyAmbush>spEnemyAmbush = std::make_shared<EnemyAmbush>();
	spEnemyAmbush->SetPlayer(m_wpPlayer.lock());
	spEnemyAmbush->SetMapManager(m_wpMapManager.lock());
	spEnemyAmbush->Init();
	spEnemyAmbush->SetSpawnPos(_spawnPos);



	std::shared_ptr<MapManager> spMapManager = m_wpMapManager.lock();
	if (spMapManager)
	{
		spMapManager->MapHitEnemy(spEnemyAmbush);
	}


	m_enemyList.push_back(spEnemyAmbush);
}