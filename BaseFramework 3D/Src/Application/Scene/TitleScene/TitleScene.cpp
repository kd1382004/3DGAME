//---- 自分のヘッダー ----
#include "TitleScene.h"												//タイトルシーン

//---- シーン ----
#include "../SceneManager.h"										//シーンの管理

//---- カメラ ----
#include"../../GameObject/Camera/TPSCamera/TPSCamera.h"				//三人称カメラ

//---- キャラクター ----
#include"../../GameObject/TitelMove/TitelMove.h"					//タイトル画面で歩き回るキャラ

//---- 敵 ----
#include"../../GameObject/Character/Enemy/EnemyManager.h"			//敵の管理
#include"../../GameObject/Character/Enemy/EnemyBase.h"				//敵の基底クラス

//---- マップ ----
#include"../../GameObject/Terrains/Map/MapManager.h"				//マップの管理・経路探索

//---- マップオブジェクト ----
#include"../../GameObject/Terrains/MapObj/MapObjManager.h"			//マップオブジェクトの管理

//---- UI ----
#include"../../GameObject/UI/VOLUI/VOLUI.h"							//音量設定画面
#include"../../GameObject/UI/UIManager.h"							//UIの管理
#include"../../GameObject/UI/DrawTex/DrawTex.h"						//画像を1枚描くUI

//---- ボタン ----
#include"../../GameObject/Button/GameButton/GameButton.h"			//ゲームシーンへ進むボタン
#include"../../GameObject/Button/PrepareButton/PrepareButton.h"		//準備シーンへ進むボタン

//---- エディター ----
#include"../../Editor/TitleSceneEditor/TitleSceneEditor.h"			//タイトルシーンの配置編集エディター

//---- 便利機能 ----
#include"../../Info/DebugInfo/DebugInfo.h"							//デバッグ用の状態
#include"../../Info/MouseInfo/MouseInfo.h"							//マウス入力

//=============================================
// コンストラクタ・デストラクタ
//=============================================

TitleScene::~TitleScene()
{
	if (m_spTitleBGM)
	{
		m_spTitleBGM->Stop();
	}
}

//=============================================
// 基本の処理
//=============================================

void TitleScene::ImGUI()
{
	if (ImGui::Begin(U8("TitleSceneEditor"), nullptr, ImGuiWindowFlags_MenuBar))
	{
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu(U8("メニュー")))
			{
				if (ImGui::MenuItem(U8("TitleSceneEditorを閉じる")))
				{
					DebugInfo::Instance().SetSceneManagerImGUIFlg(false);
				}

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
	}

	if (ImGui::Button(U8("武器をダガーに")))
	{
		SceneManager::Instance().SetPlayerWeaponInfo(PlayerWeaponInfo::PlayerWeaponInfo_Dagger);
	}

	if (ImGui::Button(U8("武器を剣に")))
	{
		SceneManager::Instance().SetPlayerWeaponInfo(PlayerWeaponInfo::PlayerWeaponInfo_Sword);
	}




	ImGui::End();

}

void TitleScene::Init()
{
	DebugInfo::Instance().SetSceneManagerImGUIFlg(false);
	
	m_spTitleSceneEditor = std::make_shared<TitleSceneEditor>();


	m_factory.RegisterCreateFunction("GameButton", []() {
		std::shared_ptr<GameButton>spObj = std::make_shared<GameButton>();
		spObj->Init();
		return spObj;
		});

	m_factory.RegisterCreateFunction("PrepareButton", []() {
		std::shared_ptr<PrepareButton>spObj = std::make_shared<PrepareButton>();
		spObj->Init();
		return spObj;
		});


	SetObjectList();

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
	m_spTitleMove = std::make_shared<TitelMove>();
	m_objList.push_back(m_spTitleMove);


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
	m_spTitleMove->SetMapManager(m_spMapManager);
	m_spTitleMove->SetCamera(m_spCamera);


	m_spCamera->SetTarget(m_spTitleMove);
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
	Math::Vector2 size = { 50,50 };
	m_spMapManager->GenerateMap(size, 20, MapType_Dungeon);

	m_spCamera->SetPos(m_spMapManager->GetPlayerSpawnPos());
	m_spTitleMove->SetPos(m_spMapManager->GetPlayerSpawnPos());

	m_spTitleMove->SetNextPos(m_spMapManager->GetRandomPos());
	m_spTitleMove->SetMoveFlg(true);

	m_spMapManager->SetPlayerChunk(m_spMapManager->GetPlayerSpawnPos());

	//音
	if (!m_spTitleBGM)
	{
		m_spTitleBGM = KdAudioManager::Instance().Play("Asset/Sound/BGM/Titel/TitelBGM.wav", true);
		if (m_spTitleBGM)
		{
			m_spTitleBGM->SetVolume(VOLInfo::Instance().GetBGMVol());
		}
	}
}

//=============================================
// 設定
//=============================================

void TitleScene::SetObjectList()
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

		auto obj = m_factory.CreateGameObject(name);
		if (obj)
		{
			obj->SetFilePass(dataPath);
			obj->DataLoad();
			m_objList.push_back(obj);
		}

	}
}

//=============================================
// その他の処理
//=============================================

void TitleScene::Event()
{
	if (m_spTitleBGM)
	{
		m_spTitleBGM->SetVolume(VOLInfo::Instance().GetMSBGMVol() * 0.5);
	}

	MouseInfo::Instance().SetMouseFreeFlg(true);

	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		DebugInfo::Instance().SetSceneManagerImGUIFlg(true);
	}

	m_spMapManager->SetPlayerChunk(m_spTitleMove->GetPos());

	//プレイヤーのあたり判定リストを毎フレーム更新
	m_spMapManager->MapHit(m_spTitleMove);

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
