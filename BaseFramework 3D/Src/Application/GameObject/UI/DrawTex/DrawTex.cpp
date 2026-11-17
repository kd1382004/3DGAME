#include"../../../Info/KeyInfo/KeyInfo.h"
#include "DrawTex.h"

void DrawTex::Update()
{

}

void DrawTex::DrawSprite()
{
	if (m_spTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTex, m_2DPos.x, m_2DPos.y);
	}
}

void DrawTex::LoadTex(std::string _path)
{
	if (!m_spTex) { m_spTex = std::make_shared<KdTexture>(); }
	m_spTex->Load(_path);
}
