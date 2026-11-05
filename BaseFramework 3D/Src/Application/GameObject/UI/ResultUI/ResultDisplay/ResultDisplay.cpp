#include "ResultDisplay.h"

#include"ResultDisplayBase.h"
#include"ResultDisplay_Score/ResultDisplay_Score.h"
#include"ResultDisplay_PotionInventory/ResultDisplay_PotionInventory.h"

#include"../../../../Info/MouseInfo/MouseInfo.h"
#include"../../../../Info/KeyInfo/KeyInfo.h"
void ResultDisplay::Init()
{

	if (!m_ResultBackTex)
	{
		m_ResultBackTex = std::make_shared<KdTexture>();
		m_ResultBackTex->Load("Asset/Textures/GameUI/Result/ResultBack.png");
	}

	if (!m_ArrowTex)
	{
		m_ArrowTex = std::make_shared<KdTexture>();
		m_ArrowTex->Load("Asset/Textures/GameUI/Result/Arrow.png");
		m_ArrowSiz = { (float)m_ArrowTex->GetWidth(),(float)m_ArrowTex->GetHeight() };
		m_ArrowLPos = { -400, 0 };
		m_ArrowRPos = { 400, 0 };
	}

	m_ResultDisplay_Score = std::make_shared<ResultDisplay_Score>();
	m_ResultDisplay_Score->Init();
	m_ResultDisplayList.push_back(m_ResultDisplay_Score);



	auto spResultDisplay_PotionInventory = std::make_shared<ResultDisplay_PotionInventory>();
	spResultDisplay_PotionInventory->Init();
	m_ResultDisplayList.push_back(spResultDisplay_PotionInventory);
}

void ResultDisplay::Update()
{

	ArrowUpdate();

	if (m_CurrentDisplayIndex < 0 || m_CurrentDisplayIndex >= m_ResultDisplayList.size()) { return; }
	m_ResultDisplayList[m_CurrentDisplayIndex]->Update();
}

void ResultDisplay::PreDraw()
{
	if (m_CurrentDisplayIndex < 0 || m_CurrentDisplayIndex >= m_ResultDisplayList.size()) { return; }
	m_ResultDisplayList[m_CurrentDisplayIndex]->PreDraw();
}

void ResultDisplay::DrawSprite()
{
	if (m_ResultBackTex)
	{
		Math::Color col = { 3,3,3,1 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_ResultBackTex, 0, 0, nullptr, &col);
	}

	ArrowDrawSprite();

	if (m_CurrentDisplayIndex < 0 || m_CurrentDisplayIndex >= m_ResultDisplayList.size()) { return; }
	m_ResultDisplayList[m_CurrentDisplayIndex]->DrawSprite();
}

void ResultDisplay::SetPath(std::string path)
{

}

void ResultDisplay::SetDisplayFloorInfo(std::vector<DisplayFloorInfo> _displayFloorInfo)
{
	m_ResultDisplay_Score->SetDisplayFloorInfo(_displayFloorInfo);
}

void ResultDisplay::ArrowUpdate()
{
	m_AroowAlpha += m_AddAroowAlpha * DeltaTime::Instance().GetRealDeltaTime();
	if (m_AroowAlpha > 1)
	{
		m_AroowAlpha = 1;
		m_AddAroowAlpha *= -1;
	}

	if (m_AroowAlpha < 0.3)
	{
		m_AroowAlpha = 0.3;
		m_AddAroowAlpha *= -1;
	}







	POINT mouse = MouseInfo::Instance().m_windowPos;
	{

		//左
		float Left = m_ArrowLPos.x - m_ArrowSiz.x;
		float Roght = m_ArrowLPos.x + m_ArrowSiz.x;
		float top = m_ArrowLPos.y + m_ArrowSiz.y;
		float bot = m_ArrowLPos.y - m_ArrowSiz.y;

		if (mouse.x >= Left && mouse.x <= Roght
			&& mouse.y <= top && mouse.y >= bot)
		{
			m_ArrowLHiFlg = true;
			if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
			{
				m_CurrentDisplayIndex--;

				if (0 >= m_CurrentDisplayIndex)
				{
					m_CurrentDisplayIndex = 0;
				}
			}
		}
		else
		{
			m_ArrowLHiFlg = false;
		}
	}
	{

		//migi
		float Left = m_ArrowRPos.x - m_ArrowSiz.x;
		float Roght = m_ArrowRPos.x + m_ArrowSiz.x;
		float top = m_ArrowRPos.y + m_ArrowSiz.y;
		float bot = m_ArrowRPos.y - m_ArrowSiz.y;

		if (mouse.x >= Left && mouse.x <= Roght
			&& mouse.y <= top && mouse.y >= bot)
		{
			m_ArrowRHiFlg = true;
			if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
			{
				m_CurrentDisplayIndex++;

				if (m_ResultDisplayList.size() <= m_CurrentDisplayIndex)
				{
					m_CurrentDisplayIndex = m_ResultDisplayList.size() - 1;
				}
			}
		}
		else
		{
			m_ArrowRHiFlg = false;
		}
	}

}

void ResultDisplay::ArrowDrawSprite()
{
	if (m_ArrowTex)
	{


		Math::Color col = { 1,1,1,m_AroowAlpha };

		Math::Vector2 Siz = m_ArrowSiz;
		if (m_ArrowLHiFlg)
		{
			Siz *= m_AroowHitScale;
		}

		if (m_CurrentDisplayIndex != 0)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_ArrowTex, m_ArrowLPos.x, m_ArrowLPos.y, Siz.x, Siz.y, nullptr, &col);
		}


		Siz = m_ArrowSiz;
		if (m_ArrowRHiFlg)
		{
			Siz *= m_AroowHitScale;
		}
		if (m_CurrentDisplayIndex != m_ResultDisplayList.size() - 1)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_ArrowTex, m_ArrowRPos.x, m_ArrowRPos.y, Siz.x, Siz.y, nullptr, &col, { 0.5,0.5 }, 180);
		}
	}

}
