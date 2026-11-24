//---- 自分のヘッダー ----
#include "NextFloorGaugeUI.h"					//次の階へ進むゲージ

//---- 便利機能 ----
#include"../../../Info/KeyInfo/KeyInfo.h"		//キー入力

void NextFloorGaugeUI::Init()
{
	if (!m_spGaugeTexture)
	{
		m_spGaugeTexture = std::make_shared<KdTexture>();
		m_spGaugeTexture->Load("Asset/Textures/GameUI/NextFloorCount/NextFloorCount.png");

		m_size.x = m_spGaugeTexture->GetInfo().Width;
		m_size.y = m_spGaugeTexture->GetInfo().Height / 2;

		m_gaugePercent = 0;

		m_2DPos = { 50, 50 };
		m_actionKeySize = { 32,32 };
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

		if (m_actionKeyAlpha < m_actionKeyAlphaMin)
		{
			m_actionKeyAlpha = m_actionKeyAlphaMin;
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
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spGaugeTexture, m_2DPos.x, m_2DPos.y, m_size.x, m_size.y, &srcRect, nullptr, pivot);

	srcRect = { 0,
		(long)m_spGaugeTexture->GetInfo().Height / 2,
		(long)m_spGaugeTexture->GetInfo().Width,
		(long)m_spGaugeTexture->GetInfo().Height / 2 };

	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spGaugeTexture, m_2DPos.x, m_2DPos.y, m_size.x * m_gaugePercent, m_size.y, &srcRect, nullptr, pivot);

	KeyInfo::Instance().Draw2Dkey(m_actionKey, { m_2DPos.x - m_actionKeySize.x / 2, m_2DPos.y }, m_actionKeySize, m_actionKeyAlpha, { 0.5f,0.5f });
}
