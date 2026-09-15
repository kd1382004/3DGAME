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

//ポーション
#include"../../GameObject/Potions/PotionUseController.h"

//ワープゲート
#include"../../GameObject/WarpGate/WarpGateManager.h"

//エフェクト
#include"../../GameObject/Effect/EffectManager.h"

//アクセサリー
#include"../../GameObject/Accessory/AccessoryManager.h"

void GameScene::ImGUi()
{
	for (auto Camera : m_spCharacterStatus)
	{
		Camera->ImGUI();
	}

	//FOGImGUI
	if (ImGui::TreeNode(U8("FOG")))
	{
		ImGui::InputFloat(U8("R"), &m_FOGCol.x, 0.01f, 1.0f, "%.2f");
		ImGui::InputFloat(U8("G"), &m_FOGCol.y, 0.01f, 1.0f, "%.2f");
		ImGui::InputFloat(U8("B"), &m_FOGCol.z, 0.01f, 1.0f, "%.2f");
		ImGui::InputFloat(U8("m_FOGdensity"), &m_FOGdensity, 0.01f, 1.0f, "%.2f");
		ImGui::TreePop();
	}

	KdShaderManager::Instance().WorkAmbientController().SetDistanceFog(m_FOGCol, m_FOGdensity);

	//ImGUI
	if (ImGui::TreeNode(U8("階数")))
	{
		ImGui::InputInt(U8("マップ広さMAX"), &m_mapGenerateDisplayFloorMax, 1, 1);

		if (ImGui::Button(U8("次の階")))
		{
			GenerateMap();
		}
		ImGui::TreePop();
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

	if (GetAsyncKeyState('3') & 0x8000)
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
	//////////////////////////////////////////

	///////////////////////////////////////////////////
	//更新チャンク更新
	///////////////////////////////////////////////////
	if (!m_spMapManager) { return; }
	//更新チャンク決め
	m_spMapManager->SetPlayerChanke(m_spPlayer->GetPos());

	if (m_spMapObjManager)
	{
		m_spMapObjManager->SetMapObjUpdateList(m_spMapManager);
	}

	if (m_spEnemyManager)
	{
		m_spEnemyManager->SetEnemyUpdateList(m_spMapManager);
	}


	//あたり判定セット

	//プレイヤーのあたり判定リストを毎フレーム更新
	m_spMapManager->MapHit(m_spPlayer);

	//プレイヤーとマップオブジェクトのあたり判定
	if (m_spMapObjManager)
	{
		m_spMapObjManager->MapObjHit(m_spPlayer);
	}

	//敵のあたり判定リストを毎フレーム更新
	if (!m_spEnemyManager) { return; }
	for (auto enemy : m_spEnemyManager->GetEnemyUpdateList())
	{
		m_spMapManager->MapHitEnemy(enemy);
		if (m_spMapObjManager)
		{
			m_spMapObjManager->MapObjHit(enemy);
		}
	}

	///////////////////////////////////////////////////
	if (!m_spWeapon) { return; }
	m_spWeapon->ClearAttackHitCharacterList();

	//敵同士のあたり判定
	auto& enemies = m_spEnemyManager->GetEnemyUpdateList();

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


	//KdShaderManager::Instance().WorkAmbientController().SetDirLight({ 1,-1,1 }, { 0,0,0 });



	//Fog(霧)
	//distance...距離
	//height ...高さ
	KdShaderManager::Instance().WorkAmbientController().SetFogEnable(true, false);

	//距離フォグの設定
	//col...色
	//density...密度
	KdShaderManager::Instance().WorkAmbientController().SetDistanceFog(m_FOGCol, m_FOGdensity);

	KdShaderManager::Instance().WorkAmbientController().SetAmbientLight({ 1.1,1.1,1.1,1 });

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
	//アクセサリー
	/////////////////////////////////////////	
	std::shared_ptr<AccessoryManager> spAccessoryManager = std::make_shared<AccessoryManager>();
	spAccessoryManager->Init();
	m_objList.push_back(spAccessoryManager);

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
	if (m_spCamera)
	{
		KdEffekseerManager::GetInstance().SetCamera(m_spCamera->GetCamera());
	}
	m_spPlayer->SetCamera(m_spCamera);
	m_spPlayer->SetGameScene(self);
	m_spPlayer->SetWepon(m_spWeapon);
	m_spPlayer->AddUIList(spUIManager);
	m_spPlayer->SetEffectManager(effect);
	m_spPlayer->SetHitDamage(spHitDamage);
	m_spPlayer->SetAccessoryManager(spAccessoryManager);
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
	m_spMapManager->SetMapObjManager(m_spMapObjManager);

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

	//全effect停止
	KdEffekseerManager::GetInstance().StopAllEffect();

	int baseSize = 30;              // 1階のマップサイズ
	float growth = 1.01f;

	m_mapGenerateDisplayFloor = m_displayFloor;

	if (m_displayFloor > m_mapGenerateDisplayFloorMax)
	{
		m_mapGenerateDisplayFloor = m_mapGenerateDisplayFloorMax;
	}

	m_displayFloor++;

	int mapSizeX = static_cast<int>((baseSize + m_mapGenerateDisplayFloor * m_mapLinearGrowthPerFloorX) * std::pow(growth, m_mapGenerateDisplayFloor));
	int mapSizeY = static_cast<int>((baseSize + m_mapGenerateDisplayFloor * m_mapLinearGrowthPerFloorY) * std::pow(growth, m_mapGenerateDisplayFloor));

	int baseRoomCount = 3;
	int roomCount = baseRoomCount + m_mapGenerateDisplayFloor * 2;




	m_spEnemyManager->EnemyListReset();
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


	//更新チャンク決め
	m_spMapManager->SetPlayerChanke(m_spPlayer->GetPos());

	if (m_spMapObjManager)
	{
		m_spMapObjManager->SetMapObjUpdateList(m_spMapManager);
	}


	if (m_spEnemyManager)
	{
		m_spEnemyManager->SetEnemyUpdateList(m_spMapManager);
	}
}
