//---- 自分のヘッダー ----
#include "UIMap_Player.h"		//ミニマップのプレイヤーアイコン

//=============================================
// 基本の処理
//=============================================

void UIMap_Player::Init()
{
	if (!m_spPlayerTex)
	{
		m_spPlayerTex = std::make_shared<KdTexture>();
		m_spPlayerTex->Load("Asset/Textures/GameUI/Map/Player.png");

		m_angle = 0;
	}
}

void UIMap_Player::PreDraw()
{
	Change2DPos();
}

void UIMap_Player::DrawSprite()
{
	if (!m_spPlayerTex) { return; }

	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spPlayerTex, m_basePos.x + m_player2DPos.x * m_size, m_basePos.y + m_player2DPos.y * m_size,nullptr,nullptr,{0.5,0.5}, m_angle);
}

//=============================================
// 取得
//=============================================

Math::Vector2 UIMap_Player::GetPos()
{
	return { m_basePos.x + m_player2DPos.x * m_size, m_basePos.y + m_player2DPos.y * m_size };
}

//=============================================
// その他の処理
//=============================================

void UIMap_Player::Change2DPos()
{
	float tileX = (m_player3DPos.x - m_base3DPos.x) / m_tileSize;
	float tileY = (m_player3DPos.z - m_base3DPos.z) / m_tileSize;


	//タイル中心
	float px = tileX * m_mapTexSize.x + m_mapTexSize.x * 0.5f;
	float py = tileY * m_mapTexSize.y + m_mapTexSize.y * 0.5f;

	m_player2DPos = { px, py };
}
