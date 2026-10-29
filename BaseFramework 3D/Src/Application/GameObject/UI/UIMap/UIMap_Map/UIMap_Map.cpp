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


	//デバッグ
	if (GetAsyncKeyState('4') & 0x8000)
	{
		SetMinimapRevealAll(true);
	}
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
	m_isStairsMine = true;
}

void UIMap_Map::PosListReset()
{
	m_posList.clear();
	m_StairsMineMapInfo.m_drawFlg = false;
	m_isStairsMine = false;
}


void UIMap_Map::SetMinimapRevealAll(bool enable)
{
	if (enable)
	{
		for (auto& info : m_posList)
		{
			info.m_drawFlg = true;
			m_StairsMineMapInfo.m_drawFlg = true;
		}
	}
}

void UIMap_Map::DiscoverTile()
{
	float discoverRadius = m_discoverTileNum * m_mapTexSiz.x;
	std::unordered_set<int> revealedRooms;
	for (auto& info : m_posList)
	{
		if (info.m_drawFlg) continue;
		Math::Vector2 pos = { m_basePos.x + info.m_pos.x * m_siz, m_basePos.y + info.m_pos.y * m_siz };
		float dist = (pos - m_minMapPlayerPos).Length();
		if (dist < discoverRadius)
		{
			info.m_drawFlg = true;
			if (info.m_LoomNum != m_NOTLoom)
			{
				revealedRooms.insert(info.m_LoomNum);
			}
		}
	}
	// 新たに視界に入った部屋がある場合のみ、1回だけ全走査してその部屋のタイルを有効化
	if (!revealedRooms.empty())
	{
		for (auto& info : m_posList)
		{
			if (revealedRooms.count(info.m_LoomNum))
			{
				info.m_drawFlg = true;
			}
		}
		if (revealedRooms.count(m_StairsMineMapInfo.m_LoomNum))
		{
			m_StairsMineMapInfo.m_drawFlg = true;
			m_isStairsMine = true;
		}
	}
}