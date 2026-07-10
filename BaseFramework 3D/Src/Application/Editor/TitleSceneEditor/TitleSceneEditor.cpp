#include "TitleSceneEditor.h"
#include"../../Scene/SceneManager.h"
#include"../../Info/DebugInfo/DebugInfo.h"

#include"../../GameObject/Button/GameButton/GameButton.h"

#include"../../GameObject/Terrains/Ground/Ground.h"

TitleSceneEditor::TitleSceneEditor()
{
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
}

TitleSceneEditor::~TitleSceneEditor()
{

}

void TitleSceneEditor::ImGui()
{
	std::list< std::shared_ptr<KdGameObject>>ObjeClassCamera;
	std::list< std::shared_ptr<KdGameObject>>ObjeClassBotton;
	std::list< std::shared_ptr<KdGameObject>>ObjeClassTerrains;


	for (auto obj : SceneManager::Instance().GetObjList())
	{
		switch (obj->GetGameObjectClass())
		{
		case KdGameObject::GameObjectClass::GameObjectClass_Camera:
			ObjeClassCamera.push_back(obj);
			break;

		case  KdGameObject::GameObjectClass::GameObjectClass_Botton:
			ObjeClassBotton.push_back(obj);
			break;

		case KdGameObject::GameObjectClass::GameObjectClass_Terrains:
			ObjeClassTerrains.push_back(obj);
			break;
		default:
			break;
		}
	}

	if (ImGui::Begin(U8("TitleSceneEditor"), nullptr, ImGuiWindowFlags_MenuBar))
	{
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu(U8("メニュー")))
			{
				if (ImGui::MenuItem(U8("全オブジェクトの情報を保存")))
				{

				}

				if (ImGui::MenuItem(U8("TitleSceneEditorを閉じる")))
				{
					DebugInfo::Instance().SetSceneManagerImGUIFlg(false);
				}

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::Text(U8("ObjListのサイズ : %d"), SceneManager::Instance().GetObjList().size());

		ImGui::Separator();

		CameraImGUI(ObjeClassCamera);

		ImGui::Separator();

		ButtonImGUI(ObjeClassBotton);

		ImGui::Separator();

		TerrainsImGUI(ObjeClassTerrains);
	}

	ImGui::End();
}

void TitleSceneEditor::CameraImGUI(std::list<std::shared_ptr<KdGameObject>>_obj)
{
	if (ImGui::TreeNode(U8("カメラクラス")))
	{
		ImGui::Separator();
		ImGui::Text(U8("カメラクラスの量 : %d"), _obj.size());

		int i = 0;
		for (auto obj : _obj)
		{

			std::string titleBer = obj->GetImGUIName() + std::to_string(i);
			if (ImGui::TreeNode((void*)(intptr_t)i, U8("%s"), titleBer.c_str()))
			{
				obj->ImGUI();

				ImGui::TreePop();
			}
			i++;
		}

		ImGui::TreePop();
	}
}

void TitleSceneEditor::ButtonImGUI(std::list<std::shared_ptr<KdGameObject>> _obj)
{
	if (ImGui::TreeNode(U8("ボタンクラス")))
	{
		ImGui::Separator();
		ImGui::Text(U8("ボタンクラスの量 : %d"), _obj.size());
		
		if (ImGui::TreeNode(U8("ボタンクラス追加")))
		{
			if (ImGui::Button("GameButton"))
			{
				auto gameBotton = g_factory.CreateGameObject("GameButton");
				SceneManager::Instance().AddObject(gameBotton);
			}

			ImGui::TreePop();
		}


		int i = 0;
		for (auto obj : _obj)
		{
			std::string titleBer = obj->GetImGUIName() + std::to_string(i);
			if (ImGui::TreeNode((void*)(intptr_t)i, U8("%s"), titleBer.c_str()))
			{
				obj->ImGUI();

				ImGui::TreePop();
			}
			i++;
		}

		ImGui::TreePop();
	}
}

void TitleSceneEditor::TerrainsImGUI(std::list<std::shared_ptr<KdGameObject>> _obj)
{
	if (ImGui::TreeNode(U8("地形クラス")))
	{
		ImGui::Separator();
		ImGui::Text(U8("地形クラスの量 : %d"), _obj.size());

		if (ImGui::TreeNode(U8("地形クラス追加")))
		{
			if (ImGui::Button("Ground"))
			{
				auto ground = g_factory.CreateGameObject("Ground");;
				SceneManager::Instance().AddObject(ground);
			}

			ImGui::TreePop();
		}


		int i = 0;
		for (auto obj : _obj)
		{
			std::string titleBer = obj->GetImGUIName() + std::to_string(i);
			if (ImGui::TreeNode((void*)(intptr_t)i, U8("%s"), titleBer.c_str()))
			{
				obj->ImGUI();

				ImGui::TreePop();
			}
			i++;
		}

		ImGui::TreePop();
	}
}
