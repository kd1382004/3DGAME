#include "ResultDisplay.h"
#include"../../../../Info/NumDraw/NumDraw.h"
#include"../../../../Info/MouseInfo/MouseInfo.h"
#include"../../../../Info/KeyInfo/KeyInfo.h"
void ResultDisplay::Init()
{
	m_displayFloorInfoNum = 0;
	if (!m_ResultBackTex)
	{
		m_ResultBackTex = std::make_shared<KdTexture>();
		m_ResultBackTex->Load("Asset/Textures/GameUI/Result/ResultBack.png");
	}
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
	if (!m_playTimeScTex)
	{
		m_playTimeScTex = std::make_shared<KdTexture>();
		m_playTimeScTex->Load("Asset/Textures/GameUI/Result/Time.png");
	}
	if (!m_TimeColonTex)
	{
		m_TimeColonTex = std::make_shared<KdTexture>();
		m_TimeColonTex->Load("Asset/Textures/GameUI/Result/TimeColon.png");
	}
	if (!m_TreasureChestsNumTex)
	{
		m_TreasureChestsNumTex = std::make_shared<KdTexture>();
		m_TreasureChestsNumTex->Load("Asset/Textures/GameUI/Result/TreasureChestsNum.png");
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
	if (!m_backTex)
	{
		m_backTex = std::make_shared<KdTexture>();
		m_backTex->Load("Asset/Textures/GameUI/Result/back.png");
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
{
}

void ResultDisplay::DrawSprite()
{

	int floorNum = m_displayFloorInfo[m_displayFloorInfoNum].DisplayFloor;
	int enemykill = m_displayFloorInfo[m_displayFloorInfoNum].enemyKillCount;
	int TreasureChestsNum = m_displayFloorInfo[m_displayFloorInfoNum].TreasureChestsNum;
	int timeSc = m_displayFloorInfo[m_displayFloorInfoNum].playTimeSc;

	if (m_ResultBackTex)
	{
		Math::Color col = { 3,3,3,1 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_ResultBackTex, 0, 0, nullptr, &col);
	}


	if (m_displayFloorInfoNum == 0 && m_OverallResultTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_OverallResultTex, m_OverallResultPos.x, m_OverallResultPos.y);
	}
	else
	{
		Math::Vector2 pivo = { 0,0.5 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_HierarchyTex, m_OverallResultPos.x, m_OverallResultPos.y, nullptr, nullptr, pivo);
		NumDraw::GetInstance().Drow(floorNum, Aligned::RAligned, m_OverallResultPos, kWhiteColor, 5, true);
	}



	if (m_ArrowTex)
	{


		Math::Color col = { 1,1,1,m_AroowAlpha };

		Math::Vector2 Siz = m_ArrowSiz;
		if (m_ArrowLHiFlg)
		{
			Siz *= m_AroowHitScale;
		}

		if (m_displayFloorInfoNum != 0)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_ArrowTex, m_ArrowLPos.x, m_ArrowLPos.y, Siz.x, Siz.y, nullptr, &col);
		}


		Siz = m_ArrowSiz;
		if (m_ArrowRHiFlg)
		{
			Siz *= m_AroowHitScale;
		}
		if (m_displayFloorInfoNum != m_displayFloorInfo.size() - 1)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_ArrowTex, m_ArrowRPos.x, m_ArrowRPos.y, Siz.x, Siz.y, nullptr, &col, { 0.5,0.5 }, 180);
		}
	}


	int Num = 0;
	Math::Vector2 pivo = { 0,0.5 };

	if (m_DisplayFloorTex && m_displayFloorInfoNum == 0)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_DisplayFloorTex, m_base2DPos.x, m_base2DPos.y, nullptr, nullptr, pivo);
		NumDraw::GetInstance().Drow(floorNum, Aligned::RAligned, m_baseNumPos, kWhiteColor, 4, true);
		Num++;
	}

	if (m_enemyKillTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_enemyKillTex, m_base2DPos.x, m_base2DPos.y - Num * 100, nullptr, nullptr, pivo);
		NumDraw::GetInstance().Drow(enemykill, Aligned::RAligned, { m_baseNumPos.x,m_baseNumPos.y - Num * 100 }, kWhiteColor, 4, true);
		Num++;
	}

	if (m_TreasureChestsNumTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_TreasureChestsNumTex, m_base2DPos.x, m_base2DPos.y - Num * 100, nullptr, nullptr, pivo);
		NumDraw::GetInstance().Drow(TreasureChestsNum, Aligned::RAligned, { m_baseNumPos.x,m_baseNumPos.y - Num * 100 }, kWhiteColor, 4, true);
		Num++;
	}

	if (m_playTimeScTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_playTimeScTex, m_base2DPos.x, m_base2DPos.y - Num * 100, nullptr, nullptr, pivo);


		int sc = timeSc % 60;       // 秒
		int mine = (timeSc / 60) % 60; // 分
		int hour = timeSc / 3600;      // 時


		NumDraw::GetInstance().Drow(sc, Aligned::RAligned, { m_baseNumPos.x,m_baseNumPos.y - Num * 100 }, kWhiteColor, 4, true, 2);

		if (m_TimeColonTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_TimeColonTex, m_baseNumPos.x - 78, m_base2DPos.y - Num * 100);
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_TimeColonTex, m_baseNumPos.x - 158, m_base2DPos.y - Num * 100);
		}


		NumDraw::GetInstance().Drow(mine, Aligned::RAligned, { m_baseNumPos.x - 82,m_baseNumPos.y - Num * 100 }, kWhiteColor, 4, true, 2);
		NumDraw::GetInstance().Drow(hour, Aligned::RAligned, { m_baseNumPos.x - 162,m_baseNumPos.y - Num * 100 }, kWhiteColor, 4, true, 2);
		Num++;
	}

	for (int i = 0;i < Num;i++)
	{
		float sizX = m_baseNumPos.x - m_base2DPos.x;
		float sizY = 100;
		Math::Color color = {};
		if (i % 2 == 0)
		{
			color = { 1,1,1,0.3f };
		}
		else
		{
			color = { 0.5,0.5,0.5,0.3f };
		}

		KdShaderManager::Instance().m_spriteShader.DrawTex(m_backTex, m_base2DPos.x, m_base2DPos.y - i * 100, sizX, sizY, nullptr, &color, pivo);
	}

}

void ResultDisplay::SetPath(std::string path)
{

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
