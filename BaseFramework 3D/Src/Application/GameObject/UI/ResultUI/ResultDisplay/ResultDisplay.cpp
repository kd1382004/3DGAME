#include "ResultDisplay.h"
#include"../../../../Info/NumDraw/NumDraw.h"
#include"../../../../Info/MouseInfo/MouseInfo.h"
#include"../../../../Info/KeyInfo/KeyInfo.h"
void ResultDisplay::Init()
{
	m_displayFloorInfoNum = 0;
	if (!m_DisplayFloorTex)
	{
		m_DisplayFloorTex = std::make_shared<KdTexture>();
		m_DisplayFloorTex->Load("Asset/Textures/GameUI/Result/floorReached.png");
	}
	if (!m_enemyKillTex)
	{
		m_enemyKillTex = std::make_shared<KdTexture>();
		m_enemyKillTex->Load("Asset/Textures/GameUI/Result/EnemyKill.png");
	}

	if (!m_OverallResultTex)
	{
		m_OverallResultTex = std::make_shared<KdTexture>();
		m_OverallResultTex->Load("Asset/Textures/GameUI/Result/OverallResult.png");
	}

	if (!m_HierarchyTex)
	{
		m_HierarchyTex = std::make_shared<KdTexture>();
		m_HierarchyTex->Load("Asset/Textures/GameUI/Result/HierarchyTex.png");
	}

	m_OverallResultPos = { 0,250 };


	m_base2DPos = { -300,m_OverallResultPos.y - 200 };
	m_baseNumPos = m_base2DPos;
	m_baseNumPos.x += 600;


	if (!m_ArrowTex)
	{
		m_ArrowTex = std::make_shared<KdTexture>();
		m_ArrowTex->Load("Asset/Textures/GameUI/Result/Arrow.png");


		m_ArrowLPos = m_OverallResultPos;
		m_ArrowRPos = m_OverallResultPos;

		m_ArrowSiz = { (float)m_ArrowTex->GetWidth(),(float)m_ArrowTex->GetHeight() };

		m_ArrowLPos.y -= m_ArrowSiz.y / 2 + m_OverallResultTex->GetHeight() / 2;
		m_ArrowLPos.x -= m_ArrowSiz.x;

		m_ArrowRPos.y -= m_ArrowSiz.y / 2 + m_OverallResultTex->GetHeight() / 2;
		m_ArrowRPos.x += m_ArrowSiz.x;
	}
}

void ResultDisplay::Update()
{
	ArrowUpdate();
}

void ResultDisplay::PreDraw()
{}

void ResultDisplay::DrawSprite()
{

	int floorNum = m_displayFloorInfo[m_displayFloorInfoNum].DisplayFloor;
	int enemykill = m_displayFloorInfo[m_displayFloorInfoNum].enemyKillCount;
	float timeSc = m_displayFloorInfo[m_displayFloorInfoNum].playTimeSc;


	if (m_displayFloorInfoNum == 0 && m_OverallResultTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_OverallResultTex, m_OverallResultPos.x, m_OverallResultPos.y);
	}
	else
	{
		Math::Vector2 pivo = { 0,0.5 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_HierarchyTex, m_OverallResultPos.x, m_OverallResultPos.y, nullptr, nullptr, pivo);
		NumDraw::GetInstance().Drow(floorNum, Aligned::RAligned, m_OverallResultPos, kWhiteColor, 4,true);
	}



	if (m_ArrowTex)
	{

		Math::Vector2 Siz = m_ArrowSiz;
		if (m_ArrowLHiFlg)
		{
			Siz *= m_AroowHitScale;
		}

		if (m_displayFloorInfoNum != 0)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_ArrowTex, m_ArrowLPos.x, m_ArrowLPos.y, Siz.x, Siz.y);
		}


		Siz = m_ArrowSiz;
		if (m_ArrowRHiFlg)
		{
			Siz *= m_AroowHitScale;
		}
		if (m_displayFloorInfoNum != m_displayFloorInfo.size() - 1)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_ArrowTex, m_ArrowRPos.x, m_ArrowRPos.y, Siz.x, Siz.y, nullptr, &kWhiteColor, { 0.5,0.5 }, 180);
		}
	}


	if (m_DisplayFloorTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_DisplayFloorTex, m_base2DPos.x, m_base2DPos.y);
		NumDraw::GetInstance().Drow(floorNum, Aligned::RAligned, m_baseNumPos, kWhiteColor, 4, true);
	}

	if (m_enemyKillTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_enemyKillTex, m_base2DPos.x, m_base2DPos.y - 100);
		NumDraw::GetInstance().Drow(enemykill, Aligned::RAligned, { m_baseNumPos.x,m_baseNumPos.y - 100 }, kWhiteColor, 4, true);
	}


}

void ResultDisplay::SetPath(std::string path)
{

}

void ResultDisplay::ArrowUpdate()
{

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
				m_displayFloorInfoNum--;

				if (0 >= m_displayFloorInfoNum)
				{
					m_displayFloorInfoNum = 0;
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
				m_displayFloorInfoNum++;

				if (m_displayFloorInfo.size() <= m_displayFloorInfoNum)
				{
					m_displayFloorInfoNum = m_displayFloorInfo.size() - 1;
				}
			}
		}
		else
		{
			m_ArrowRHiFlg = false;
		}
	}

}
