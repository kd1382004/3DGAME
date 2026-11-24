//---- 自分のヘッダー ----
#include "PrepareButton.h"							//準備シーンへ進むボタン

//---- シーン ----
#include "../../../Scene/SceneManager.h"			//シーンの管理

//---- 便利機能 ----
#include"../../../Info/DebugInfo/DebugInfo.h"		//デバッグ用の状態
#include"../../../Info/KeyInfo/KeyInfo.h"			//キー入力

void PrepareButton::Init()
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


	m_ImGUIName = "PrepareButton";
}

void PrepareButton::Update()
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
					SceneManager::SceneType::Prepare
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
