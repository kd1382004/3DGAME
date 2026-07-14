#include "GameScene.h"
#include"../SceneManager.h"
#include"../../GameObject/Character/Player/PlayerBase.h"


void GameScene::Event()
{
	if (GetAsyncKeyState('T') & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::Title
		);
	}
}

void GameScene::Init()
{

	std::shared_ptr<PlayerBase>player = std::make_shared<PlayerBase>();
	player->Init();
	m_objList.push_back(player);
}
