#include "TitleSceneEditor.h"
#include"../../Scene/SceneManager.h"
#include"../../Info/DebugInfo/DebugInfo.h"

TitleSceneEditor::TitleSceneEditor()
{

}

TitleSceneEditor::~TitleSceneEditor()
{

}

void TitleSceneEditor::ImGui()
{
	std::list< std::shared_ptr<KdGameObject>>ObjeClassCamera;


	for (auto obj : SceneManager::Instance().GetObjList())
	{
		switch (obj->GetGameObjectClass())
		{
		case KdGameObject::GameObjectClass::GameObjectClass_Camera:
			ObjeClassCamera.push_back(obj);
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
					DebugInfo::Instance().m_SceneManagerImGUIFlg = false;
				}

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::Text(U8("ObjListのサイズ : %d"), SceneManager::Instance().GetObjList().size());

		ImGui::Separator();


		if (ObjeClassCamera.size() > 0)
		{
			if (ImGui::TreeNode(U8("カメラクラス")))
			{
				ImGui::Separator();
				ImGui::Text(U8("カメラクラスの量 : %d"), ObjeClassCamera.size());

				int i = 0;
				for (auto obj : ObjeClassCamera)
				{
					if (ImGui::TreeNode((void*)(intptr_t)i, U8("カメラクラス %d"), i))
					{


						ImGui::TreePop();
					}
					i++;
				}

				ImGui::TreePop();
			}

			ImGui::Separator();
		}
	}

	ImGui::End();
}
