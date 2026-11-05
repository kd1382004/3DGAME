#include "ResultDisplay_PotionInventory.h"
#include"../../../../../Info/MouseInfo/MouseInfo.h"
#include"../../../../../Info/KeyInfo/KeyInfo.h"
#include"../../../../Character/Player/PlayerInventory/PlayerInventory.h"
#include"../../../../Potions/PotionTexInfo/PotionTexInfo.h"
void ResultDisplay_PotionInventory::Init()
{

}

void ResultDisplay_PotionInventory::Update()
{
	IconHit();
}

void ResultDisplay_PotionInventory::PreDraw()
{
}

void ResultDisplay_PotionInventory::DrawSprite()
{
	for (const auto& tex : m_itemIconInfo)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(tex.m_IconTex, tex.m_2DPos.x, tex.m_2DPos.y, m_iconDimensions.w, m_iconDimensions.h);

		if (tex.m_ItemID == m_selectPotionID)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(tex.m_ExplanationTex, tex.m_2DPos.x, tex.m_2DPos.y, m_iconDimensions.w, m_iconDimensions.h);
		}

	}
}

void ResultDisplay_PotionInventory::PotionInfoInit()
{

}
void ResultDisplay_PotionInventory::AddPotionTexInfo(const std::vector<Inventory>& _potions)
{
	m_itemIconInfo.clear();

	int i = 0;
	int j = 0;
	Math::Vector2 m_iconBase2DPos = { -475, 90 }; // 初期位置
	float m_iconSpacing = 10;

	for (const Inventory& potion : _potions)
	{
		if (potion.m_num > 0)
		{
			ItemIconInfo itemIconInfo;
			itemIconInfo.m_ItemID = potion.m_ID;

			if (i > 20)
			{
				j++;
				i = 0;
			}

			float x = m_iconBase2DPos.x + i * m_iconDimensions.w + i * m_iconSpacing;
			float y = m_iconBase2DPos.y - j * m_iconDimensions.h - j * m_iconSpacing;
			itemIconInfo.m_2DPos = { x,y };
			itemIconInfo.m_num = potion.m_num;
			itemIconInfo.m_name = potion.m_name;

			std::shared_ptr<PotionTexInfo>_spPotionTexInfo = m_wpPotionTexInfo.lock();
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




	bool flg = false;
	for (auto& potion : m_itemIconInfo)
	{
		if (m_selectPotionID == potion.m_ItemID)
		{
			flg = true;
		}
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


