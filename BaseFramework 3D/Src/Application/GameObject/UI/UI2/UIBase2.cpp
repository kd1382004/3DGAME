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
			Math::Vector2 piv = { 0.5,0.5 };

			KdShaderManager::Instance().m_spriteShader.DrawTex(m_tex, m_2DPos.x, m_2DPos.y, nullptr, nullptr, piv);
			float siz = m_tex->GetHeight();
			Math::Vector2 pos = m_2DPos;

			switch (m_DrawKeyPos)
			{
			case DrawKeyPos_Left:
				piv = { 1,0.5 };
				pos.x -= m_tex->GetWidth()/2;
				break;

			case DrawKeyPos_Right:
				piv = { 0,0.5 };
				pos.x += m_tex->GetWidth() / 2;
				break;

			case DrawKeyPos_Top:
				piv = { 0.5,0 };
				pos.y+= m_tex->GetHeight()/2;
				break;

			case DrawKeyPos_Bot:
				piv = { 0.5,1 };
				pos.y -= m_tex->GetHeight() / 2;
				break;

			default:
				break;
			}


			KeyInfo::Instance().Draw2Dkey(m_DrawKey, pos, { siz,siz }, m_alpha, piv);
		}
	}

}
