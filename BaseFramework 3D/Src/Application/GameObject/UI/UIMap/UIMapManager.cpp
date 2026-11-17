#include "UIMapManager.h"
#include"UIMap_Map/UIMap_Map.h"
#include"UIMap_Player/UIMap_Player.h"
#include"UIMap_Enemy/UIMap_Enemy.h"
#include"UIMap_TreasureChest/UIMap_TreasureChest.h"
void UIMapManager::Init()
{
	m_basePos = { -600,350 };


	if (!m_spUIMap_Map)
	{
		m_spUIMap_Map = std::make_shared<UIMap_Map>();
		m_spUIMap_Map->Init();
		m_spUIMap_Map->SetBasePos(m_basePos);


		if (!m_spUIMap_Player)
		{
			m_spUIMap_Player = std::make_shared<UIMap_Player>();
			m_spUIMap_Player->Init();
			m_spUIMap_Player->SetBasePos(m_basePos);
			m_spUIMap_Player->SetMapTexSiz(m_spUIMap_Map->GetMapTexSiz());
		}

		if (!m_spUIMap_Enemy)
		{
			m_spUIMap_Enemy = std::make_shared<UIMap_EnemyManage>();
			m_spUIMap_Enemy->Init();
			m_spUIMap_Enemy->SetBasePos(m_basePos);
			m_spUIMap_Enemy->SetUIMap_Map(m_spUIMap_Map);
			m_spUIMap_Enemy->SetMapTexSiz(m_spUIMap_Map->GetMapTexSiz());
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
	if (m_spUIMap_Player)
	{
		m_spUIMap_Player->PreDraw();
	}

	if (m_spUIMap_Map)
	{
		m_spUIMap_Map->SetMiniMapPlayerPos(m_spUIMap_Player->GetPos());
		m_spUIMap_Map->PreDraw();
	}
}

void UIMapManager::DrawSprite()
{
	if (m_spUIMap_Map)
	{
		m_spUIMap_Map->DrawSprite();
	}

	if (m_spUIMap_Enemy)
	{
		m_spUIMap_Enemy->DrawSprite();
	}

	if (m_spUIMap_Player)
	{
		m_spUIMap_Player->DrawSprite();
	}

	for (auto mapTreasureChest : m_UIMap_TreasureChest)
	{
		mapTreasureChest->DrawSprite();
	}
}

void UIMapManager::SetBase3DPos(Math::Vector3 _base3DPos)
{
	m_base3DPos = _base3DPos;

	if (m_spUIMap_Player)
	{
		m_spUIMap_Player->SetBase3DPos(m_base3DPos);
	}

	if (m_spUIMap_Map)
	{
		m_spUIMap_Map->SetBase3DPos(m_base3DPos);
	}

	if (m_spUIMap_Enemy)
	{
		m_spUIMap_Enemy->SetBase3DPos(m_base3DPos);
	}
}

void UIMapManager::SetTileSiz(float _siz)
{
	{ m_tileSiz = _siz; }

	if (m_spUIMap_Player)
	{
		m_spUIMap_Player->SetTileSiz(m_tileSiz);
	}

	if (m_spUIMap_Enemy)
	{
		m_spUIMap_Enemy->SetTileSiz(m_tileSiz);
	}
}

void UIMapManager::AddUIMap_TreasureChest(std::shared_ptr<UIMap_TreasureChest> _spTreasureChest,Math::Vector3 _3DPos)
{
	_spTreasureChest->Init();
	_spTreasureChest->SetMapTexSiz(m_spUIMap_Map->GetMapTexSiz());
	_spTreasureChest->SetUIMap_Map(m_spUIMap_Map);
	_spTreasureChest->SetBase3DPos(m_base3DPos);
	_spTreasureChest->SetBasePos(m_basePos);
	_spTreasureChest->SetTileSiz(m_tileSiz);
	_spTreasureChest->SetPos(_3DPos, m_tileSiz);
	m_UIMap_TreasureChest.push_back(_spTreasureChest);
}

void UIMapManager::SetMinimapRevealAll(bool _enable)
{
	if (m_spUIMap_Map)
	{
		m_spUIMap_Map->SetMinimapRevealAll(_enable);
	}
}
