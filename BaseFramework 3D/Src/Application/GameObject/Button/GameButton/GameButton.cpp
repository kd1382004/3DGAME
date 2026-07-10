#include "GameButton.h"
#include "../../../Scene/SceneManager.h"
#include"../../../Info/DebugInfo/DebugInfo.h"
#include"../../../Info/KeyInfo/KeyInfo.h"

void GameButton::Init()
{
	ButtonBase::Init();

	if (!m_tex)
	{
		m_tex = std::make_shared<KdTexture>();
		m_tex->Load("Asset/Textures/Button/GameButton/PlayButton.png");
		m_scale = m_defScale;
		m_diameter = { m_tex->GetWidth() / 2.0f * m_scale , m_tex->GetHeight() / 2.0f * m_scale };

		Math::Matrix scaleMat = Math::Matrix::CreateScale(m_scale);
		Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0);

		m_mWorld = scaleMat * transMat;
	}


	m_ImGUIName = "GameButtom";
}

void GameButton::Update()
{

	ButtonBase::Update();


	if (!DebugInfo::Instance().GetSceneManagerImGUIFlg())
	{
		ButtonBase::MouseChosen();

		if (m_choseFlg)
		{
			KeyInfo::Instance().SetKeyValid('Q');
			if (KeyInfo::Instance().GetValidKeyPush('Q', true, true))
			{
				SceneManager::Instance().SetNextScene
				(
					SceneManager::SceneType::Game
				);
			}

		
			AlphaFlashing();
		}
		else
		{
			m_alpha = m_alphaMax;
		}
	}

}

