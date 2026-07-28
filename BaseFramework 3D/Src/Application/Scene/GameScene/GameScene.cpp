#include "GameScene.h"
#include"../SceneManager.h"
#include"../../GameObject/Character/Player/PlayerBase.h"
#include"../../GameObject/Terrains/Ground/Ground.h"
#include"../../GameObject/Camera/TPSCamera/TPSCamera.h"


//マップ
#include"../../GameObject/Terrains/Map/MapManager.h"

#include"../../../Application/Info/DebugInfo/DebugInfo.h"
void GameScene::ImGUi()
{
	for (auto Camera : m_spCharacterStatus)
	{
		Camera->ImGUI();
	}
}

void GameScene::Event()
{
	if (GetAsyncKeyState('T') & 0x8000)
	{
		GenerateMap();
	}
}


void GameScene::Init()
{
	DebugInfo::Instance().SetSceneManagerImGUIFlg(true);

	/////////////////////////////////////////
	//プレイヤー
	/////////////////////////////////////////
	m_spPlayer = std::make_shared<PlayerBase>();
	m_spPlayer->Init();
	m_objList.push_back(m_spPlayer);

	m_spCharacterStatus.push_back(m_spPlayer);



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
	//プレイヤーにセット
	/////////////////////////////////////////
	m_spPlayer->SetCamera(camera);

	/////////////////////////////////////////
	//カメラにセット
	/////////////////////////////////////////
	camera->SetTarget(m_spPlayer);

	/////////////////////////////////////////
	//マップにセット
	/////////////////////////////////////////
	m_spMapManager->SetCamera(camera);
	m_spMapManager->SetPlayer(m_spPlayer);

	//マップ生成
	GenerateMap();
}

//マップ生成
void GameScene::GenerateMap()
{
	if (!m_spMapManager) { return; }
	if (!m_spPlayer) { return; }
	m_spMapManager->GenerateMap();
	m_spMapManager->MapHit(m_spPlayer);


	Math::Vector3 playerSpwn = m_spMapManager->GetPlayerSpawnPos();
	playerSpwn.y += 1;
	m_spPlayer->SetPos(playerSpwn);
}
