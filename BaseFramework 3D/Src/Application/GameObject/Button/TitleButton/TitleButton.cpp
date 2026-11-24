//---- 自分のヘッダー ----
#include "TitleButton.h"							//タイトルシーンへ戻るボタン

//---- シーン ----
#include "../../../Scene/SceneManager.h"			//シーンの管理

//---- 便利機能 ----
#include"../../../Info/DebugInfo/DebugInfo.h"		//デバッグ用の状態
#include"../../../Info/KeyInfo/KeyInfo.h"			//キー入力

void TitleButton::Init()
{
	ButtonBase::Init();

	if (!m_spTex)
	{
		m_spTex = std::make_shared<KdTexture>();
		m_spTex->Load("Asset/Textures/Button/TitleButton/TitleButton.png");
		m_scale = m_defScale;
		m_diameter = { m_spTex->GetWidth() / 2.0f * m_scale , m_spTex->GetHeight() / 2.0f * m_scale };

		Math::Matrix scaleMat = Math::Matrix::CreateScale(m_scale);
		Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos.x, m_pos.y, 0);

		m_mWorld = scaleMat * transMat;


		KeyInfo::Instance().SetKeyValid(VK_LBUTTON);
	}


	m_ImGUIName = "TitleButton";
}

void TitleButton::Update()
{

	ButtonBase::Update();


	if (!DebugInfo::Instance().GetSceneManagerImGUIFlg())
	{
		ButtonBase::MouseChosen();

		if (m_choseFlg)
		{
			if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true, true))
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

}
