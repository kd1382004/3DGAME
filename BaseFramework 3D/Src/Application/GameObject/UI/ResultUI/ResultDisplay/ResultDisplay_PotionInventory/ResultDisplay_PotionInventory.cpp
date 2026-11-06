#include "ResultDisplay_PotionInventory.h"
#include"../../../../../Info/MouseInfo/MouseInfo.h"
#include"../../../../../Info/KeyInfo/KeyInfo.h"
#include"../../../../Character/Player/PlayerInventory/PlayerInventory.h"
#include"../../../../Potions/PotionTexInfo/PotionTexInfo.h"
#include"../../../../../Info/NumDraw/NumDraw.h"
void ResultDisplay_PotionInventory::Init()
{
	m_iconDimensions = { 40,40 };

	if (!m_potionSelectTex)
	{
		m_potionSelectTex = std::make_shared<KdTexture>();
		m_potionSelectTex->Load("Asset/Textures/GameUI/Item/Potion/PotionSelect.png");
	}
	if (!m_potionTitleTex)
	{
		m_potionTitleTex = std::make_shared<KdTexture>();
		m_potionTitleTex->Load("Asset/Textures/GameUI/Result/ResultDisplay_PotionInventory/PotionTiyel.png");
		m_potionTitleTexPos = { 0,250 };
	
	}
	if (!m_potionBackTex)
	{
		m_potionBackTex = std::make_shared<KdTexture>();
		m_potionBackTex->Load("Asset/Textures/GameUI/Result/ResultDisplay_PotionInventory/BagBackTex2.png");
		m_potionBackTexPos = { 200,-70 };
	
	}
}

void ResultDisplay_PotionInventory::Update()
{
	IconHit();
}

void ResultDisplay_PotionInventory::PreDraw()
{}

void ResultDisplay_PotionInventory::DrawSprite()
{
	if (m_potionTitleTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_potionTitleTex, m_potionTitleTexPos.x, m_potionTitleTexPos.y);
	}
	if (m_potionBackTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_potionBackTex, m_potionBackTexPos.x, m_potionBackTexPos.y);
	
		Math::Vector2 pos = { -150,m_potionBackTexPos.y };
		Math::Vector2 siz = { 180,(float)m_potionBackTex->GetHeight() / 2 };
		Math::Color col = { 0.8,0.8,0.8,1 };
		KdShaderManager::Instance().m_spriteShader.DrawBox(pos.x, pos.y, siz.x, siz.y, &col);
	}



	for (const auto& tex : m_itemIconInfo)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(tex.m_IconTex, tex.m_2DPos.x, tex.m_2DPos.y, m_iconDimensions.w, m_iconDimensions.h);

		if (tex.m_ItemID == m_selectPotionID && m_potionSelectTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_potionSelectTex, tex.m_2DPos.x, tex.m_2DPos.y, m_iconDimensions.w, m_iconDimensions.h);
			KdShaderManager::Instance().m_spriteShader.DrawTex(tex.m_ExplanationTex, 200, -110);
			KdShaderManager::Instance().m_spriteShader.DrawTex(tex.m_NameTex, 200, 85);
			NumDraw::GetInstance().Drow(tex.m_num, RAligned, { 300,15 }, kWhiteColor, 4);
		}

	}
}

void ResultDisplay_PotionInventory::PotionInfoInit()
{

}
void ResultDisplay_PotionInventory::AddPotionTexInfo(std::vector<Inventory> _potions)
{
	m_itemIconInfo.clear();

	int i = 0;
	int j = 0;
	Math::Vector2 m_iconBase2DPos = { -300, 90 }; // 初期位置
	float m_iconSpacing = 10;

	for (const Inventory& potion : _potions)
	{
		if (potion.m_num > 0)
		{
			ItemIconInfo itemIconInfo;
			itemIconInfo.m_ItemID = potion.m_ID;

			if (i > 7)
			{
				j++;
				i = 0;
			}

			float x = m_iconBase2DPos.x + i * m_iconDimensions.w + i * m_iconSpacing;
			float y = m_iconBase2DPos.y - j * m_iconDimensions.h - j * m_iconSpacing;
			itemIconInfo.m_2DPos = { x,y };
			itemIconInfo.m_num = potion.m_num;
			itemIconInfo.m_name = potion.m_name;

			std::shared_ptr<PotionTexInfo>_spPotionTexInfo = std::make_shared<PotionTexInfo>();
			if (_spPotionTexInfo)
			{
				itemIconInfo.m_IconTex = _spPotionTexInfo->GetPotionIcon(itemIconInfo.m_ItemID);
				itemIconInfo.m_ExplanationTex = _spPotionTexInfo->GetPotionExplanation(itemIconInfo.m_ItemID);
				itemIconInfo.m_NameTex = _spPotionTexInfo->GetPotionName(itemIconInfo.m_ItemID);
			}

			m_itemIconInfo.push_back(itemIconInfo);
			i++;
		}
	}

	if (m_itemIconInfo.size() > 0)
	{
		m_selectPotionID = m_itemIconInfo[0].m_ItemID;
	}
}

void ResultDisplay_PotionInventory::IconHit()
{
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
			MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
			potion.m_hit = true;
			if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
			{
				m_selectPotionID = potion.m_ItemID;
			}
		}
		else
		{
			potion.m_hit = false;
		}
	}


}


