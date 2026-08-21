#include "ResultScene.h"
#include"../SceneManager.h"
void ResultScene::ImGUi()
{
}

void ResultScene::Init()
{

	
}

void ResultScene::Event()
{
	if (GetAsyncKeyState('V') & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::Title
		);
	}
}
