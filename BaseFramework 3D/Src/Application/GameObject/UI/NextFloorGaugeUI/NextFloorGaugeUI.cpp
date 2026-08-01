#include "NextFloorGaugeUI.h"
#include"../../../Info/KeyInfo/KeyInfo.h"

void NextFloorGaugeUI::Init()
{
	if (!m_spGaugeTexture)
	{
		m_spGaugeTexture = std::make_shared<KdTexture>();
		m_spGaugeTexture->Load("Asset/Textures/GameUI/NextFloorCount/NextFloorCount.png");

		m_siz.x = m_spGaugeTexture->GetInfo().Width;
		m_siz.y = m_spGaugeTexture->GetInfo().Height / 2;

		m_gaugePercent = 0;

		m_2DPos = { 100, 100 };
		m_actionKeySiz = { 32,32 };
	}
}

void NextFloorGaugeUI::Update()
{


	if (KeyInfo::Instance().GetValidKeyPush(m_actionKey))
	{
		m_actionKeyAlpha = 1.0f;

	}
	else
	{
		m_actionKeyAlpha += m_actionKeyPushAlpha;

		if (m_actionKeyAlpha > 1.0f)
		{
			m_actionKeyAlpha = 1.0f;
			m_actionKeyPushAlpha *= -1;
		}

		if (m_actionKeyAlpha < m_actionKeyAlphaMine)
		{
			m_actionKeyAlpha = m_actionKeyAlphaMine;
			m_actionKeyPushAlpha *= -1;
		}
	}

}

void NextFloorGaugeUI::DrawSprite()
{
	if (!m_spGaugeTexture) { return; }
	if (!m_drawFlg) { return; }

	Math::Vector2 pivot = { 0.0f, 0.5f };


	Math::Rectangle srcRect = { 0,  0, (long)m_spGaugeTexture->GetInfo().Width, (long)m_spGaugeTexture->GetInfo().Height / 2 };
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spGaugeTexture, m_2DPos.x, m_2DPos.y, m_siz.x, m_siz.y, &srcRect, nullptr, pivot);

	srcRect = { 0,
		(long)m_spGaugeTexture->GetInfo().Height / 2,
		(long)m_spGaugeTexture->GetInfo().Width,
		(long)m_spGaugeTexture->GetInfo().Height / 2 };

	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spGaugeTexture, m_2DPos.x, m_2DPos.y, m_siz.x * m_gaugePercent, m_siz.y, &srcRect, nullptr, pivot);

	KeyInfo::Instance().Draw2Dkey(m_actionKey, { m_2DPos.x - m_actionKeySiz.x / 2, m_2DPos.y }, m_actionKeySiz, m_actionKeyAlpha, { 0.5f,0.5f });
}
