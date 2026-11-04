#include "GameScene.h"
//プレイヤー
#include"../../GameObject/Character/Player/PlayerBase.h"
#include"../../GameObject/Character/Player/PlayerBuffManager/PlayerBuffManager.h"
#include"../../GameObject/Character/Player/PlayerInventory/PlayerInventory.h"

#include"../../GameObject/Camera/TPSCamera/TPSCamera.h"
#include"../../Info/MouseInfo/MouseInfo.h"
#include"../../Info/FadeDraw/FadeDraw.h"


//マップ
#include"../../GameObject/Terrains/Map/MapManager.h"
#include"../../GameObject/Terrains/MapObj/MapObjManager.h"


#include"../../../Application/Info/DebugInfo/DebugInfo.h"

//UI
#include"../../GameObject/UI/UIManager.h"
#include"../../GameObject/UI/BuffUI/BuffUI.h"
#include"../../GameObject/UI/HitDamage/HitDamage.h"
#include"../../GameObject/UI/UI2/UIBase2.h"

//武器
#include"../../GameObject/Weapon/Dagger/Dagger.h"
#include"../../GameObject/Weapon/Sword/Sword.h"

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

GameScene::~GameScene()
{
	if (m_spBossBGM)
	{
		m_spBossBGM->Stop();
	}

	if (m_spGameBGM)
	{
		m_spGameBGM->Stop();
	}

}

void GameScene::ImGUi()
{
	for (auto Camera : m_spCharacterStatus)
	{
		Camera->ImGUI();
	}

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

void GameScene::WarpGateInit(Math::Vector3 _setNextGatePos, Math::Vector3 _setEacapeGatePos)
{
	if (!m_spWarpGateManager) { return; }

	m_spWarpGateManager->WarpGateInit(_setNextGatePos, _setEacapeGatePos);
}

void GameScene::SetGameResult()
{
	GameResult gameResult;
	gameResult.displayFloorInfo = m_DisplayFloorInfo;


	if (m_spPlayer)
	{
		gameResult.playerDead = m_spPlayer->GetIsDead();


		bool saveFlg = !gameResult.playerDead;
		SaveInventoryData(saveFlg, m_spPlayer->GetPotionsInventoryList(), m_spPlayer->GetWeaponsStrengtheningInventoryList());
		SaveWeaponStrengtheningData(saveFlg);

		//プレイヤーのステータス状況を保存
		m_spPlayer->ClearAllBuffs();
		if (saveFlg)
		{
			m_spPlayer->SavePlayerStatus(m_PlayerStatusFilePath);
		}
		else
		{
			//プレイヤーのステータス状況をリセット
			m_spPlayer->LoadPlayerStatus(m_spPlayer->GetCharaDefaultStatusFilePath());
			m_spPlayer->SavePlayerStatus(m_PlayerStatusFilePath);
		}

	}

	SceneManager::Instance().SetGameResult(gameResult);
}

void GameScene::ChangeResultScene()
{
	SetGameResult();
	FadeDraw::Instance().SetFadeType(FedeOut, 3);
	SceneManager::Instance().SetNextScene(SceneManager::SceneType::Result);
}

void GameScene::AddEnemyKill()
{
	m_DisplayFloorInfo[m_displayFloor].enemyKillCount++;
	m_DisplayFloorInfo[0].enemyKillCount++;
}

void GameScene::AddTreasureChestsNum()
{
	m_DisplayFloorInfo[m_displayFloor].TreasureChestsNum++;
	m_DisplayFloorInfo[0].TreasureChestsNum++;
}

void GameScene::PlayerDefeat()
{
	m_playerDefeatFlg = true;

	FadeDraw::Instance().SetFadeType(FedeIn, 3);
}

void GameScene::Event()
{
	if (m_playerDefeatFlg)
	{
		if (!FadeDraw::Instance().GetFadeFlg())
		{
			ChangeResultScene();
		}

		return;
	}



	////////////////////////////////////////////
	//デバック
	static bool REFLG = false;

	if (GetAsyncKeyState('3') & 0x8000)
	{
		GenerateMap();
		if (!REFLG)
		{
			
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

	// プレイヤーのチャンクが移動した時だけ、更新リストを再構築する
	if (m_spMapManager->GetIsChunkChanged())
	{
		if (m_spMapObjManager)
		{
			m_spMapObjManager->SetMapObjUpdateList(m_spMapManager);
		}
		if (m_spEnemyManager)
		{
			m_spEnemyManager->SetEnemyUpdateList(m_spMapManager);
		}

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
	m_spWeapon->ClearObjList();
	std::vector<std::shared_ptr<MapBase>> mapList = m_spMapManager->GetUpdateChankes();
	for (std::shared_ptr<MapBase> map : mapList)
	{
		m_spWeapon->AddObjList(map);
	}


	//武器とのあたり判定
	auto& enemies = m_spEnemyManager->GetEnemyUpdateList();
	for (auto& enemyA : enemies)
	{
		// 武器とのあたり判定
		if (m_spWeapon)
		{
			m_spWeapon->AddAttackHitCharacterList(enemyA);
		}
	}


	KdDebugGUI::Instance().ClearLog();
	KdDebugGUI::Instance().AddLog("kaisuu: %d\n", m_displayFloor);



	float dt = DeltaTime::Instance().GetRealDeltaTime();

	m_DisplayFloorInfo[m_displayFloor].playTimeSc += dt;
	m_DisplayFloorInfo[0].playTimeSc += dt;


	if (GetAsyncKeyState('5') & 0x8000)
	{
		DebugInfo::Instance().SetSceneManagerImGUIFlg(true);
	}

	if (GetAsyncKeyState('6') & 0x8000)
	{
		DebugInfo::Instance().SetSceneManagerImGUIFlg(false);
	}
}

void GameScene::SaveInventoryData(bool _isSave, const std::vector<Inventory>& _inventoryPotionsList, const std::vector<Inventory>& _inventoryWeaponsList)
{
	nlohmann::json data;

	// アイテムが0個のときも、nullではなく [] として保存する
	data["potions"] = nlohmann::json::array();
	data["weaponStrengtheningMaterials"] = nlohmann::json::array();

	for (const Inventory& potion : _inventoryPotionsList)
	{
		int num = 0;
		if (_isSave)
		{
			num = potion.m_num;
		}

		data["potions"].push_back({
			{"id", potion.m_ID},
			{"num", num},
			{"name", potion.m_name}
			});
	}

	for (const Inventory& weapon : _inventoryWeaponsList)
	{
		int num = 0;
		if (_isSave)
		{
			num = weapon.m_num;
		}

		data["weaponStrengtheningMaterials"].push_back({
			{"id", weapon.m_ID},
			{"num", num},
			{"name", weapon.m_name}
			});
	}

	std::ofstream file(m_InventoryFilePath);
	if (!file) return; // 必要に応じてエラー処理を追加

	file << data.dump(4);

}

void GameScene::LoadInventoryData()
{
	std::ifstream file(m_InventoryFilePath);
	if (!file) return; // 初回起動などでファイルがない場合は何もしない


	nlohmann::json data = nlohmann::json::parse(file);

	std::vector<Inventory> potions = ReadInventoryList(data, "potions");
	std::vector<Inventory> materials = ReadInventoryList(data, "weaponStrengtheningMaterials");

	m_spPlayer->SetPotionsInventoryList(potions);
	m_spPlayer->SetWeaponsStrengtheningInventoryList(materials);
}

std::vector<Inventory> GameScene::ReadInventoryList(const nlohmann::json& _data, const std::string& _key)
{
	std::vector<Inventory> list;

	if (!_data.contains(_key) || !_data[_key].is_array())
	{
		return list;
	}

	for (const nlohmann::json& itemData : _data[_key])
	{
		Inventory item;
		item.m_ID = itemData.value("id", 0);
		item.m_num = itemData.value("num", 0);
		item.m_name = itemData.value("name", std::string{});
		list.push_back(item);
	}

	return list;
}

void GameScene::SaveWeaponStrengtheningData(bool _isSave)
{
	// 他の武器のデータを残すために1度開く（ファイルがなくても続行する）
	nlohmann::json oldData = nlohmann::json::object();
	{
		std::ifstream oldFile(m_WeaponStrengtheningFilePath);
		if (oldFile)
		{
			oldData = nlohmann::json::parse(oldFile, nullptr, false);
			if (!oldData.is_object())
			{
				oldData = nlohmann::json::object();
			}
		}
	}

	nlohmann::json data = nlohmann::json::object();
	const int currentWeapon = SceneManager::Instance().GetPlayerWeaponInfo();

	// 武器ごとのループ
	for (int weapon = 0; weapon < PlayerWeaponInfo::PlayerWeaponInfoSize; ++weapon)
	{
		const std::string key = GetWeaponKey(static_cast<PlayerWeaponInfo>(weapon));

		std::vector<WeaponsStrengtheningInfo> infoList;

		if (_isSave)
		{
			if (weapon == currentWeapon && m_spWeapon)
			{
				infoList = m_spWeapon->GetWeaponsStrengtheningInfoList();
			}
			else
			{
				infoList = ReadWeaponStrengtheningList(oldData, key);
			}
		}

		// falseのときや保存データがないときは、初期値で全項目を作る
		infoList.resize(WeaponsStrengtheningSiz);

		// 強化項目ごとのループ
		nlohmann::json weaponData = nlohmann::json::object();
		for (int i = 0; i < WeaponsStrengtheningSiz; ++i)
		{
			const WeaponsStrengtheningInfo& item = infoList[i];

			weaponData[kStrengtheningKeys[i]] = {
				{"Lv", item.Lv},
				{"nextLvNum", item.nextLvNum},
				{"strengtheningItemNum", item.strengtheningItemNum},
				{"strengtheningFlg", item.strengtheningFlg}
			};
		}

		data[key] = weaponData;
	}

	// ファイルに書き込む
	std::ofstream file(m_WeaponStrengtheningFilePath);
	if (!file) return; // 必要に応じてエラー処理を追加

	file << data.dump(4);
}
void GameScene::LoadWeaponStrengtheningData()
{
	if (!m_spWeapon) { return; }

	std::ifstream file(m_WeaponStrengtheningFilePath);
	if (!file) return; // 初回起動などでファイルがない場合は何もしない
	nlohmann::json data = nlohmann::json::parse(file);

	std::string _key = GetWeaponKey(SceneManager::Instance().GetPlayerWeaponInfo());

	std::vector<WeaponsStrengtheningInfo> weaponStrengtheningList = ReadWeaponStrengtheningList(data, _key);
	m_spWeapon->SetWeaponsStrengtheningInfo(weaponStrengtheningList);
}

std::vector<WeaponsStrengtheningInfo> GameScene::ReadWeaponStrengtheningList(const nlohmann::json& _data, const std::string& _key)
{
	std::vector<WeaponsStrengtheningInfo> list;

	if (!_data.contains(_key) || !_data[_key].is_object())
	{
		return list;
	}

	const nlohmann::json& weaponData = _data[_key];

	for (int i = 0; i < WeaponsStrengtheningSiz; ++i)
	{
		WeaponsStrengtheningInfo item;

		// 保存データがない項目は初期値のまま追加し、番号がずれないようにする
		if (weaponData.contains(kStrengtheningKeys[i]) && weaponData[kStrengtheningKeys[i]].is_object())
		{
			const nlohmann::json& itemData = weaponData[kStrengtheningKeys[i]];

			item.Lv = itemData.value("Lv", 0);
			item.nextLvNum = itemData.value("nextLvNum", 1);
			item.strengtheningItemNum = itemData.value("strengtheningItemNum", 0);
			item.strengtheningFlg = itemData.value("strengtheningFlg", false);
		}

		list.push_back(item);
	}

	return list;
}







void GameScene::Init()
{
	DebugInfo::Instance().SetSceneManagerImGUIFlg(false);
	MouseInfo::Instance().SetMouseFreeFlg(false);

	DisplayFloorInfo displayFloorInfo;
	m_DisplayFloorInfo.push_back(displayFloorInfo);



	/////////////////////////////////////////
	//ゲームシーン
	/////////////////////////////////////////
	auto self = shared_from_this();
	m_displayFloor = 0;

	/////////////////////////////////////////
	//カメラ
	/////////////////////////////////////////
	m_spCamera = std::make_shared<TPSCamera>();
	m_spCamera->Init();
	m_objList.push_back(m_spCamera);


	/////////////////////////////////////////
	//プレイヤー
	/////////////////////////////////////////
	m_spPlayer = std::make_shared<PlayerBase>();
	m_spPlayer->Init();
	m_spPlayer->SetPlayerWeaponInfo(SceneManager::Instance().GetPlayerWeaponInfo());
	m_spPlayer->LoadPlayerStatus(m_PlayerStatusFilePath);
	m_objList.push_back(m_spPlayer);

	LoadInventoryData();

	m_spCharacterStatus.push_back(m_spPlayer);

	/////////////////////////////////////////
	//武器
	/////////////////////////////////////////

	switch (SceneManager::Instance().GetPlayerWeaponInfo())
	{
	case PlayerWeaponInfo_Dagger:
		m_spWeapon = std::make_shared<Dagger>();
		break;
	case PlayerWeaponInfo_Sword:
		m_spWeapon = std::make_shared<Sword>();
		break;
	case PlayerWeaponInfoSize:
		break;
	default:
		break;
	}


	m_spWeapon->Init();
	LoadWeaponStrengtheningData();

	m_objList.push_back(m_spWeapon);

	/////////////////////////////////////////
	//マップ
	/////////////////////////////////////////
	m_spMapManager = std::make_shared<MapManager>();
	m_spMapManager->Init();
	m_objList.push_back(m_spMapManager);


	/////////////////////////////////////////
	//敵
	/////////////////////////////////////////
	m_spEnemyManager = std::make_shared<EnemyManager>();
	m_spEnemyManager->Init();
	m_objList.push_back(m_spEnemyManager);






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
	m_spUIManager = std::make_shared<UIManager>();
	m_spUIManager->Init();
	m_spUIManager->SetpPotionTexInfo();
	m_spUIManager->AddPlayerInventoryUI();
	m_spUIManager->AddUIMapManager();
	m_objList.push_back(m_spUIManager);


	/////////////////////////////////////////
	//アクセサリー
	/////////////////////////////////////////	
	std::shared_ptr<AccessoryManager> spAccessoryManager = std::make_shared<AccessoryManager>();
	spAccessoryManager->Init();
	m_objList.push_back(spAccessoryManager);

	/////////////////////////////////////////
	//UIにセット
	/////////////////////////////////////////
	m_spUIManager->SetPlayer(m_spPlayer);
	m_spUIManager->SetGameScene(self);
	m_spUIManager->SetPotionUseController(m_spPotionUseController);

	std::shared_ptr<BuffUI>spBuffUI = std::make_shared<BuffUI>();
	spBuffUI->Init();
	spBuffUI->Set2DPos({ -180,-320 });
	spBuffUI->SetPotionTexInfo(m_spUIManager->GetPotionTexInfo());
	spBuffUI->SetPlayer(m_spPlayer);
	m_spUIManager->AddUIObj(spBuffUI);

	std::shared_ptr<HitDamage>spHitDamage = std::make_shared<HitDamage>();
	spHitDamage->Init();
	spHitDamage->SetCamera(m_spCamera);
	m_spUIManager->AddUIObj(spHitDamage);

	std::shared_ptr<UIBase2>spUIBase2 = std::make_shared<UIBase2>();
	std::shared_ptr<KdTexture>spTex = std::make_shared<KdTexture>();
	spTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/inventoryType/Potion.png");
	spUIBase2->SetTex(spTex);
	spUIBase2->Set2DPos({ 580, 320 });
	spUIBase2->SetDrawKey(VK_TAB);
	spUIBase2->SetDrawKeyPos(DrawKeyPos_Bot);
	spUIBase2->SetDrawFlg(true);
	m_spUIManager->AddUIObj(spUIBase2);

	/////////////////////////////////////////
	//プレイヤーにセット
	/////////////////////////////////////////
	m_spPlayer->SetCamera(m_spCamera);
	m_spPlayer->SetGameScene(self);
	m_spPlayer->SetWepon(m_spWeapon);
	m_spPlayer->AddUIList(m_spUIManager);
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
	m_spEnemyManager->AddUIList(m_spUIManager);
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
	m_spMapManager->SetUIManager(m_spUIManager);
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

	/////////////////////////////////////////
	//マップオブジェクトにセット
	/////////////////////////////////////////
	m_spMapObjManager->SetGameScene(self);



	if (m_spCamera)
	{
		KdEffekseerManager::GetInstance().SetCamera(m_spCamera->GetCamera());
	}


	//マップの成長率を設定
	m_mapLinearGrowthPerFloor = 5;
	m_mapLinearGrowthPerFloorX = 5 * KdRandom::GetFloat(0.8f, 1.2f);
	m_mapLinearGrowthPerFloorY = 5 * KdRandom::GetFloat(0.8f, 1.2f);


	//音

	if (!m_spGameBGM)
	{
		m_spGameBGM = KdAudioManager::Instance().Play("Asset/Sound/BGM/Game/GameBGM.wav", true);
		m_spGameBGM->SetVolume(VOLInfo::Instance().GetMSBGMVol());
		m_spGameBGM->Stop();
	}

	if (!m_spBossBGM)
	{
		m_spBossBGM = KdAudioManager::Instance().Play("Asset/Sound/BGM/Game/BossBGM.wav", true);
		m_spBossBGM->SetVolume(VOLInfo::Instance().GetMSBGMVol());
		m_spBossBGM->Stop();
	}


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
	if (!m_spUIManager) { return; }

	//全effect停止
	KdEffekseerManager::GetInstance().StopAllEffect();

	int baseSize = 24;              // 1階のマップサイズ
	float growth = 1.01f;



	m_mapGenerateDisplayFloor = m_displayFloor;

	if (m_displayFloor > m_mapGenerateDisplayFloorMax)
	{
		m_mapGenerateDisplayFloor = m_mapGenerateDisplayFloorMax;
	}

	m_displayFloor++;
	DisplayFloorInfo displayFloorInfo;
	displayFloorInfo.DisplayFloor = m_displayFloor;
	m_DisplayFloorInfo.push_back(displayFloorInfo);

	m_DisplayFloorInfo[0].DisplayFloor = m_displayFloor;

	int mapSizeX = static_cast<int>((baseSize + m_mapGenerateDisplayFloor * m_mapLinearGrowthPerFloorX) * std::pow(growth, m_mapGenerateDisplayFloor));
	int mapSizeY = static_cast<int>((baseSize + m_mapGenerateDisplayFloor * m_mapLinearGrowthPerFloorY) * std::pow(growth, m_mapGenerateDisplayFloor));

	int baseRoomCount = 3;
	int roomCount = baseRoomCount + m_mapGenerateDisplayFloor * 2;




	m_spEnemyManager->EnemyListReset();
	m_spMapObjManager->ResetMapObj();

	if (m_displayFloor % m_bossInterval != 0)
	{
		if (m_spBossBGM)
		{
			m_spBossBGM->Stop();
		}

		if (m_spGameBGM && !m_spGameBGM->IsPlaying())
		{
			m_spGameBGM->SetVolume(VOLInfo::Instance().GetMSBGMVol() * 0.4);
			m_spGameBGM->Play();
		}

		m_spMapManager->GenerateMap({ (float)mapSizeX,(float)mapSizeY }, roomCount, MapType_Grassland);
	}
	else
	{

		if (m_spBossBGM)
		{
			m_spBossBGM->Stop();
		}

		if (m_spGameBGM && !m_spGameBGM->IsPlaying())
		{
			m_spGameBGM->SetVolume(VOLInfo::Instance().GetMSBGMVol() * 0.4);
			m_spGameBGM->Play();
		}

		m_spMapManager->GenerateMap({ (float)mapSizeX,(float)mapSizeY }, roomCount, MapType_Grassland);

		////ボス戦用マップ生成
		//mapSizeX = 7;
		//mapSizeY = 7;
		//m_spMapManager->GenerateBossMap({ (float)mapSizeX,(float)mapSizeY }, MapType_Grassland);
		//m_spUIManager->SetMinimapRevealAll(true);



		//if (m_spBossBGM)
		//{
		//	m_spBossBGM->SetVolume(VOLInfo::Instance().GetMSBGMVol() * 0.1);
		//	m_spBossBGM->Play();
		//}

		//if (m_spGameBGM)
		//{
		//	m_spGameBGM->Stop();
		//}
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

	m_spEnemyManager->SetVisionFlg(true);
}
