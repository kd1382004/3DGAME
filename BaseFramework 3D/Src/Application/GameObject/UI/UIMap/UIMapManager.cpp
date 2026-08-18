#include "UIMapManager.h"
#include"UIMap_Map/UIMap_Map.h"
#include"UIMap_Player/UIMap_Player.h"
#include"UIMap_Enemy/UIMap_Enemy.h"
#include"UIMap_TreasureChest/UIMap_TreasureChest.h"
void UIMapManager::Init()
{
	m_basePos = { -600,350 };


	if (!m_UIMap_Map)
	{
		m_UIMap_Map = std::make_shared<UIMap_Map>();
		m_UIMap_Map->Init();
		m_UIMap_Map->SetBasePos(m_basePos);


		if (!m_UIMap_Player)
		{
			m_UIMap_Player = std::make_shared<UIMap_Player>();
			m_UIMap_Player->Init();
			m_UIMap_Player->SetBasePos(m_basePos);
			m_UIMap_Player->SetMapTexSiz(m_UIMap_Map->GetMapTexSiz());
		}

		if (!m_UIMap_Enemy)
		{
			m_UIMap_Enemy = std::make_shared<UIMap_EnemyManage>();
			m_UIMap_Enemy->Init();
			m_UIMap_Enemy->SetBasePos(m_basePos);
			m_UIMap_Enemy->SetUIMap_Map(m_UIMap_Map);
			m_UIMap_Enemy->SetMapTexSiz(m_UIMap_Map->GetMapTexSiz());
		}

	}

	m_layerPriority = 2;
}

void UIMapManager::PreUpdate()
{
	auto it = m_UIMap_TreasureChest.begin();

	while (it != m_UIMap_TreasureChest.end())
	{
		if ((*it)->IsExpired())	// IsExpired() ・・・ 無効ならtrue
		{
			// 無効なオブジェクトをリストから削除
			it = m_UIMap_TreasureChest.erase(it);
		}
		else
		{
			++it;	// 次の要素へイテレータを進める
		}
	}
}

void UIMapManager::PreDraw()
{
	KdShaderManager::Instance().m_spriteShader.DrawCircle(m_basePos.x, m_basePos.y, 5);
}

void UIMapManager::DrawSprite()
{
	if (m_UIMap_Map)
	{
		m_UIMap_Map->SetMinMapPlayerPos(m_UIMap_Player->GetPos());
		m_UIMap_Map->DrawSprit();
	}

	if (m_UIMap_Enemy)
	{
		m_UIMap_Enemy->DrawSprit();
	}

	if (m_UIMap_Player)
	{
		m_UIMap_Player->DrawSprit();
	}

	for (auto Map_TreasureChest : m_UIMap_TreasureChest)
	{
		Map_TreasureChest->SetMinMapPlayerPos(m_UIMap_Player->GetPos());
		Map_TreasureChest->DrawSprite();
	}
}

void UIMapManager::SetBase3DPos(Math::Vector3 _base3DPos)
{
	m_base3DPos = _base3DPos;

	if (m_UIMap_Player)
	{
		m_UIMap_Player->SetBase3DPos(m_base3DPos);
	}

	if (m_UIMap_Map)
	{
		m_UIMap_Map->SetBase3DPos(m_base3DPos);
	}

	if (m_UIMap_Enemy)
	{
		m_UIMap_Enemy->SetBase3DPos(m_base3DPos);
	}
}

void UIMapManager::SetTileSiz(float _siz)
{
	{ m_tileSiz = _siz; }

	if (m_UIMap_Player)
	{
		m_UIMap_Player->SetTileSiz(m_tileSiz);
	}

	if (m_UIMap_Enemy)
	{
		m_UIMap_Enemy->SetTileSiz(m_tileSiz);
	}
}

void UIMapManager::AddUIMap_TreasureChest(std::shared_ptr<UIMap_TreasureChest> _TreasureChest,Math::Vector3 _3Dpos)
{
	_TreasureChest->Init();
	_TreasureChest->SetMapTexSiz(m_UIMap_Map->GetMapTexSiz());
	_TreasureChest->SetUIMap_Map(m_UIMap_Map);
	_TreasureChest->SetBase3DPos(m_base3DPos);
	_TreasureChest->SetBasePos(m_basePos);
	_TreasureChest->SetTileSiz(m_tileSiz);
	_TreasureChest->SetPos(_3Dpos, m_tileSiz);
	m_UIMap_TreasureChest.push_back(_TreasureChest);
}
