#include "PlayerInventoryUI.h"

#include"../../../Info/KeyInfo/KeyInfo.h"
#include"../../../Scene/GameScene/GameScene.h"
#include"../../../Info/MouseInfo/MouseInfo.h"

#include"../../Potions/PotionsType.h"
#include"../../Potions/PotionUseController.h"


#include"../../Character/Player/PlayerBase.h"
#include"../../Character/Player/PlayerInventory/PlayerInventory.h"

#include"../../Potions/PotionTexInfo/PotionTexInfo.h"

#include"../../../Info/NumDraw/NumDraw.h"
void PlayerInventoryUI::Init()
{
	KeyInfo::Instance().SetKeyValid(VK_TAB);
	m_playerInventoryUIFlg = false;

	if (!m_back1Tex)
	{
		m_back1Tex = std::make_shared<KdTexture>();
		m_back1Tex->Load("Asset/Textures/GameUI/Item/PlayerInventory/BagBackTex1.png");
		m_back1Tex2DPos = { -100,0 };

	}


	if (!m_notSelsect)
	{
		m_notSelsect = std::make_shared<KdTexture>();
		m_notSelsect->Load("Asset/Textures/GameUI/Item/PlayerInventory/BagBackTex2.png");
		m_back2Tex = m_notSelsect;
		m_back2Tex2DPos = m_back1Tex2DPos + Math::Vector2{ (float)m_back1Tex->GetWidth() / 2,0 } + Math::Vector2{ (float)m_back2Tex->GetWidth(),0 };

	}

	if (!m_UseTex)
	{
		m_UseTex = std::make_shared<KdTexture>();
		m_UseTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/UseTex.png");

		float x = m_back2Tex2DPos.x;
		float y = m_back2Tex2DPos.y - m_back2Tex->GetHeight() / 2 + m_UseTex->GetHeight();
		m_UseTex2DPos = { x,y };
	}

	m_iconDimensions = { 40,40 };
	m_layerPriority = 1;
}

void PlayerInventoryUI::PreUpdate()
{

}

void PlayerInventoryUI::Update()
{
	PlayerInventoryOpen();

	if (m_playerInventoryUIFlg)
	{
		AddPotionTexInfo();


		IconHit();


		PotionIUse();
	}


	//debaggu
	if (GetAsyncKeyState('1') & 0x8000)
	{
		std::shared_ptr<PlayerBase>spPlayer = m_wpPlayerBase.lock();
		if (spPlayer)
		{
			std::shared_ptr<PlayerInventory> spPlayerInventory = spPlayer->GetPlayerInventory();
			if (!spPlayerInventory) { return; }
			for (Inventory potion : spPlayerInventory->GetPotionsInventory())
			{
				spPlayer->GetPlayerInventory()->AddPotionsInventory(potion.m_ID);
			}
		}
	}

}

void PlayerInventoryUI::PreDraw()
{}

void PlayerInventoryUI::DrawSprite()
{
	if (!m_playerInventoryUIFlg) { return; }


	if (m_back1Tex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_back1Tex, m_back1Tex2DPos.x, m_back1Tex2DPos.y);
	}



	if (m_back2Tex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_back2Tex, m_back2Tex2DPos.x, m_back2Tex2DPos.y);

		Math::Vector2 pos = m_back2Tex2DPos;
		pos.y += 150;
		pos.x += 80;
		NumDraw::GetInstance().Drow(m_num, RAligned, pos, kWhiteColor, 4);
	}



	if (m_UseTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_UseTex, m_UseTex2DPos.x, m_UseTex2DPos.y);


	}




	for (auto tex : m_itemIconInfo)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(tex.m_IconTex, tex.m_2DPos.x, tex.m_2DPos.y, m_iconDimensions.w, m_iconDimensions.h);
	}

}

void PlayerInventoryUI::PlayerInventoryOpen()
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


		if (m_playerInventoryUIFlg)
		{
			AddPotionTexInfo();
			if (m_itemIconInfo.size() > 0)
			{
				m_back2Tex = m_itemIconInfo[0].m_ExplanationTex;
				m_selectPotionID = m_itemIconInfo[0].m_ItemID;
			}
			else
			{
				m_back2Tex = m_notSelsect;
			}

		}
	}

}

void PlayerInventoryUI::AddPotionTexInfo()
{
	std::shared_ptr<PlayerBase >spPlayer = m_wpPlayerBase.lock();
	if (!spPlayer) { return; }

	std::shared_ptr<PlayerInventory> spPlayerInventory = spPlayer->GetPlayerInventory();
	if (!spPlayerInventory) { return; }

	m_itemIconInfo.clear();

	int i = 0;
	Math::Vector2 m_iconBase2DPos = { -470, 90 }; // 初期位置
	float m_iconSpacing = 5;
	for (Inventory potion : spPlayerInventory->GetPotionsInventory())
	{
		if (potion.m_num > 0)
		{
			ItemIconInfo itemIconInfo;
			itemIconInfo.m_ItemID = potion.m_ID;
			itemIconInfo.m_2DPos = { m_iconBase2DPos.x + i * m_iconDimensions.w + i * m_iconSpacing, m_iconBase2DPos.y };
			itemIconInfo.m_num = potion.m_num;
			itemIconInfo.m_name = potion.m_name;

			std::shared_ptr<PotionTexInfo>_spPotionTexInfo = m_wpPotionTexInfo.lock();
			if (_spPotionTexInfo)
			{
				itemIconInfo.m_IconTex = _spPotionTexInfo->GetIcon(itemIconInfo.m_ItemID);
				itemIconInfo.m_ExplanationTex = _spPotionTexInfo->GetExplanation(itemIconInfo.m_ItemID);
			}


			m_itemIconInfo.push_back(itemIconInfo);
			i++;
		}
		else
		{
			if (potion.m_ID == m_selectPotionID)
			{
				m_selectPotionID = -999;
			}
		}
	}
}

void PlayerInventoryUI::IconHit()
{
	if (m_itemIconInfo.size() <= 0)
	{
		m_back2Tex = m_notSelsect;
		return;
	}


	POINT mousePos = MouseInfo::Instance().m_windowPos;

	for (auto& potion : m_itemIconInfo)
	{
		float Left = potion.m_2DPos.x - m_iconDimensions.w / 2;
		float Right = potion.m_2DPos.x + m_iconDimensions.w / 2;
		float Top = potion.m_2DPos.y + m_iconDimensions.h / 2;
		float Bot = potion.m_2DPos.y - m_iconDimensions.h / 2;


		if (mousePos.x >= Left && mousePos.x <= Right &&
			mousePos.y >= Bot && mousePos.y <= Top)
		{
			potion.m_hit = true;
			if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
			{
				m_back2Tex = potion.m_ExplanationTex;
				m_selectPotionID = potion.m_ItemID;
			}
		}
		else
		{
			potion.m_hit = false;
		}
	}


}

void PlayerInventoryUI::PotionIUse()
{
	if (m_itemIconInfo.size() <= 0 || m_selectPotionID == -999) { return; }
	std::shared_ptr<PotionUseController >spPotionUseController = m_wpPotionUseController.lock();
	if (!spPotionUseController) { return; }

	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayerBase.lock();
	if (!spPlayer) { return; }

	POINT mousePos = MouseInfo::Instance().m_windowPos;


	float Left = m_UseTex2DPos.x - m_UseTex->GetWidth() / 2;
	float Right = m_UseTex2DPos.x + m_UseTex->GetWidth() / 2;
	float Top = m_UseTex2DPos.y + m_UseTex->GetHeight() / 2;
	float Bot = m_UseTex2DPos.y - m_UseTex->GetHeight() / 2;

	if (mousePos.x >= Left && mousePos.x <= Right &&
		mousePos.y >= Bot && mousePos.y <= Top)
	{
		if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
		{

			spPotionUseController->SetPlayer(spPlayer);
			spPotionUseController->PotionUse(m_selectPotionID);
			spPlayer->GetPlayerInventory()->UsePotionsInventory(m_selectPotionID);


		}
	}


	m_num = spPlayer->GetPlayerInventory()->GetPotionsInventoryNum(m_selectPotionID);
}
