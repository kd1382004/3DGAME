//---- 自分のヘッダー ----
#include "UIBase2.h"							//キー表示付きUIの基底クラス

//---- 便利機能 ----
#include"../../../Info/KeyInfo/KeyInfo.h"		//キー入力

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
	if (m_drawFlg)
	{
		if (m_spTex)
		{
			Math::Vector2 piv = { 0.5,0.5 };

			KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTex, m_2DPos.x, m_2DPos.y, nullptr, nullptr, piv);
			float size = m_spTex->GetHeight();
			Math::Vector2 pos = m_2DPos;

			switch (m_drawKeyPos)
			{
			case DrawKeyPos_Left:
				piv = { 1,0.5 };
				pos.x -= m_spTex->GetWidth()/2;
				break;

			case DrawKeyPos_Right:
				piv = { 0,0.5 };
				pos.x += m_spTex->GetWidth() / 2;
				break;

			case DrawKeyPos_Top:
				piv = { 0.5,0 };
				pos.y+= m_spTex->GetHeight()/2;
				break;

			case DrawKeyPos_Bot:
				piv = { 0.5,1 };
				pos.y -= m_spTex->GetHeight() / 2;
				break;

			default:
				break;
			}


			KeyInfo::Instance().Draw2Dkey(m_drawKey, pos, { size,size }, m_alpha, piv);
		}
	}

}
