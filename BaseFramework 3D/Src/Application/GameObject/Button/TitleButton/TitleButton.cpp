#include "TitleButton.h"
#include "../../../Scene/SceneManager.h"
#include"../../../Info/KeyInfo/KeyInfo.h"

void TitleButton::Init()
{
	m_tex = std::make_shared<KdTexture>();
	m_tex->Load("Asset/Textures/Button/TitleButton/TitleButton.png");
	m_scale = m_defScale;
	m_radius = { m_tex->GetWidth() / 2.0f * m_scale , m_tex->GetHeight() / 2.0f * m_scale };


	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_scale);
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0);

	m_mWorld = scaleMat * transMat;
}

void TitleButton::Update()
{

	if (m_choseFlg)
	{
		if (KeyInfo::GetInstance().KeyPush(VK_SPACE, true, true))
		{
			SceneManager::Instance().SetNextScene
			(
				SceneManager::SceneType::Title
			);
		}

		AlphaFlashing();
	}
	else
	{
		m_alpha = m_alphaMax;
	}
}

