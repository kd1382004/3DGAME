#include "TitleScene.h"
#include "../SceneManager.h"
#include"../../Editor/TitleSceneEditor/TitleSceneEditor.h"

#include"../../GameObject/Camera/TPSCamera/TPSCamera.h"

#include"../../Info/DebugInfo/DebugInfo.h"

#include"../../GameObject/Button/GameButton/GameButton.h"

#include"../../GameObject/Terrains/Ground/Ground.h"

void TitleScene::Init()
{
	m_spTitleSceneEditor = std::make_shared<TitleSceneEditor>();

	//実験用
	std::shared_ptr<TPSCamera>tPSCamera = std::make_shared<TPSCamera>();
	tPSCamera->Init();
	m_objList.push_back(tPSCamera);


	g_factory.RegisterCreateFunction("GameButton", []() {
		std::shared_ptr<GameButton>obj = std::make_shared<GameButton>();
		obj->Init();
		return obj;
		});

	g_factory.RegisterCreateFunction("Ground", []() {
		std::shared_ptr<Ground>obj = std::make_shared<Ground>();
		obj->Init();
		return obj;
		});

	SetObjeList();
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
}
