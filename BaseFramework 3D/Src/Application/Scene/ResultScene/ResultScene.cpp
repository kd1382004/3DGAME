#include "ResultScene.h"
#include"../SceneManager.h"
#include"../../Info/FadeDraw/FadeDraw.h"


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

	std::shared_ptr<ResultDisplay>spResultDisplay = std::make_shared<ResultDisplay>();

	if (gameResult.playerDead)
	{
		spResultDisplay = std::make_shared<ResultDisplay>();
		spResultDisplay->Init();
		spResultDisplay->Set2DPos({ 0,200 });
		spResultDisplay->SetDisplayFloorInfo(gameResult.displayFloorInfo);
		spUIManager->AddUIObj(spResultDisplay);

	}
	else
	{
	
	}

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
