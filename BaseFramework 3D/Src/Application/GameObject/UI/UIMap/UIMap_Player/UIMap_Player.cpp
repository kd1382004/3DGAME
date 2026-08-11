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

	m_angle++;

	Math::Matrix rMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angle));

	float m_siz =1.0f;
	//KdShaderManager::Instance().m_spriteShader.SetMatrix(rMat);
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_playerTex, m_basePos.x + m_player2DPos.x * m_siz, m_basePos.y + m_player2DPos.y * m_siz);
}

void UIMap_Player::Change2DPos()
{
	// ワールド → タイルインデックス（左上原点）
	float tileX = (m_player3DPos.x - m_base3DPos.x) / m_tileSiz;
	float tileY = (m_player3DPos.z - m_base3DPos.z) / m_tileSiz;

	Math::Vector2 m_mapTexSiz = { 5,5 };

	// ★ タイル中心を使う（ここがズレの原因だった）
	float px = tileX * m_mapTexSiz.x + m_mapTexSiz.x * 0.5f;
	float py = tileY * m_mapTexSiz.y + m_mapTexSiz.y * 0.5f;

	m_player2DPos = { px, py };

}
