#include "ResultScene.h"
#include"../SceneManager.h"

#include"../../Info/MouseInfo/MouseInfo.h"
void ResultScene::ImGUi()
{
}

void ResultScene::Init()
{

	MouseInfo::Instance().SetMouseFreeFlg(true);
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
