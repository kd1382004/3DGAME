#include "ResultDisplay.h"

#include"ResultDisplayBase.h"
#include"ResultDisplay_Score/ResultDisplay_Score.h"


void ResultDisplay::Init()
{

	if (!m_ResultBackTex)
	{
		m_ResultBackTex = std::make_shared<KdTexture>();
		m_ResultBackTex->Load("Asset/Textures/GameUI/Result/ResultBack.png");
	}



	m_ResultDisplay_Score = std::make_shared<ResultDisplay_Score>();
	m_ResultDisplay_Score->Init();

	m_ResultDisplayBase = m_ResultDisplay_Score;
}

void ResultDisplay::Update()
{
	if (!m_ResultDisplayBase) { return; }
	m_ResultDisplayBase->Update();
}

void ResultDisplay::PreDraw()
{
	if (!m_ResultDisplayBase) { return; }
	m_ResultDisplayBase->PreDraw();
}

void ResultDisplay::DrawSprite()
{
	if (m_ResultBackTex)
	{
		Math::Color col = { 3,3,3,1 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_ResultBackTex, 0, 0, nullptr, &col);
	}

	if (!m_ResultDisplayBase) { return; }
	m_ResultDisplayBase->DrawSprite();
}

void ResultDisplay::SetPath(std::string path)
{

}

void ResultDisplay::SetDisplayFloorInfo(std::vector<DisplayFloorInfo> _displayFloorInfo)
{
	if (!m_ResultDisplay_Score) { return; }
	m_ResultDisplay_Score->SetDisplayFloorInfo(_displayFloorInfo);
}
