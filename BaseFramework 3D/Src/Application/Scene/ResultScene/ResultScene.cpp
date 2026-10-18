#include "ResultScene.h"
#include"../SceneManager.h"
#include"../../Info/FadeDraw/FadeDraw.h"

#include"../../Info/DebugInfo/DebugInfo.h"

#include"../../Info/MouseInfo/MouseInfo.h"

#include"../../GameObject/UI/UIManager.h"
#include"../../GameObject/UI/ResultUI/ResultDisplay/ResultDisplay.h"

#include"../../GameObject/Button/GameButton/GameButton.h"
#include"../../GameObject/Button/TitleButton/TitleButton.h"
ResultScene::~ResultScene()
{
	if (m_spPlayerDeadBGM)
	{
		m_spPlayerDeadBGM->Stop();
	}

	if (m_spEscapeBGM)
	{
		m_spEscapeBGM->Stop();
	}
}
void ResultScene::ImGUi()
{

}

void ResultScene::Init()
{
	DebugInfo::Instance().SetSceneManagerImGUIFlg(false);
	GameResult gameResult = SceneManager::Instance().GetGameResult();
	std::shared_ptr<UIManager>spUIManager = std::make_shared<UIManager>();
	spUIManager->Init();
	m_objList.push_back(spUIManager);

	std::shared_ptr<ResultDisplay>spResultDisplay = std::make_shared<ResultDisplay>();


	spResultDisplay = std::make_shared<ResultDisplay>();
	spResultDisplay->Init();
	spResultDisplay->Set2DPos({ 0,200 });
	spResultDisplay->SetDisplayFloorInfo(gameResult.displayFloorInfo);
	spUIManager->AddUIObj(spResultDisplay);

	std::shared_ptr<GameButton>spGameButton = std::make_shared<GameButton>();
	spGameButton->Init();
	spGameButton->SetPos({ 500,-280,0 });
	m_objList.push_back(spGameButton);

	std::shared_ptr<TitleButton>spTitleButton = std::make_shared<TitleButton>();
	spTitleButton->Init();
	spTitleButton->SetPos({ -500,-280,0 });
	m_objList.push_back(spTitleButton);

	if (gameResult.playerDead)
	{
		if (!m_spPlayerDeadBGM)
		{
			m_spPlayerDeadBGM = KdAudioManager::Instance().Play("Asset/Sound/BGM/Result/PlayerDeadBGM.wav", true);
			m_spPlayerDeadBGM->SetVolume(VOLInfo::Instance().GetMSBGMVol() * 0.3);
		}
	}
	else
	{
		if (!m_spEscapeBGM)
		{
			m_spEscapeBGM = KdAudioManager::Instance().Play("Asset/Sound/BGM/Result/EscapeBGM.wav", true);
			m_spEscapeBGM->SetVolume(VOLInfo::Instance().GetMSBGMVol() * 0.3);
		}
	}
}

void ResultScene::Event()
{
	MouseInfo::Instance().SetMouseFreeFlg(true);
	if (GetAsyncKeyState('V') & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::Title
		);
	}
}
