#include "GameButton.h"
#include "../../../Scene/SceneManager.h"
#include"../../../Info/DebugInfo/DebugInfo.h"
#include"../../../Info/KeyInfo/KeyInfo.h"

void GameButton::Init()
{
	ButtonBase::Init();

	if (!m_spTex)
	{
		m_spTex = std::make_shared<KdTexture>();
		m_spTex->Load(m_texPath);
		m_scale = m_defScale;
		m_diameter = { m_spTex->GetWidth() / 2.0f * m_scale , m_spTex->GetHeight() / 2.0f * m_scale };

		Math::Matrix scaleMat = Math::Matrix::CreateScale(m_scale);
		Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0);

		m_mWorld = scaleMat * transMat;


		KeyInfo::Instance().SetKeyValid(VK_LBUTTON);
	}


	m_ImGUIName = "GameButtom";
}

void GameButton::Update()
{
	// 表示しないときは押せない
	if (!m_activeFlg) { return; }

	ButtonBase::Update();


	if (!DebugInfo::Instance().GetSceneManagerImGUIFlg())
	{
		ButtonBase::MouseChosen();

		if (m_choseFlg)
		{
			if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true, true))
			{
				if (m_onPush)
				{
					m_onPush();
				}
				else
				{
					SceneManager::Instance().SetNextScene
					(
						SceneManager::SceneType::Game
					);
				}
			}


			AlphaFlashing();
		}
		else
		{
			m_alpha = m_alphaMax;
		}
	}

}

