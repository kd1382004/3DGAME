#include "ResultDisplay.h"
#include"../../../../Info/NumDraw/NumDraw.h"
void ResultDisplay::Init()
{
}

void ResultDisplay::Update()
{
}

void ResultDisplay::PreDraw()
{
}

void ResultDisplay::DrawSprite()
{
	if (!m_tex) { return; }
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_tex, m_2DPos.x, m_2DPos.y);


	if (!m_numDrawFlg) { return; }

	Math::Vector2 pos = m_2DPos;
	pos.x += 300;
	NumDraw::GetInstance().Drow(m_setNum,LAligned, pos,kWhiteColor,4);
}

void ResultDisplay::SetPath(std::string path)
{
	if (!m_tex)
	{
		m_tex = std::make_shared<KdTexture>();
		m_tex->Load(path);
	}

}
