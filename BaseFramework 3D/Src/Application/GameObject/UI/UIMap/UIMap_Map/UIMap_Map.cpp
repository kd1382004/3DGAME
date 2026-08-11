#include "UIMap_Map.h"
#include"../../../Terrains/Map/MapGenerate/MapGenerate.h"

void UIMap_Map::Init()
{
	if (!m_mapTex)
	{
		m_mapTex = std::make_shared<KdTexture>();
		m_mapTex->Load("Asset/Textures/GameUI/Map/Map.png");

		m_mapTexSiz.x = m_mapTex->GetWidth();
		m_mapTexSiz.y = m_mapTex->GetHeight();

		m_basePos = {};
	}
}

void UIMap_Map::DrawSprit()
{

	if (!m_mapTex) { return; }

	float m_siz = 1.0f;

	for (auto pos : m_posList)
	{
		// pos はミニマップ左上基準ピクセル座標
		float drawX = m_basePos.x + pos.x * m_siz;
		float drawY = m_basePos.y + pos.y * m_siz;

		KdShaderManager::Instance().m_spriteShader.DrawTex(m_mapTex, drawX, drawY, m_mapTexSiz.x, m_mapTexSiz.y);
	}
}

void UIMap_Map::AddPosList(Math::Vector3 _3DPos, float _worldTileSize)
{
	// ワールド → タイルインデックス
	float tileX = _3DPos.x / _worldTileSize;
	float tileY = _3DPos.z / _worldTileSize;

	// タイル中心をミニマップ左上基準ピクセル座標に変換
	float px = tileX * m_mapTexSiz.x + (m_mapTexSiz.x );
	float py = tileY * m_mapTexSiz.y + (m_mapTexSiz.y);

	m_posList.push_back({ px, py });
}

void UIMap_Map::PosListReset()
{
	m_posList.clear();
}

void UIMap_Map::CreateNavMap(std::vector<std::vector<int>> mapDate)
{
	m_posList.clear();
	for (int y = 0;y < mapDate.size();y++)
	{
		for (int x = 0;x < mapDate[y].size();x++)
		{
			int tile = mapDate[y][x];

			if (tile == (int)MapGenerate::TileType::None)
			{
				Math::Vector2 pos;
				pos.x = m_basePos.x + m_mapTexSiz.x * x;
				pos.y = m_basePos.y + m_mapTexSiz.y * y;
				m_posList.push_back(pos);
			}
		}
	}

}
