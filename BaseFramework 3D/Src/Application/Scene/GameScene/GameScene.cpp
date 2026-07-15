#include "GameScene.h"
#include"../SceneManager.h"
#include"../../GameObject/Character/Player/PlayerBase.h"
#include"../../GameObject/Terrains/Ground/Ground.h"
#include"../../GameObject/Camera/TPSCamera/TPSCamera.h"

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

	std::shared_ptr<Ground>ground = std::make_shared<Ground>();
	ground->Init();
	m_objList.push_back(ground);

	std::shared_ptr<TPSCamera>camera = std::make_shared<TPSCamera>();
	camera->Init();
	m_objList.push_back(camera);

	//それぞれにセット
	camera->SetTarget(player);

	player->SetCamera(camera);
	player->RegistHitObject(ground);
}
