#include "ResultScene.h"
#include"../SceneManager.h"

#include"../../Info/MouseInfo/MouseInfo.h"

#include"../../GameObject/UI/UIManager.h"
#include"../../GameObject/UI/ResultUI/ResultDisplay/ResultDisplay.h"
void ResultScene::ImGUi()
{

}

void ResultScene::Init()
{
	GameResult gameResult = SceneManager::Instance().GetGameResult();







	MouseInfo::Instance().SetMouseFreeFlg(true);
	std::shared_ptr<UIManager>spUIManager = std::make_shared<UIManager>();
	spUIManager->Init();
	m_objList.push_back(spUIManager);




	//倒した敵の数
	std::shared_ptr<ResultDisplay>spResultDisplay = std::make_shared<ResultDisplay>();
	spResultDisplay->Init();
	spResultDisplay->SetPath("Asset/Textures/GameUI/Result/EnemyKill.png");
	spResultDisplay->Set2DPos({-300,0});
	spResultDisplay->SetNum(gameResult.enemyKillCount);
	spUIManager->AddUIObj(spResultDisplay);

	spResultDisplay = std::make_shared<ResultDisplay>();
	spResultDisplay->Init();
	spResultDisplay->SetPath("Asset/Textures/GameUI/Result/floorReached.png");
	spResultDisplay->Set2DPos({-300,-110});
	spResultDisplay->SetNum(gameResult.floorReached);
	spUIManager->AddUIObj(spResultDisplay);

	spResultDisplay = std::make_shared<ResultDisplay>();
	spResultDisplay->Init();
	spResultDisplay->SetPath("Asset/Textures/GameUI/Result/ResultTitle.png");
	spResultDisplay->Set2DPos({ 0,200 });
	spResultDisplay->SetNumDraw(false);
	spUIManager->AddUIObj(spResultDisplay);
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
