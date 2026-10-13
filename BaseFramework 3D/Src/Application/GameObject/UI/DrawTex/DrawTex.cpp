#include"../../../Info/KeyInfo/KeyInfo.h"
#include "DrawTex.h"

void DrawTex::Update()
{

}

void DrawTex::DrawSprite()
{
	if (m_tex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_tex, m_2DPos.x, m_2DPos.y);
	}
}

void DrawTex::LoadTex(std::string path)
{
	if (!m_tex) { m_tex = std::make_shared<KdTexture>(); }
	m_tex->Load(path);
}
