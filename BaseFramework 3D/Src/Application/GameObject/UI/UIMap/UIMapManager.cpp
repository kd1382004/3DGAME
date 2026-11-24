//---- 自分のヘッダー ----
#include "UIMapManager.h"								//ミニマップの管理

//---- UI ----
#include"UIMap_Map/UIMap_Map.h"							//ミニマップの地形
#include"UIMap_Player/UIMap_Player.h"					//ミニマップのプレイヤーアイコン
#include"UIMap_Enemy/UIMap_Enemy.h"						//ミニマップの敵アイコン
#include"UIMap_TreasureChest/UIMap_TreasureChest.h"		//ミニマップの宝箱アイコン

//=============================================
// 基本の処理
//=============================================

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
			m_spUIMap_Player->SetMapTexSize(m_spUIMap_Map->GetMapTexSize());
		}

		if (!m_spUIMap_Enemy)
		{
			m_spUIMap_Enemy = std::make_shared<UIMap_EnemyManage>();
			m_spUIMap_Enemy->Init();
			m_spUIMap_Enemy->SetBasePos(m_basePos);
			m_spUIMap_Enemy->SetUIMap_Map(m_spUIMap_Map);
			m_spUIMap_Enemy->SetMapTexSize(m_spUIMap_Map->GetMapTexSize());
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

	for (auto mapTreasureChest : m_UIMap_TreasureChest)
	{
		if (mapTreasureChest)
		{
			mapTreasureChest->DrawSprite();
		}
	}


	if (m_spUIMap_Enemy)
	{
		m_spUIMap_Enemy->DrawSprite();
	}

	if (m_spUIMap_Player)
	{
		m_spUIMap_Player->DrawSprite();
	}
}

//=============================================
// 設定
//=============================================

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

void UIMapManager::SetTileSize(float _size)
{
	{ m_tileSize = _size; }

	if (m_spUIMap_Player)
	{
		m_spUIMap_Player->SetTileSize(m_tileSize);
	}

	if (m_spUIMap_Enemy)
	{
		m_spUIMap_Enemy->SetTileSize(m_tileSize);
	}
}

void UIMapManager::AddUIMap_TreasureChest(std::shared_ptr<UIMap_TreasureChest> _spTreasureChest,Math::Vector3 _3DPos)
{
	_spTreasureChest->Init();
	_spTreasureChest->SetMapTexSize(m_spUIMap_Map->GetMapTexSize());
	_spTreasureChest->SetUIMap_Map(m_spUIMap_Map);
	_spTreasureChest->SetBase3DPos(m_base3DPos);
	_spTreasureChest->SetBasePos(m_basePos);
	_spTreasureChest->SetTileSize(m_tileSize);
	_spTreasureChest->SetPos(_3DPos, m_tileSize);
	m_UIMap_TreasureChest.push_back(_spTreasureChest);
}

void UIMapManager::SetMinimapRevealAll(bool _enable)
{
	if (m_spUIMap_Map)
	{
		m_spUIMap_Map->SetMinimapRevealAll(_enable);
	}
}
