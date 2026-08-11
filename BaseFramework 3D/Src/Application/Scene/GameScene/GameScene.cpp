#include "GameScene.h"
#include"../SceneManager.h"
#include"../../GameObject/Character/Player/PlayerBase.h"
#include"../../GameObject/Terrains/Ground/Ground.h"
#include"../../GameObject/Camera/TPSCamera/TPSCamera.h"


//マップ
#include"../../GameObject/Terrains/Map/MapManager.h"

#include"../../../Application/Info/DebugInfo/DebugInfo.h"

//UI
#include"../../GameObject/UI/UIManager.h"

//武器
#include"../../GameObject/Weapon/Dagger/Dagger.h"

//敵
#include"../../GameObject/Character/Enemy/EnemyManager.h"
#include"../../GameObject/Character/Enemy/EnemyBase.h"

void GameScene::ImGUi()
{
	for (auto Camera : m_spCharacterStatus)
	{
		Camera->ImGUI();
	}
}

void GameScene::Event()
{
	if (GetAsyncKeyState(VK_RETURN))
	{
		GenerateMap();
	}


	///////////////////////////////////////////////////
	//マップとのあたり判定

	//プレイヤーのあたり判定リストを毎フレーム更新
	m_spMapManager->MapHit(m_spPlayer);

	//敵のあたり判定リストを毎フレーム更新
	for (auto enemy : m_spEnemyManager->GetEnemyList())
	{
		m_spMapManager->MapHitEnemy(enemy);
	}

	///////////////////////////////////////////////////
	//敵同士のあたり判定
	auto& enemies = m_spEnemyManager->GetEnemyList();

	for (auto itA = enemies.begin(); itA != enemies.end(); ++itA)
	{
		auto itB = itA;
		++itB;

		for (; itB != enemies.end(); ++itB)
		{
			auto enemyA = *itA;
			auto enemyB = *itB;

			Math::Vector3 dic = enemyA->GetPos() - enemyB->GetPos();
			if (dic.Length() < 10)
			{
				enemyA->RegistHitObject(enemyB);
			}
		}
	}
}


void GameScene::Init()
{
	DebugInfo::Instance().SetSceneManagerImGUIFlg(true);

	/////////////////////////////////////////
	//ゲームシーン
	/////////////////////////////////////////
	auto self = shared_from_this();


	/////////////////////////////////////////
	//プレイヤー
	/////////////////////////////////////////
	m_spPlayer = std::make_shared<PlayerBase>();
	m_spPlayer->Init();
	m_objList.push_back(m_spPlayer);

	m_spCharacterStatus.push_back(m_spPlayer);

	/////////////////////////////////////////
	//武器
	/////////////////////////////////////////
	std::shared_ptr<WeaponBase> spWeapon = std::make_shared<Dagger>();
	spWeapon->Init();
	m_objList.push_back(spWeapon);

	/////////////////////////////////////////
	//敵
	/////////////////////////////////////////
	m_spEnemyManager = std::make_shared<EnemyManager>();
	m_spEnemyManager->Init();
	m_objList.push_back(m_spEnemyManager);


	/////////////////////////////////////////
	//カメラ
	/////////////////////////////////////////
	std::shared_ptr<TPSCamera>camera = std::make_shared<TPSCamera>();
	camera->Init();
	m_objList.push_back(camera);


	/////////////////////////////////////////
	//マップ
	/////////////////////////////////////////
	m_spMapManager = std::make_shared<MapManager>();
	m_spMapManager->Init();
	m_objList.push_back(m_spMapManager);
	
	/////////////////////////////////////////
	//UI
	/////////////////////////////////////////	
	std::shared_ptr<UIManager> spUIManager = std::make_shared<UIManager>();
	spUIManager->SetPlayer(m_spPlayer);
	spUIManager->Init();
	m_objList.push_back(spUIManager);

	
	/////////////////////////////////////////
	//プレイヤーにセット
	/////////////////////////////////////////
	m_spPlayer->SetCamera(camera);
	m_spPlayer->SetGameScene(self);
	m_spPlayer->SetWepon(spWeapon);
	m_spPlayer->AddUIList(spUIManager);

	/////////////////////////////////////////
	//敵にセット
	/////////////////////////////////////////
	m_spEnemyManager->SetPlayer(m_spPlayer);
	m_spEnemyManager->SetMapManager(m_spMapManager);
	m_spEnemyManager->SetCamera(camera);
	m_spEnemyManager->AddUIList(spUIManager);

	/////////////////////////////////////////
	//カメラにセット
	/////////////////////////////////////////
	camera->SetTarget(m_spPlayer);

	/////////////////////////////////////////
	//マップにセット
	/////////////////////////////////////////
	m_spMapManager->SetCamera(camera);
	m_spMapManager->SetPlayer(m_spPlayer);
	m_spMapManager->SetEnemyManager(m_spEnemyManager);
	m_spMapManager->SetUIManager(spUIManager);

	//マップ生成
	GenerateMap();
}

//マップ生成
void GameScene::GenerateMap()
{
	if (!m_spMapManager) { return; }
	if (!m_spPlayer) { return; }
	if (!m_spEnemyManager) { return; }
	m_spEnemyManager->EnemyListReset();
	m_spMapManager->GenerateMap();
	m_spMapManager->MapHit(m_spPlayer);

	Math::Vector3 playerSpawn = m_spMapManager->GetPlayerSpawnPos();
	playerSpawn.y += 1;
	m_spPlayer->SetPos(playerSpawn);


}
