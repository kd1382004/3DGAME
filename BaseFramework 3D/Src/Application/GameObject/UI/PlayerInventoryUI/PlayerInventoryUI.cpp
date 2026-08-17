#include "PlayerInventoryUI.h"

#include"../../../Info/KeyInfo/KeyInfo.h"
#include"../../../Scene/GameScene/GameScene.h"
#include"../../../Info/MouseInfo/MouseInfo.h"

void PlayerInventoryUI::Init()
{
	KeyInfo::Instance().SetKeyValid(VK_TAB);
	m_playerInventoryUIFlg = false;

	if (!m_back1Tex)
	{
		m_back1Tex = std::make_shared<KdTexture>();
		m_back1Tex->Load("Asset/Textures/GameUI/Item/PlayerInventory/BagBackTex1.png");
		m_back1Tex2DPos = {-100,0};

		if (!m_back2Tex)
		{
			m_back2Tex = std::make_shared<KdTexture>();
			m_back2Tex->Load("Asset/Textures/GameUI/Item/PlayerInventory/BagBackTex2.png");
			m_back2Tex2DPos = m_back1Tex2DPos + Math::Vector2{ (float)m_back1Tex->GetWidth() / 2,0 } + Math::Vector2{ (float)m_back2Tex->GetWidth(),0 };
		}
	}

}

void PlayerInventoryUI::Update()
{
	if (KeyInfo::Instance().GetValidKeyPush(VK_TAB, true))
	{
		m_playerInventoryUIFlg = !m_playerInventoryUIFlg;


		std::shared_ptr spGameScene = m_wpGameScene.lock();
		if (spGameScene)
		{
			//開いてたら止まるため
			spGameScene->SetPoseFlg(m_playerInventoryUIFlg);
		}

		MouseInfo::Instance().SetMouseFreeFlg(m_playerInventoryUIFlg);
	}


}

void PlayerInventoryUI::PreDraw()
{
}

void PlayerInventoryUI::DrawSprite()
{
	if (!m_playerInventoryUIFlg) { return; }
	
	
	if (!m_back1Tex) { return; }

	KdShaderManager::Instance().m_spriteShader.DrawTex(m_back1Tex, m_back1Tex2DPos.x, m_back1Tex2DPos.y);

	if (!m_back2Tex) { return; }

	KdShaderManager::Instance().m_spriteShader.DrawTex(m_back2Tex, m_back2Tex2DPos.x, m_back2Tex2DPos.y);

}
