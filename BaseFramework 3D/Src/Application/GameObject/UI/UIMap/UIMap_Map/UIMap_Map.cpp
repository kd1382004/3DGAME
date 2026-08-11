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



	DiscoverTile();

	for (auto info : m_posList)
	{
		if (!info.m_drawFlg) { continue; }

		// pos はミニマップ
		float drawX = m_basePos.x + info.m_pos.x * m_siz;
		float drawY = m_basePos.y + info.m_pos.y * m_siz;

		KdShaderManager::Instance().m_spriteShader.DrawTex(m_mapTex, drawX, drawY, m_mapTexSiz.x, m_mapTexSiz.y);
	}
}

void UIMap_Map::AddPosList(Math::Vector3 _3DPos, float _worldTileSize)
{
	float tileX = _3DPos.x / _worldTileSize;
	float tileY = _3DPos.z / _worldTileSize;

	float px = tileX * m_mapTexSiz.x + m_mapTexSiz.x * 0.5f;
	float py = tileY * m_mapTexSiz.y + m_mapTexSiz.y * 0.5f;

	MineMapInfo info;
	info.m_pos = { px, py };
	info.m_tileIndex = { tileX, tileY };
	info.m_drawFlg = false;

	m_posList.push_back(info);
}

void UIMap_Map::PosListReset()
{
	m_posList.clear();
}

void UIMap_Map::CreateNavMap(std::vector<std::vector<int>> mapDate)
{
	//m_posList.clear();
	//for (int y = 0;y < mapDate.size();y++)
	//{
	//	for (int x = 0;x < mapDate[y].size();x++)
	//	{
	//		int tile = mapDate[y][x];

	//		if (tile == (int)MapGenerate::TileType::None)
	//		{
	//			Math::Vector2 pos;
	//			pos.x = m_basePos.x + m_mapTexSiz.x * x;
	//			pos.y = m_basePos.y + m_mapTexSiz.y * y;
	//			m_posList.push_back({ pos,false });
	//		}
	//	}
	//}

}

void UIMap_Map::DiscoverTile()
{
	float discoverRadius = m_discoverTileNum * m_mapTexSiz.x;

	for (auto& info : m_posList) 
	{
		if (info.m_drawFlg) continue;


		Math::Vector2 pos = { m_basePos.x + info.m_pos.x * m_siz, m_basePos.y + info.m_pos.y * m_siz };
		float dist = (pos - m_minMapPlayerPos).Length();

		if (dist < discoverRadius)
		{
			info.m_drawFlg = true; 
		}
	}
}
