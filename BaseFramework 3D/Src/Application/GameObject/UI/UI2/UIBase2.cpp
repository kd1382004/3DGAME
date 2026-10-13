#include "UIBase2.h"
#include"../../../Info/KeyInfo/KeyInfo.h"

void UIBase2::Update()
{
	m_alpha += m_addAlpha * DeltaTime::Instance().GetGameDeltaTime();;
	if (m_alpha > 1)
	{
		m_alpha = 1;
		m_addAlpha *= -1;
	}

	if (m_alpha < 0.3)
	{
		m_alpha = 0.3;
		m_addAlpha *= -1;
	}
}

void UIBase2::DrawSprite()
{
	if (m_drwaFlg)
	{
		if (m_tex)
		{
			Math::Vector2 piv = { 0,0.5 };

			KdShaderManager::Instance().m_spriteShader.DrawTex(m_tex, m_2DPos.x, m_2DPos.y, nullptr, nullptr, piv);

			piv = { 1,0.5 };

			float siz = m_tex->GetHeight();
			KeyInfo::Instance().Draw2Dkey(m_DrawKey, m_2DPos, { siz,siz }, m_alpha, piv);
		}
	}

}
