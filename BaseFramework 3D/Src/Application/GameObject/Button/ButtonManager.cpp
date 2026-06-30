#include "ButtonManager.h"
#include"ButtonBase.h"
#include"../../Info/KeyInfo/KeyInfo.h"
#include "../../Object/Button/GameButton/GameButton.h"
#include "../../Object/Button/TitleButton/TitleButton.h"

void ButtonManager::InitTitel()
{
	m_buttonList.clear();
	m_posList.clear();

	std::shared_ptr<GameButton> gameButton = std::make_shared<GameButton>();
	gameButton->Init();
	gameButton->SetPos({ 0,-200,0 });
	m_buttonList.push_back(gameButton);


	m_posListNum = 0;
	for (auto& obj : m_buttonList)
	{
		m_posList.push_back(obj->GetPos());
	}

	Init();
}

void ButtonManager::InitResult()
{
	m_buttonList.clear();
	m_posList.clear();

	std::shared_ptr<GameButton> gameButton = std::make_shared<GameButton>();
	gameButton->Init();
	gameButton->SetPos({ -200,-300,0 });
	m_buttonList.push_back(gameButton);

	std::shared_ptr<TitleButton> titleButton = std::make_shared<TitleButton>();
	titleButton->Init();
	titleButton->SetPos({ 200,-300,0 });
	m_buttonList.push_back(titleButton);

	for (auto& obj : m_buttonList)
	{
		m_posList.push_back(obj->GetPos());
	}

	Init();
}

void ButtonManager::Update()
{

	int oldPosListNum = m_posListNum;

	if (KeyInfo::GetInstance().KeyPush(VK_LEFT, true, true))
	{
		m_posListNum--;

		if (m_posListNum <= 0)
		{
			m_posListNum = 0;
		}

		m_bottonChange->Play();
	}

	if (KeyInfo::GetInstance().KeyPush(VK_RIGHT, true, true))
	{
		m_posListNum++;

		if (m_posListNum >= m_posList.size())
		{
			m_posListNum = m_posList.size() - 1;
		}

		m_bottonChange->Play();
	}

	int i = 0;
	for (auto& obj : m_buttonList)
	{
		if (i == m_posListNum)
		{
			obj->KeyChosen(true);
		}
		else
		{
			obj->KeyChosen(false);
		}

		obj->Update();

		if (KeyInfo::GetInstance().GetKeyPush(VK_SPACE))
		{
			m_bottonDecision->Play();
		}
		
		i++;
	}
}

void ButtonManager::DrawSprite()
{
	for (auto& obj : m_buttonList)
	{
		obj->DrawSprite();
	}

}

void ButtonManager::SetPos(const Math::Vector3& pos)
{

}

void ButtonManager::Init()
{
	m_bottonChange = KdAudioManager::Instance().Play("Asset/Sound/Button/Change.wav");
	m_bottonChange->SetVolume(0.5);
	m_bottonChange->Stop();
	m_bottonDecision = KdAudioManager::Instance().Play("Asset/Sound/Button/Decision.wav");
	m_bottonDecision->SetVolume(0.5);
	m_bottonDecision->Stop();
}
