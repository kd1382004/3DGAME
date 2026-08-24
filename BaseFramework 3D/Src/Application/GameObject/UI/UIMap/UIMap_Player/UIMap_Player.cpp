#include "UIMap_Player.h"

void UIMap_Player::Init()
{
	if (!m_playerTex)
	{
		m_playerTex = std::make_shared<KdTexture>();
		m_playerTex->Load("Asset/Textures/GameUI/Map/Player.png");

		m_angle = 0;
	}
}

void UIMap_Player::DrawSprit()
{
	if (!m_playerTex) { return; }

	Change2DPos();


	KdShaderManager::Instance().m_spriteShader.DrawTex(m_playerTex, m_basePos.x + m_player2DPos.x * m_siz, m_basePos.y + m_player2DPos.y * m_siz,nullptr,nullptr,{0.5,0.5}, m_angle);
}

Math::Vector2 UIMap_Player::GetPos()
{
	return { m_basePos.x + m_player2DPos.x * m_siz, m_basePos.y + m_player2DPos.y * m_siz };
}

void UIMap_Player::Change2DPos()
{
	float tileX = (m_player3DPos.x - m_base3DPos.x) / m_tileSiz;
	float tileY = (m_player3DPos.z - m_base3DPos.z) / m_tileSiz;


	//タイル中心
	float px = tileX * m_mapTexSiz.x + m_mapTexSiz.x * 0.5f;
	float py = tileY * m_mapTexSiz.y + m_mapTexSiz.y * 0.5f;

	m_player2DPos = { px, py };
}
