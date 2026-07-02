#include "TitleScene.h"
#include "../SceneManager.h"
#include"../../Editor/TitleSceneEditor/TitleSceneEditor.h"

#include"../../GameObject/Camera/TPSCamera/TPSCamera.h"


void TitleScene::Init()
{
	m_spTitleSceneEditor = std::make_shared<TitleSceneEditor>();

	//実験用
	std::shared_ptr<TPSCamera>tPSCamera = std::make_shared<TPSCamera>();
	tPSCamera->Init();
	m_objList.push_back(tPSCamera);
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
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::Game
		);
	}
}
