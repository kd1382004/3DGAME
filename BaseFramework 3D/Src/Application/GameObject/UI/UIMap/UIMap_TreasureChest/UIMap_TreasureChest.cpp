#include "UIMap_TreasureChest.h"
#include"../UIMap_Map/UIMap_Map.h"
void UIMap_TreasureChest::Init()
{
	if (!m_treasureChestTex)
	{
		m_treasureChestTex = std::make_shared<KdTexture>();
		m_treasureChestTex->Load("Asset/Textures/GameUI/Map/TreasureChest.png");
		m_basePos = {};
	}

}

void UIMap_TreasureChest::DrawSprite()
{

	if (!m_treasureChestTex) { return; }

	DiscoverTile(&m_posList);


	if (!m_posList.m_drawFlg) { return; }
	float drawX = m_basePos.x + m_2DPos.x * m_siz;
	float drawY = m_basePos.y + m_2DPos.y * m_siz;

	KdShaderManager::Instance().m_spriteShader.DrawTex(m_treasureChestTex, drawX, drawY, m_mapTexSiz.x, m_mapTexSiz.y);
}

void UIMap_TreasureChest::SetPos(Math::Vector3 _3DPos, float _worldTileSize)
{
	Math::Vector3 base = m_base3DPos;

	float tileX = (_3DPos.x - base.x) / _worldTileSize;
	float tileY = (_3DPos.z - base.z) / _worldTileSize;

	float px = tileX * m_mapTexSiz.x + m_mapTexSiz.x * 0.5f;
	float py = tileY * m_mapTexSiz.y + m_mapTexSiz.y * 0.5f;

	m_2DPos = { px, py };

	m_posList.m_pos.x = base.x + tileX * _worldTileSize;
	m_posList.m_pos.z = base.z + tileY * _worldTileSize;
}



void UIMap_TreasureChest::DiscoverTile(List* _info)
{
	auto spUIMap_Map = m_wpUIMap_Map.lock();
	if (!spUIMap_Map) return;
	//  敵のタイル番号
	int treasureChestTileX = (int)((_info->m_pos.x - m_base3DPos.x) / m_tileSiz);
	int treasureChestTileY = (int)((_info->m_pos.z - m_base3DPos.z) / m_tileSiz);


	const auto& tileList = spUIMap_Map->GetMineMapInfo();
	for (const auto& tile : tileList)
	{
		if ((int)tile.m_tileIndex.x == treasureChestTileX && (int)tile.m_tileIndex.y == treasureChestTileY)
		{
			_info->m_drawFlg = tile.m_drawFlg;
			return;
		}
	}
	_info->m_drawFlg = false;
}