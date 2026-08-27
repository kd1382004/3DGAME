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


	if (!m_mapStairsTex)
	{
		m_mapStairsTex = std::make_shared<KdTexture>();
		m_mapStairsTex->Load("Asset/Textures/GameUI/Map/Stairs.png");

	}
}

void UIMap_Map::DrawSprit()
{

	if (!m_mapTex) { return; }




	for (const auto& info : m_posList)
	{
		if (!info.m_drawFlg) { continue; }

		// pos はミニマップ
		float drawX = m_basePos.x + info.m_pos.x * m_siz;
		float drawY = m_basePos.y + info.m_pos.y * m_siz;

		KdShaderManager::Instance().m_spriteShader.DrawTex(m_mapTex, drawX, drawY, m_mapTexSiz.x, m_mapTexSiz.y);
	}


	if (!m_mapStairsTex) { return; }
	if (!m_StairsMineMapInfo.m_drawFlg) { return; }
	if (!m_isStairsMine) { return; }

	float drawX = m_basePos.x + m_StairsMineMapInfo.m_pos.x * m_siz;
	float drawY = m_basePos.y + m_StairsMineMapInfo.m_pos.y * m_siz;

	KdShaderManager::Instance().m_spriteShader.DrawTex(m_mapStairsTex, drawX, drawY, m_mapTexSiz.x, m_mapTexSiz.y);
}

void UIMap_Map::PreDraw()
{
	DiscoverTile();
}

void UIMap_Map::AddPosList(Math::Vector3 _3DPos, float _worldTileSize, int LoomNum)
{
	float tileX = _3DPos.x / _worldTileSize;
	float tileY = _3DPos.z / _worldTileSize;

	float px = tileX * m_mapTexSiz.x + m_mapTexSiz.x * 0.5f;
	float py = tileY * m_mapTexSiz.y + m_mapTexSiz.y * 0.5f;

	MineMapInfo info;
	info.m_pos = { px, py };
	info.m_tileIndex = { tileX, tileY };
	info.m_drawFlg = false;
	info.m_LoomNum = LoomNum;

	m_posList.push_back(info);
}

void UIMap_Map::AddStairsPos(Math::Vector3 _3DPos, float _worldTileSize, int LoomNum)
{
	float tileX = _3DPos.x / _worldTileSize;
	float tileY = _3DPos.z / _worldTileSize;

	float px = tileX * m_mapTexSiz.x + m_mapTexSiz.x * 0.5f;
	float py = tileY * m_mapTexSiz.y + m_mapTexSiz.y * 0.5f;

	MineMapInfo info;
	info.m_pos = { px, py };
	info.m_tileIndex = { tileX, tileY };
	info.m_drawFlg = false;
	info.m_LoomNum = LoomNum;

	m_StairsMineMapInfo = info;
}

void UIMap_Map::PosListReset()
{
	m_posList.clear();
	m_StairsMineMapInfo.m_drawFlg = false;
	m_isStairsMine = false;
}


void UIMap_Map::DiscoverTile()
{
	float discoverRadius = m_discoverTileNum * m_mapTexSiz.x;

	for (auto& info : m_posList)
	{
		if (info.m_drawFlg) { continue; }


		Math::Vector2 pos = { m_basePos.x + info.m_pos.x * m_siz, m_basePos.y + info.m_pos.y * m_siz };
		float dist = (pos - m_minMapPlayerPos).Length();

		if (dist < discoverRadius)
		{
			info.m_drawFlg = true;
		}

		if (!info.m_drawFlg) { continue; }

		//ほかの部屋も描画する
		if (info.m_LoomNum == m_NOTLoom) { continue; }
		for (auto& info2 : m_posList)
		{
			if (info2.m_LoomNum == info.m_LoomNum)
			{
				info2.m_drawFlg = true;
			}
		}

		if (info.m_LoomNum == m_StairsMineMapInfo.m_LoomNum)
		{
			m_StairsMineMapInfo.m_drawFlg = true;
			m_isStairsMine = true;
		}
	}
}
