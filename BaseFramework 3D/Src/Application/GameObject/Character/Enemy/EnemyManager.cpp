//---- 自分のヘッダー ----
#include "EnemyManager.h"								//敵の管理

//---- シーン ----
#include"../../../Scene/GameScene/GameScene.h"			//ゲームシーン

//---- 敵 ----
#include"EnemyBase.h"									//敵の基底クラス
#include"Goblin/Enemy_Goblin.h"							//敵(ゴブリン)
#include"Golem/Golem.h"									//敵(ゴーレム)
#include"Boss/Giant/Giant.h"							//ボス(巨人)

//---- マップ ----
#include"../../Terrains/Map/MapManager.h"				//マップの管理・経路探索

//---- UI ----
#include"../../UI/UIManager.h"							//UIの管理
#include"../../UI/UIMap/UIMapManager.h"					//ミニマップの管理
#include"../../UI/UIMap/UIMap_Enemy/UIMap_Enemy.h"		//ミニマップの敵アイコン

//=============================================
// 基本の処理
//=============================================

void EnemyManager::Init()
{
	SetEnemyListPlayer();
}

void EnemyManager::PreUpdate()
{
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
	}
	{
		auto it = m_enemyUpdateList.begin();

		while (it != m_enemyUpdateList.end())
		{
			if ((*it)->IsExpired())	// IsExpired() ・・・ 無効ならtrue
			{
				// 無効なオブジェクトをリストから削除
				it = m_enemyUpdateList.erase(it);
			}
			else
			{
				++it;	// 次の要素へイテレータを進める
			}
		}
	}
	

	if (m_spEnemyBoss)
	{
		if (m_spEnemyBoss->IsExpired())
		{
			m_spEnemyBoss = nullptr;
		}
	}

	for (const auto& enemy : m_enemyUpdateList)
	{
		enemy->PreUpdate();
	}
}

void EnemyManager::Update()
{
	for (const auto& enemy : m_enemyUpdateList)
	{
		enemy->Update();
	}
}

void EnemyManager::PostUpdate()
{
	for (const auto& enemy : m_enemyUpdateList)
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

			spUIMapManager->GetUIMap_Enemy()->PosListReset();
			for (const auto& enemy : m_enemyList)
			{
				enemy->PreDraw();
				spUIMapManager->GetUIMap_Enemy()->SetEnemyPosAngleList(enemy->GetPos(), enemy->GetAngle());
			}
		}
	}


	if (m_spEnemyBoss)
	{
		std::shared_ptr<UIMapManager>spUIMapManager = spUIManager->GetUIMapManager();
		if (spUIMapManager)
		{
			m_spEnemyBoss->PreDraw();
			spUIMapManager->GetUIMap_Enemy()->SetEnemyPosAngleList(m_spEnemyBoss->GetPos(), m_spEnemyBoss->GetAngle());
		}
	}


}

void EnemyManager::DrawLit()
{
	for (const auto& enemy : m_enemyUpdateList)
	{
		enemy->DrawLit();
	}
}

void EnemyManager::GenerateDepthMapFromLight()
{
	for (const auto& enemy : m_enemyUpdateList)
	{
		enemy->GenerateDepthMapFromLight();
	}
}

void EnemyManager::DrawDebug()
{
	for (const auto& enemy : m_enemyUpdateList)
	{
		enemy->DrawDebug();
	}
}

//=============================================
// 設定
//=============================================

void EnemyManager::SetPlayer(std::shared_ptr<PlayerBase> _spPlayer)
{
	m_wpPlayer = _spPlayer;
	for (const auto& enemy : m_enemyList)
	{
		enemy->SetPlayer(m_wpPlayer.lock());
	}
}

void EnemyManager::SetEnemyUpdateList(std::shared_ptr<MapManager> _spMapManager)
{
	if (!_spMapManager) { return; }

	m_enemyUpdateList.clear();
	for (auto& obj : m_enemyList)
	{
		Math::Vector3 pso = obj->GetPos();
		if (_spMapManager->GetChunksUpdate(pso))
		{
			m_enemyUpdateList.push_back(obj);
		}
	}


	if (m_spEnemyBoss)
	{
		m_enemyUpdateList.push_back(m_spEnemyBoss);
	}
}

void EnemyManager::SetVisionFlg(bool _flg)
{
	for (const auto& enemy : m_enemyList)
	{
		enemy->SetVisionFlg(_flg);
	}
}

//=============================================
// その他の処理
//=============================================

void EnemyManager::SpawnEnemy(EnemyType _enemyType, Math::Vector3 _spawnPos)
{
	std::shared_ptr<EnemyBase>spEnemyAmbush;
	switch (_enemyType)
	{
	case RoomEnemy:
		spEnemyAmbush = std::make_shared<Goblin>();
		break;
	case RoomStarsEnemy:
		spEnemyAmbush = std::make_shared<Golem>();
		break;
	default:
		break;
	}


	spEnemyAmbush->SetPlayer(m_wpPlayer.lock());
	spEnemyAmbush->SetMapManager(m_wpMapManager.lock());
	spEnemyAmbush->Init();
	spEnemyAmbush->SetSpawnPos(_spawnPos);
	spEnemyAmbush->SetCamera(m_wpCamera.lock());
	spEnemyAmbush->SetGameScene(m_wpGameScene.lock());
	spEnemyAmbush->AddUIList(m_wpUIManager.lock());
	spEnemyAmbush->SetHitDamage(m_wpHitDamage.lock());
	spEnemyAmbush->SetEnemyManager(m_wpEnemyManager.lock());

	int floor = 1;
	auto spGameScene = m_wpGameScene.lock();
	if (spGameScene)
	{
		floor = spGameScene->GetDisplayFloor();
	}
	spEnemyAmbush->SetLV(floor);

	std::shared_ptr<MapManager> spMapManager = m_wpMapManager.lock();
	if (spMapManager)
	{
		spMapManager->MapHitEnemy(spEnemyAmbush);
	}
	m_enemyList.push_back(spEnemyAmbush);

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
	spBoss->SetEnemyManager(m_wpEnemyManager.lock());
	spBoss->SetHitDamage(m_wpHitDamage.lock());

	int floor = 1;
	auto spGameScene = m_wpGameScene.lock();
	if (spGameScene)
	{
		floor = spGameScene->GetDisplayFloor();
	}
	spBoss->SetLV(floor);

	std::shared_ptr<MapManager> spMapManager = m_wpMapManager.lock();
	if (spMapManager)
	{
		spMapManager->MapHitEnemy(spBoss);
	}

	m_spEnemyBoss = spBoss;

	if (m_spEnemyBoss)
	{
		m_enemyUpdateList.push_back(m_spEnemyBoss);
	}

}

//=============================================
// 設定
//=============================================

void EnemyManager::SetEnemyListPlayer()
{
	for (const auto& enemy : m_enemyList)
	{
		enemy->SetPlayer(m_wpPlayer.lock());
	}
}
