//---- 自分のヘッダー ----
#include "UIMap_TreasureChest.h"		//ミニマップの宝箱アイコン

//---- UI ----
#include"../UIMap_Map/UIMap_Map.h"		//ミニマップの地形

//=============================================
// 基本の処理
//=============================================

void UIMap_TreasureChest::Init()
{
	if (!m_spTreasureChestTex)
	{
		m_spTreasureChestTex = std::make_shared<KdTexture>();
		m_spTreasureChestTex->Load("Asset/Textures/GameUI/Map/TreasureChest.png");
		m_basePos = {};
	}

}

void UIMap_TreasureChest::DrawSprite()
{

	if (!m_spTreasureChestTex) { return; }

	DiscoverTile(&m_posList);


	if (!m_posList.m_drawFlg) { return; }
	float drawX = m_basePos.x + m_2DPos.x * m_size;
	float drawY = m_basePos.y + m_2DPos.y * m_size;

	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTreasureChestTex, drawX, drawY);
}

//=============================================
// 設定
//=============================================

void UIMap_TreasureChest::SetPos(Math::Vector3 _3DPos, float _worldTileSize)
{
	Math::Vector3 base = m_base3DPos;

	float tileX = (_3DPos.x - base.x) / _worldTileSize;
	float tileY = (_3DPos.z - base.z) / _worldTileSize;

	float px = tileX * m_mapTexSize.x + m_mapTexSize.x * 0.5f;
	float py = tileY * m_mapTexSize.y + m_mapTexSize.y * 0.5f;

	m_2DPos = { px, py };

	m_posList.m_pos.x = base.x + tileX * _worldTileSize;
	m_posList.m_pos.z = base.z + tileY * _worldTileSize;
}

//=============================================
// その他の処理
//=============================================

void UIMap_TreasureChest::DiscoverTile(List* _info)
{
	auto spUIMap_Map = m_wpUIMap_Map.lock();
	if (!spUIMap_Map) return;
	//  敵のタイル番号
	int treasureChestTileX = (int)((_info->m_pos.x - m_base3DPos.x) / m_tileSize);
	int treasureChestTileY = (int)((_info->m_pos.z - m_base3DPos.z) / m_tileSize);


	const auto& tileList = spUIMap_Map->GetMiniMapInfo();
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
