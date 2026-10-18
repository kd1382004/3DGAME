#include "TitleScene.h"
#include "../SceneManager.h"
#include"../../Editor/TitleSceneEditor/TitleSceneEditor.h"

#include"../../GameObject/Camera/TPSCamera/TPSCamera.h"

#include"../../Info/DebugInfo/DebugInfo.h"

#include"../../GameObject/Button/GameButton/GameButton.h"

#include"../../Info/MouseInfo/MouseInfo.h"

#include"../../GameObject/Terrains/Map/MapManager.h"
#include"../../GameObject/Terrains/MapObj/MapObjManager.h"
#include"../../GameObject/TitelMove/TitelMove.h"
#include"../../GameObject/Character/Enemy/EnemyManager.h"
#include"../../GameObject/Character/Enemy/EnemyBase.h"


#include"../../GameObject/UI/VOLUI/VOLUI.h"
#include"../../GameObject/UI/UIManager.h"
#include"../../GameObject/UI/DrawTex/DrawTex.h"



void TitleScene::Init()
{
	DebugInfo::Instance().SetSceneManagerImGUIFlg(false);

	m_spTitleSceneEditor = std::make_shared<TitleSceneEditor>();


	g_factory.RegisterCreateFunction("GameButton", []() {
		std::shared_ptr<GameButton>obj = std::make_shared<GameButton>();
		obj->Init();
		return obj;
		});


	SetObjeList();

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
	// TitelMove
	/////////////////////////////////////////
	m_spTitelMove = std::make_shared<TitelMove>();
	m_objList.push_back(m_spTitelMove);


	/////////////////////////////////////////
	// UI
	/////////////////////////////////////////
	std::shared_ptr<UIManager>spUIManager = std::make_shared<UIManager>();
	spUIManager->Init();
	m_objList.push_back(spUIManager);


	std::shared_ptr<DrawTex>spDrawTex = std::make_shared<DrawTex>();
	spDrawTex->Init();
	spDrawTex->Set2DPos({ 0,200 });

	spDrawTex->LoadTex("Asset/Textures/Tite/紅フードと不思議の迷宮.png");
	spUIManager->AddUIObj(spDrawTex);


	std::shared_ptr<VOLUI>spVOLUI = std::make_shared<VOLUI>();
	spVOLUI->Init();
	spVOLUI->SetVolPos({ 400,-300 });
	spUIManager->AddUIObj(spVOLUI);
	/////////////////////////////////////////
	m_spTitelMove->SetMapManager(m_spMapManager);
	m_spCamera->SetTarget(m_spTitelMove);
	m_spMapManager->SetCamera(m_spCamera);
	m_spMapManager->SetMapObjManager(m_spMapObjManager);
	m_spMapManager->SetEnemyManager(m_spEnemyManager);
	m_spMapObjManager->ObjSetCamera(m_spCamera);

	m_spEnemyManager->SetMapManager(m_spMapManager);
	m_spEnemyManager->SetCamera(m_spCamera);
	if (m_spCamera)
	{
		KdEffekseerManager::GetInstance().SetCamera(m_spCamera->GetCamera());
	}



	//
	Math::Vector2 siz = { 30,30 };
	m_spMapManager->GenerateMap(siz, 3, MapType_Grassland);

	m_spCamera->SetPos(m_spMapManager->GetPlayerSpawnPos());
	m_spTitelMove->SetPos(m_spMapManager->GetPlayerSpawnPos());

	m_spTitelMove->SetNextPos(m_spMapManager->GetRandomPos());
	m_spTitelMove->SetMoveFlg(true);

	m_spMapManager->SetPlayerChanke(m_spMapManager->GetPlayerSpawnPos());

	//音
	if (!m_spTitelBGM)
	{
		m_spTitelBGM = KdAudioManager::Instance().Play("Asset/Sound/BGM/Titel/TitelBGM.wav", true);
		if (m_spTitelBGM)
		{
			m_spTitelBGM->SetVolume(VOLInfo::Instance().GetBGMVol());
		}
	}
}

void TitleScene::SetObjeList()
{
	//nlohmann::json root;

	//for (const auto& obj : m_objList) {
	//	nlohmann::json item;
	//	item["name"] = obj->GetObjName();
	//	item["data_path"] = obj->GetFilePass();   // ここにファイルパスを入れる

	//	root["objects"].push_back(item);
	//}

	//std::ofstream ofs("Asset/Data/ObjeData/TitelObjeData/TitelObjeListData/TitelObjeListData.json");
	//ofs << root.dump(4);


	nlohmann::json root;
	std::ifstream ifs("Asset/Data/ObjeData/TitelObjeData/TitelObjeListData/TitelObjeListData.json");
	ifs >> root;

	for (auto& item : root["objects"]) {
		std::string name = item["name"];
		std::string dataPath = item["data_path"];

		auto obj = g_factory.CreateGameObject(name);
		if (obj)
		{
			obj->SetFilePass(dataPath);
			obj->DataLodo();
			m_objList.push_back(obj);
		}

	}
}


TitleScene::~TitleScene()
{
	if (m_spTitelBGM)
	{
		m_spTitelBGM->Stop();
	}
}

void TitleScene::ImGUi()
{
	if (m_spTitleSceneEditor)
	{
		m_spTitleSceneEditor->ImGui();
	}
}

void TitleScene::Event()
{
	if (m_spTitelBGM)
	{
		m_spTitelBGM->SetVolume(VOLInfo::Instance().GetMSBGMVol() * 0.5);
	}

	MouseInfo::Instance().SetMouseFreeFlg(true);

	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		DebugInfo::Instance().SetSceneManagerImGUIFlg(true);
	}


	if (m_spCamera && m_spTitelMove)
	{
		const Math::Vector3& moveVec = m_spTitelMove->GetMoveVec();
		// 移動ベクトルがある時のみ回転（停止時は直前の向きをキープ）
		if (moveVec.LengthSquared() > 0.0001f)
		{
			// XZ平面の進行方向からYaw角度(度数法)を算出
			float targetYaw = DirectX::XMConvertToDegrees(std::atan2(moveVec.x, moveVec.z));
			// 第2引数で最大回転速度（度/秒）を指定して回転
			m_spCamera->RotateTowardsY(targetYaw, 60.0f);
		}
	}


	m_spMapManager->SetPlayerChanke(m_spTitelMove->GetPos());

	//プレイヤーのあたり判定リストを毎フレーム更新
	m_spMapManager->MapHit(m_spTitelMove);

	if (m_spMapObjManager)
	{
		m_spMapObjManager->SetMapObjUpdateList(m_spMapManager);
	}

	if (m_spEnemyManager)
	{
		m_spEnemyManager->SetEnemyUpdateList(m_spMapManager);
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
}
