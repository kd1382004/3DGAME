//---- 自分のヘッダー ----
#include "UIMap_Enemy.h"				//ミニマップの敵アイコン

//---- UI ----
#include"../UIMap_Map/UIMap_Map.h"		//ミニマップの地形

//=============================================
// 基本の処理
//=============================================

void UIMap_EnemyManage::Init()
{
	if (!m_spEnemyTex)
	{
		m_spEnemyTex = std::make_shared<KdTexture>();
		m_spEnemyTex->Load("Asset/Textures/GameUI/Map/Enemy.png");

		m_angle = 0;
	}
}

void UIMap_EnemyManage::DrawSprite()
{
	if (!m_spEnemyTex) { return; }
	for (auto& posL : m_posList)
	{
		
		DiscoverEnemy(&posL);

		if (!posL.m_drawFlg) { continue; }
		Math::Vector2 pos = Change2DPos(posL.m_pos);
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spEnemyTex, m_basePos.x + pos.x * m_size, m_basePos.y + pos.y * m_size, nullptr, nullptr, { 0.5,0.5 }, posL.m_angle);
	}
}

//=============================================
// 設定
//=============================================

void UIMap_EnemyManage::SetEnemyPosAngleList(Math::Vector3 _pos, float _angle)
{
 	m_posList.push_back({ _pos,_angle });
}

//=============================================
// その他の処理
//=============================================

void UIMap_EnemyManage::PosListReset()
{
	m_posList.clear();
}

Math::Vector2 UIMap_EnemyManage::Change2DPos(Math::Vector3 _pos)
{

	float tileX = (_pos.x - m_base3DPos.x) / m_tileSize;
	float tileY = (_pos.z - m_base3DPos.z) / m_tileSize;

	//タイル中心
	float px = tileX * m_mapTexSize.x + m_mapTexSize.x * 0.5f;
	float py = tileY * m_mapTexSize.y + m_mapTexSize.y * 0.5f;

	m_enemy2DPos = { px, py };

	return m_enemy2DPos;
}

void UIMap_EnemyManage::DiscoverEnemy(List* _info)
{
	auto spUIMap_Map = m_wpUIMap_Map.lock();
	if (!spUIMap_Map) { return; }

	int enemyTileX = (int)((_info->m_pos.x - m_base3DPos.x) / m_tileSize);
	int enemyTileY = (int)((_info->m_pos.z - m_base3DPos.z) / m_tileSize);

	const MiniMapInfo* tile = spUIMap_Map->FindTile(enemyTileX, enemyTileY);
	_info->m_drawFlg = tile ? tile->m_drawFlg : false;

}
