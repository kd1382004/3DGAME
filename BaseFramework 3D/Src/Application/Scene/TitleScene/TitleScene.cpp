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

void TitleScene::Init()
{
	//KdShaderManager::Instance().WorkAmbientController().SetDirLight({ 1,-1,1 }, { 0,0,0 });
	KdShaderManager::Instance().WorkAmbientController().SetDirLightShadowArea({ 200.0f, 200.0f }, 70.0f);

	//Fog(霧)
	//distance...距離
	//height ...高さ
	KdShaderManager::Instance().WorkAmbientController().SetFogEnable(true, false);

	//距離フォグの設定
	//col...色
	//density...密度
	KdShaderManager::Instance().WorkAmbientController().SetDistanceFog({ 0.35,0.35,0.38 }, 0.02);


	//アンビエント
	KdShaderManager::Instance().WorkAmbientController().SetAmbientLight({ 1,1,1,1 });



	m_spTitleSceneEditor = std::make_shared<TitleSceneEditor>();


	g_factory.RegisterCreateFunction("GameButton", []() {
		std::shared_ptr<GameButton>obj = std::make_shared<GameButton>();
		obj->Init();
		return obj;
		});


	SetObjeList();

	MouseInfo::Instance().SetMouseFreeFlg(true);

	DebugInfo::Instance().SetSceneManagerImGUIFlg(false);


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
	// TitelMove
	/////////////////////////////////////////
	m_spTitelMove = std::make_shared<TitelMove>();
	m_objList.push_back(m_spTitelMove);





	m_spTitelMove->SetMapManager(m_spMapManager);
	m_spCamera->SetTarget(m_spTitelMove);
	m_spMapManager->SetCamera(m_spCamera);
	m_spMapManager->SetMapObjManager(m_spMapObjManager);


	



	//
	Math::Vector2 siz = { 30,30 };
	m_spMapManager->GenerateMap(siz, 3, MapType_Grassland);

	m_spCamera->SetPos(m_spMapManager->GetPlayerSpawnPos());
	m_spTitelMove->SetPos(m_spMapManager->GetPlayerSpawnPos());

	m_spTitelMove->SetNextPos(m_spMapManager->GetRandomPos());
	m_spTitelMove->SetMoveFlg(true);

	m_spMapManager->SetPlayerChanke(m_spMapManager->GetPlayerSpawnPos());
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


void TitleScene::ImGUi()
{
	if (m_spTitleSceneEditor)
	{
		m_spTitleSceneEditor->ImGui();
	}
}

void TitleScene::Event()
{
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
			// 例: 60.0f = 1秒間に最大60度旋回（数値を小さくするほどゆっくり、大きくするほど素早く追従）
			m_spCamera->RotateTowardsY(targetYaw, 90.0f);
		}
	}


	m_spMapManager->SetPlayerChanke(m_spTitelMove->GetPos());

	//プレイヤーのあたり判定リストを毎フレーム更新
	m_spMapManager->MapHit(m_spTitelMove);

	if (m_spMapObjManager)
	{
		m_spMapObjManager->SetMapObjUpdateList(m_spMapManager);
	}
}
