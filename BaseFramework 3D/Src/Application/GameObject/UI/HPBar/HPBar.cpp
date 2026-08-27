#include "HPBar.h"

#include"../../../Info/NumDraw/NumDraw.h"

void HPBar::Init()
{

	if (!m_HPBarBackTex)
	{
		m_HPBarBackTex = std::make_shared<KdTexture>();
		m_HPBarBackTex->Load("Asset/Textures/GameUI/HPBar/HPBarBackTex.png");

		m_HPBarBackTexLength = { (float)m_HPBarBackTex->GetWidth(),	(float)m_HPBarBackTex->GetHeight() };
		m_HPBarBackTexRect = { 0,0,(long)m_HPBarBackTexLength.x,(long)m_HPBarBackTexLength.y };
	}

	if (!m_HPBarTex)
	{
		m_HPBarTex = std::make_shared<KdTexture>();
		m_HPBarTex->Load("Asset/Textures/GameUI/HPBar/HPBarTex.png");

		m_HPBarTexLength = { (float)m_HPBarTex->GetWidth(),	(float)m_HPBarTex->GetHeight() };
		m_HPBarTexRect = { 0,0,(long)m_HPBarTexLength.x,(long)m_HPBarTexLength.y };
	}

	if (!m_HPBarIconFrameTex)
	{
		m_HPBarIconFrameTex = std::make_shared<KdTexture>();
		m_HPBarIconFrameTex->Load("Asset/Textures/GameUI/HPBar/HPBarIconFrameTex.png");

		m_HPBarIconFrameTexLength = { (float)m_HPBarIconFrameTex->GetWidth(),	(float)m_HPBarIconFrameTex->GetHeight() };
		m_HPBarIconFrameTexRect = { 0,0,(long)m_HPBarIconFrameTexLength.x,(long)m_HPBarIconFrameTexLength.y };
	}

	m_siz = 1;
	m_HPBarTexPercent = std::clamp(m_HPBarTexPercent, 0.0f, 1.0f);
}

void HPBar::Update()
{

}

void HPBar::PreDraw()
{

	if (!m_drawFlg) { return; }

	HPBarDelayPreDraw();

	m_HPBarTexColor = kGreenColor;
	m_HPBarTexRect = { 0, 0, static_cast<long>(m_HPBarTexLength.x * m_HPBarTexPercent), static_cast<long>(m_HPBarTexLength.y) };

	//遅延バーの座標補正
	float HPBarDelayOffset = m_HPBarTexLength.x * m_siz * m_HPBarTexPercent;
	m_HPBarDelayTex2DPos = { m_HPBarTex2DPos.x + HPBarDelayOffset, m_HPBarTex2DPos.y };
}


void HPBar::DrawSprite()
{
	if (!m_drawFlg) { return; }

	Math::Vector2 pivot = { 0.0f, 0.5f };

	//HPバー背景
	if (m_HPBarBackTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_HPBarBackTex, m_HPBarBackTex2DPos.x, m_HPBarBackTex2DPos.y, m_HPBarBackTexLength.x * m_siz, m_HPBarBackTexLength.y * m_siz, &m_HPBarBackTexRect, &kWhiteColor, pivot);
	}

	//HPバー本体
	if (m_HPBarTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_HPBarTex, m_HPBarTex2DPos.x, m_HPBarTex2DPos.y, m_HPBarTexLength.x * m_siz * m_HPBarTexPercent, m_HPBarTexLength.y * m_siz, &m_HPBarTexRect, &m_HPBarTexColor, pivot);
	}


	//HPバー遅延分
	if (m_HPBarTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_HPBarTex, m_HPBarDelayTex2DPos.x, m_HPBarDelayTex2DPos.y, m_HPBarTexLength.x * m_siz * m_HPBarDelayTexPercent * m_HPBarTexProgres, m_HPBarTexLength.y * m_siz, &m_HPBarDelayTexRect, &m_HPBarDelayTexColor, pivot);
	}


	//アイコン用フレーム
	if (m_HPBarIconFrameTex)
	{
		pivot = { 0.5f, 0.5f };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_HPBarIconFrameTex, m_HPBarIconFrameTex2DPos.x, m_HPBarIconFrameTex2DPos.y, m_HPBarIconFrameTexLength.x * m_siz, m_HPBarIconFrameTexLength.y * m_siz, &m_HPBarIconFrameTexRect, &kWhiteColor, pivot);

		Math::Vector2 pos = m_HPBarIconFrameTex2DPos;

		if (m_LV > 9)
		{
			pos.x += m_HPBarIconFrameTexLength.x / 2;
		}

		NumDraw::GetInstance().Drow(m_LV, RAligned, pos, kWhiteColor, m_siz);
	}
}

void HPBar::SetHPBarTexPercent(float _percent)
{
	float percent = std::clamp(_percent, 0.0f, 1.0f);

	m_HPBarDelayTexPercent = m_HPBarTexPercent - percent;

	m_HPBarTexPercent = percent;

	//遅延分を計算
	float HPBarDelayOffset = m_HPBarTexLength.x * m_siz * m_HPBarTexPercent;

	// 遅延バーの描画位置
	m_HPBarDelayTex2DPos.x = m_HPBarTex2DPos.x + HPBarDelayOffset;

	// rectを計算
	float totalSrcWidth = m_HPBarTexLength.x;
	long left = static_cast<long>(totalSrcWidth * percent);
	long w = static_cast<long>(totalSrcWidth * m_HPBarDelayTexPercent);
	long top = 0;
	long height = static_cast<long>(m_HPBarTexLength.y);
	m_HPBarDelayTexRect = { left, top, w, height };
	m_HPBarTexProgres = 1.0f;

}

void HPBar::Delete()
{
	m_isExpired = true;
}

void HPBar::SetLV(int _LV)
{
	m_LV = _LV;
}

void HPBar::HPBarDelayPreDraw()
{
	if (m_HPBarTexProgres <= 0) { return; }

	m_HPBarTexProgres -= DeltaTime::Instance().GetGameDeltaTime() / m_HPBarDelayTexPercentDownSped;
	float progress = std::clamp(m_HPBarTexProgres, 0.0f, 1.0f);

	if (m_HPBarTexProgres <= 0)
	{
		m_HPBarTexProgres = 0;
	}
}
