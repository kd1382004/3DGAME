#include "ButtonManager.h"
#include"ButtonBase.h"
#include"../../Info/KeyInfo/KeyInfo.h"
#include "../../Object/Button/GameButton/GameButton.h"
#include "../../Object/Button/TitleButton/TitleButton.h"

void ButtonManager::InitTitle()
{
	m_buttonList.clear();
	m_posList.clear();

	std::shared_ptr<GameButton> spGameButton = std::make_shared<GameButton>();
	spGameButton->Init();
	spGameButton->SetPos({ 0,-200,0 });
	m_buttonList.push_back(spGameButton);


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

	std::shared_ptr<GameButton> spGameButton = std::make_shared<GameButton>();
	spGameButton->Init();
	spGameButton->SetPos({ -200,-300,0 });
	m_buttonList.push_back(spGameButton);

	std::shared_ptr<TitleButton> spTitleButton = std::make_shared<TitleButton>();
	spTitleButton->Init();
	spTitleButton->SetPos({ 200,-300,0 });
	m_buttonList.push_back(spTitleButton);

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

		m_spButtonChange->Play();
	}

	if (KeyInfo::GetInstance().KeyPush(VK_RIGHT, true, true))
	{
		m_posListNum++;

		if (m_posListNum >= m_posList.size())
		{
			m_posListNum = m_posList.size() - 1;
		}

		m_spButtonChange->Play();
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
			m_spButtonDecision->Play();
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

void ButtonManager::SetPos(const Math::Vector3& _pos)
{

}

void ButtonManager::Init()
{
	m_spButtonChange = KdAudioManager::Instance().Play("Asset/Sound/Button/Change.wav");
	m_spButtonChange->SetVolume(0.5);
	m_spButtonChange->Stop();
	m_spButtonDecision = KdAudioManager::Instance().Play("Asset/Sound/Button/Decision.wav");
	m_spButtonDecision->SetVolume(0.5);
	m_spButtonDecision->Stop();
}
