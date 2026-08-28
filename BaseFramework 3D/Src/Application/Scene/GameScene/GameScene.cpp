#include "GameScene.h"
#include"../SceneManager.h"

//プレイヤー
#include"../../GameObject/Character/Player/PlayerBase.h"
#include"../../GameObject/Character/Player/PlayerBuffManager/PlayerBuffManager.h"

#include"../../GameObject/Camera/TPSCamera/TPSCamera.h"
#include"../../Info/MouseInfo/MouseInfo.h"

//マップ
#include"../../GameObject/Terrains/Map/MapManager.h"
#include"../../GameObject/Terrains/MapObj/MapObjManager.h"


#include"../../../Application/Info/DebugInfo/DebugInfo.h"

//UI
#include"../../GameObject/UI/UIManager.h"
#include"../../GameObject/UI/BuffUI/BuffUI.h"
#include"../../GameObject/UI/HitDamage/HitDamage.h"

//武器
#include"../../GameObject/Weapon/Dagger/Dagger.h"

//敵
#include"../../GameObject/Character/Enemy/EnemyManager.h"
#include"../../GameObject/Character/Enemy/EnemyBase.h"

//宝箱
#include"../../GameObject/TreasureChest/TreasureChestManager.h"

//ポーション
#include"../../GameObject/Potions/PotionUseController.h"

//ワープゲート
#include"../../GameObject/WarpGate/WarpGateManager.h"

//エフェクト
#include"../../GameObject/Effect/EffectManager.h"

void GameScene::ImGUi()
{
	for (auto Camera : m_spCharacterStatus)
	{
		Camera->ImGUI();
	}
}

void GameScene::WarpGateInit(Math::Vector3 _setPos)
{
	if (!m_spWarpGateManager) { return; }

	m_spWarpGateManager->WarpGateInit(_setPos);
}

void GameScene::SetGameResult()
{
	GameResult gameResult;
	gameResult.enemyKillCount = m_killEnemy;
	gameResult.floorReached = m_displayFloor;
	gameResult.playTime = 0;

	SceneManager::Instance().SetGameResult(gameResult);
}

void GameScene::ChangeResultScene()
{
	SceneManager::Instance().SetNextScene(SceneManager::SceneType::Result);
}

void GameScene::Event()
{
	////////////////////////////////////////////
	//デバック
	static bool REFLG = false;

	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
	{
		if (!REFLG)
		{
			GenerateMap();
			REFLG = true;
		}
	}
	else
	{
		REFLG = false;
	}
	////////////////////////////////////////////

	///////////////////////////////////////////////////
	//あたり判定セット

	//プレイヤーのあたり判定リストを毎フレーム更新
	if (!m_spMapManager) { return; }

	m_spMapManager->MapHit(m_spPlayer);

	//プレイヤーと宝箱のあたり判定
	if (!m_spTreasureChestManager) { return; }
	m_spTreasureChestManager->TreasureChestHit(m_spPlayer);

	//敵のあたり判定リストを毎フレーム更新
	if (!m_spEnemyManager) { return; }
	for (auto enemy : m_spEnemyManager->GetEnemyList())
	{
		m_spMapManager->MapHitEnemy(enemy);
		m_spTreasureChestManager->TreasureChestHit(enemy);
	}

	///////////////////////////////////////////////////
	if (!m_spWeapon) { return; }
	m_spWeapon->ClearAttackHitCharacterList();

	//敵同士のあたり判定
	auto& enemies = m_spEnemyManager->GetEnemyList();

	for (auto itA = enemies.begin(); itA != enemies.end(); itA++)
	{
		auto itB = itA;
		itB++;
		auto enemyA = *itA;
		for (; itB != enemies.end(); itB++)
		{

			auto enemyB = *itB;

			Math::Vector3 dic = enemyA->GetPos() - enemyB->GetPos();
			if (dic.Length() < 10)
			{
				enemyA->RegistHitObject(enemyB);
			}
		}


		//武器とのあたり判定
		if (m_spWeapon)
		{
			Math::Vector3 dic = enemyA->GetPos() - m_spWeapon->GetPos();
			if (dic.Length() < 10)
			{
				m_spWeapon->AddAttackHitCharacterList(enemyA);
			}
		}

	}


	KdDebugGUI::Instance().ClearLog();
	KdDebugGUI::Instance().AddLog("kaisuu: %d\n", m_displayFloor);
}


void GameScene::Init()
{
	DebugInfo::Instance().SetSceneManagerImGUIFlg(true);
	MouseInfo::Instance().SetMouseFreeFlg(false);


	/////////////////////////////////////////
	//ゲームシーン
	/////////////////////////////////////////
	auto self = shared_from_this();
	m_displayFloor = 0;

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
	m_spWeapon = std::make_shared<Dagger>();
	m_spWeapon->Init();
	m_objList.push_back(m_spWeapon);

	/////////////////////////////////////////
	//敵
	/////////////////////////////////////////
	m_spEnemyManager = std::make_shared<EnemyManager>();
	m_spEnemyManager->Init();
	m_objList.push_back(m_spEnemyManager);


	/////////////////////////////////////////
	//カメラ
	/////////////////////////////////////////
	m_spCamera = std::make_shared<TPSCamera>();
	m_spCamera->Init();
	m_objList.push_back(m_spCamera);


	/////////////////////////////////////////
	//マップ
	/////////////////////////////////////////
	m_spMapManager = std::make_shared<MapManager>();
	m_spMapManager->Init();
	m_objList.push_back(m_spMapManager);

	/////////////////////////////////////////
	//マップオブジェクト
	/////////////////////////////////////////
	m_spMapObjManager = std::make_shared<MapObjManager>();
	m_spMapObjManager->Init();
	m_objList.push_back(m_spMapObjManager);


	/////////////////////////////////////////
	//宝箱
	/////////////////////////////////////////
	m_spTreasureChestManager = std::make_shared<TreasureChestManager>();
	m_spTreasureChestManager->Init();
	m_objList.push_back(m_spTreasureChestManager);


	/////////////////////////////////////////
	//ワープゲートマネージャー
	/////////////////////////////////////////	
	m_spWarpGateManager = std::make_shared<WarpGateManager>();
	m_spWarpGateManager->Init();
	m_objList.push_back(m_spWarpGateManager);


	/////////////////////////////////////////
	//エフェクトマネージャー
	/////////////////////////////////////////	
	std::shared_ptr<EffectManager>effect = std::make_shared<EffectManager>();
	effect->Init();
	m_objList.push_back(effect);


	/////////////////////////////////////////
	//ポーション使用コントローラー
	/////////////////////////////////////////	
	m_spPotionUseController = std::make_shared<PotionUseController>();


	/////////////////////////////////////////
	//UI
	/////////////////////////////////////////	
	std::shared_ptr<UIManager> spUIManager = std::make_shared<UIManager>();
	spUIManager->Init();
	spUIManager->SetpPotionTexInfo();
	spUIManager->AddPlayerInventoryUI();
	spUIManager->AddUIMapManager();
	m_objList.push_back(spUIManager);

	/////////////////////////////////////////
	//UIにセット
	/////////////////////////////////////////
	spUIManager->SetPlayer(m_spPlayer);
	spUIManager->SetGameScene(self);
	spUIManager->SetPotionUseController(m_spPotionUseController);

	std::shared_ptr<BuffUI>spBuffUI = std::make_shared<BuffUI>();
	spBuffUI->Init();
	spBuffUI->Set2DPos({ -80,-320 });
	spBuffUI->SetPotionTexInfo(spUIManager->GetPotionTexInfo());
	spBuffUI->SetPlayer(m_spPlayer);
	spUIManager->AddUIObj(spBuffUI);

	std::shared_ptr<HitDamage>spHitDamage = std::make_shared<HitDamage>();
	spHitDamage->Init();
	spHitDamage->SetCamera(m_spCamera);
	spUIManager->AddUIObj(spHitDamage);

	/////////////////////////////////////////
	//プレイヤーにセット
	/////////////////////////////////////////
	m_spPlayer->SetCamera(m_spCamera);
	m_spPlayer->SetGameScene(self);
	m_spPlayer->SetWepon(m_spWeapon);
	m_spPlayer->AddUIList(spUIManager);
	m_spPlayer->SetEffectManager(effect);
	m_spPlayer->SetHitDamage(spHitDamage);
	m_spPlayer->GetPlayerBuffManager()->SetPlayer(m_spPlayer);

	/////////////////////////////////////////
	//敵にセット
	/////////////////////////////////////////
	m_spEnemyManager->SetPlayer(m_spPlayer);
	m_spEnemyManager->SetMapManager(m_spMapManager);
	m_spEnemyManager->SetCamera(m_spCamera);
	m_spEnemyManager->AddUIList(spUIManager);
	m_spEnemyManager->SetGameScene(self);
	m_spEnemyManager->SetHitDamage(spHitDamage);
	m_spEnemyManager->SetEnemyManager(m_spEnemyManager);

	/////////////////////////////////////////
	//カメラにセット
	/////////////////////////////////////////
	m_spCamera->SetTarget(m_spPlayer);

	/////////////////////////////////////////
	//マップにセット
	/////////////////////////////////////////
	m_spMapManager->SetCamera(m_spCamera);
	m_spMapManager->SetPlayer(m_spPlayer);
	m_spMapManager->SetEnemyManager(m_spEnemyManager);
	m_spMapManager->SetUIManager(spUIManager);
	m_spMapManager->SetTreasureChestManager(m_spTreasureChestManager);
	m_spMapManager->SetMapObjManager(m_spMapObjManager);


	/////////////////////////////////////////
	//宝箱にセット
	/////////////////////////////////////////
	m_spTreasureChestManager->SetPlayer(m_spPlayer);
	m_spTreasureChestManager->SetCamera(m_spCamera);
	m_spTreasureChestManager->SetUIManager(spUIManager);


	/////////////////////////////////////////
	//ポーション使用コントローラーにセット
	/////////////////////////////////////////	
	m_spPotionUseController->SetPlayer(m_spPlayer);

	/////////////////////////////////////////
	//ワープゲートマネージャーにセット
	/////////////////////////////////////////	
	m_spWarpGateManager->SetPlayer(m_spPlayer);
	m_spWarpGateManager->SetGameScene(self);



	//マップの成長率を設定
	m_mapLinearGrowthPerFloor = 5;
	m_mapLinearGrowthPerFloorX = 5 * KdRandom::GetFloat(0.8f, 1.2f);
	m_mapLinearGrowthPerFloorY = 5 * KdRandom::GetFloat(0.8f, 1.2f);

	//マップ生成
	GenerateMap();
}

//マップ生成
void GameScene::GenerateMap()
{
	if (!m_spMapManager) { return; }
	if (!m_spPlayer) { return; }
	if (!m_spEnemyManager) { return; }
	if (!m_spMapObjManager) { return; }
	if (!m_spCamera) { return; }

	int baseSize = 30;              // 1階のマップサイズ
	float growth = 1.01f;


	int mapSizeX = static_cast<int>((baseSize + m_displayFloor * m_mapLinearGrowthPerFloorX) * std::pow(growth, m_displayFloor));
	int mapSizeY = static_cast<int>((baseSize + m_displayFloor * m_mapLinearGrowthPerFloorY) * std::pow(growth, m_displayFloor));

	int baseRoomCount = 3;
	int roomCount = baseRoomCount + m_displayFloor * 2;

	m_displayFloor++;


	m_spEnemyManager->EnemyListReset();
	m_spTreasureChestManager->TreasureChestReset();
	m_spMapObjManager->ResetMapObj();

	if (m_displayFloor % m_bossInterval != 0)
	{
		m_spMapManager->GenerateMap({ (float)mapSizeX,(float)mapSizeY }, roomCount, MapType_Grassland);
	}
	else
	{
		//ボス戦用マップ生成
		mapSizeX = 11;
		mapSizeY = 11;
		m_spMapManager->GenerateBossMap({ (float)mapSizeX,(float)mapSizeY }, MapType_Grassland);

	}


	Math::Vector3 playerSpawn = m_spMapManager->GetPlayerSpawnPos();
	m_spPlayer->SetPos(playerSpawn);


	m_spMapObjManager->ObjSetCamera(m_spCamera);
}
