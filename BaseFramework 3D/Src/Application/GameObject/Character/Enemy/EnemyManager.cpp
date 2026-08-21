#include "EnemyManager.h"
#include"EnemyBase.h"

#include"Goblin/Enemy_Goblin.h"

#include"Boss/Giant/Giant.h"

#include"../../Terrains/Map/MapManager.h"

#include"../../UI/UIManager.h"
#include"../../UI/UIMap/UIMapManager.h"
#include"../../UI/UIMap/UIMap_Enemy/UIMap_Enemy.h"
void EnemyManager::Init()
{
	SetEnemyListPlayer();
}

void EnemyManager::PreUpdate()
{
	auto it = m_enemyList.begin();

	while (it != m_enemyList.end())
	{
		if ((*it)->IsExpired())	// IsExpired() ・・・ 無効ならtrue
		{
			// 無効なオブジェクトをリストから削除
			it = m_enemyList.erase(it);
		}
		else
		{
			++it;	// 次の要素へイテレータを進める
		}
	}



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
	std::shared_ptr<UIManager>spUIManager = m_wpUIManager.lock();
	if (spUIManager)
	{
		std::shared_ptr<UIMapManager>spUIMapManager = spUIManager->GetUIMapManager();
		if (spUIMapManager)
		{

			spUIMapManager->GetUIMap_Enemy()->PosListRiseto();
			for (const auto& enemy : m_enemyList)
			{
				enemy->PreDraw();
				spUIMapManager->GetUIMap_Enemy()->SetEnemyPosAngleList(enemy->GetPos(), enemy->GetAngle());
			}
		}
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

void EnemyManager::SpawnBoss(Math::Vector3 _spawnPos)
{
	std::shared_ptr<Giant>spBoss = std::make_shared<Giant>();
	spBoss->Init();
	spBoss->SetPlayer(m_wpPlayer.lock());
	spBoss->SetMapManager(m_wpMapManager.lock());
	spBoss->SetSpawnPos(_spawnPos);
	spBoss->SetCamera(m_wpCamera.lock());
	spBoss->AddUIList(m_wpUIManager.lock());
	spBoss->SetGameScene(m_wpGameScene.lock());
	std::shared_ptr<MapManager> spMapManager = m_wpMapManager.lock();
	if (spMapManager)
	{
		spMapManager->MapHitEnemy(spBoss);
	}


	m_enemyList.push_back(spBoss);
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
	std::shared_ptr<Goblin>spEnemyAmbush = std::make_shared<Goblin>();
	spEnemyAmbush->SetPlayer(m_wpPlayer.lock());
	spEnemyAmbush->SetMapManager(m_wpMapManager.lock());
	spEnemyAmbush->Init();
	spEnemyAmbush->SetSpawnPos(_spawnPos);
	spEnemyAmbush->SetCamera(m_wpCamera.lock());

	spEnemyAmbush->AddUIList(m_wpUIManager.lock());

	std::shared_ptr<MapManager> spMapManager = m_wpMapManager.lock();
	if (spMapManager)
	{
		spMapManager->MapHitEnemy(spEnemyAmbush);
	}


	m_enemyList.push_back(spEnemyAmbush);
}