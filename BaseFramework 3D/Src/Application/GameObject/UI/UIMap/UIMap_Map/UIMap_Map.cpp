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


	m_rtPack.CreateRenderTarget(1280, 720); // 画面サイズに合わせる
}

void UIMap_Map::DrawSprit()
{

	if (!m_mapTex) { return; }

	KdShaderManager::Instance().m_spriteShader.DrawTex(m_rtPack.m_RTTexture, 0, 0, 1280, 720);

	if (!m_mapStairsTex) { return; }
	if (!m_StairsMineMapInfo.m_drawFlg) { return; }
	if (!m_isStairsMine) { return; }

	float drawX = m_basePos.x + m_StairsMineMapInfo.m_pos.x * m_siz;
	float drawY = m_basePos.y + m_StairsMineMapInfo.m_pos.y * m_siz;

	KdShaderManager::Instance().m_spriteShader.DrawTex(m_mapStairsTex, drawX, drawY, m_mapTexSiz.x * 1.5, m_mapTexSiz.y * 1.5);
}

void UIMap_Map::PreDraw()
{
	DiscoverTile();
	if (m_isDirty) { BakeTiles(); m_isDirty = false; }


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

	m_tileMap[MakeKey((int)tileX, (int)tileY)] = &m_posList.back();
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
	m_tileMap.clear();
	m_StairsMineMapInfo.m_drawFlg = false;
	m_isStairsMine = false;
	m_isDirty = true;
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

		m_isDirty = true;
	}
}

void UIMap_Map::DiscoverTile()
{
	// プレイヤーがミニマップ上のどのタイルにいるか
	int px = (int)((m_minMapPlayerPos.x - m_basePos.x - m_mapTexSiz.x * 0.5f) / (m_mapTexSiz.x * m_siz));
	int py = (int)((m_minMapPlayerPos.y - m_basePos.y - m_mapTexSiz.y * 0.5f) / (m_mapTexSiz.y * m_siz));

	float discoverRadius = m_discoverTileNum * m_mapTexSiz.x;
	int r = m_discoverTileNum + 1;
	std::unordered_set<int> revealedRooms;

	// 周囲のタイルだけ調べる
	for (int y = py - r; y <= py + r; y++)
	{
		for (int x = px - r; x <= px + r; x++)
		{
			auto it = m_tileMap.find(MakeKey(x, y));
			if (it == m_tileMap.end()) { continue; }

			MineMapInfo* info = it->second;
			if (info->m_drawFlg) { continue; }

			Math::Vector2 pos = { m_basePos.x + info->m_pos.x * m_siz, m_basePos.y + info->m_pos.y * m_siz };
			if ((pos - m_minMapPlayerPos).Length() < discoverRadius)
			{
				info->m_drawFlg = true;
				m_isDirty = true; // ②（テクスチャに焼く方法）を入れない場合はこの行は不要
				if (info->m_LoomNum != m_NOTLoom) { revealedRooms.insert(info->m_LoomNum); }
			}
		}
	}

	// 新しく部屋を見つけたときだけ、その部屋のタイルをまとめて表示（今の処理のまま）
	if (!revealedRooms.empty())
	{
		for (auto& info : m_posList)
		{
			if (revealedRooms.count(info.m_LoomNum)) { info.m_drawFlg = true; }
		}
		if (revealedRooms.count(m_StairsMineMapInfo.m_LoomNum))
		{
			m_StairsMineMapInfo.m_drawFlg = true;
			m_isStairsMine = true;
		}
		m_isDirty = true; // 同上
	}
}


void UIMap_Map::BakeTiles()
{
	KdRenderTargetChanger changer;
	changer.ChangeRenderTarget(m_rtPack);
	m_rtPack.ClearTexture({ 0, 0, 0, 0 });

	auto& sprite = KdShaderManager::Instance().m_spriteShader;
	sprite.Begin();
	for (const auto& info : m_posList)
	{
		if (!info.m_drawFlg) { continue; }
		sprite.DrawTex(m_mapTex, m_basePos.x + info.m_pos.x * m_siz, m_basePos.y + info.m_pos.y * m_siz,
			m_mapTexSiz.x, m_mapTexSiz.y);
	}
	sprite.End();

	changer.UndoRenderTarget();
}