#include "UIMapManager.h"
#include"UIMap_Map/UIMap_Map.h"
#include"UIMap_Player/UIMap_Player.h"
void UIMapManager::Init()
{
	m_basePos = { -600,350 };


	if (!m_UIMap_Map)
	{
		m_UIMap_Map = std::make_shared<UIMap_Map>();
		m_UIMap_Map->Init();
		m_UIMap_Map->SetBasePos(m_basePos);
	}

	if (!m_UIMap_Player)
	{
		m_UIMap_Player = std::make_shared<UIMap_Player>();
		m_UIMap_Player->Init();
		m_UIMap_Player->SetBasePos(m_basePos);
	}

}

void UIMapManager::Update()
{
}

void UIMapManager::PreDraw()
{
	KdShaderManager::Instance().m_spriteShader.DrawCircle(m_basePos.x, m_basePos.y, 5);
}

void UIMapManager::DrawSprite()
{
	if (m_UIMap_Map)
	{
		m_UIMap_Map->DrawSprit();
	}

	if (m_UIMap_Player)
	{
		m_UIMap_Player->DrawSprit();
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
}

void UIMapManager::SetTileSiz(float _siz)
{
	{ m_tileSiz = _siz; }

	if (m_UIMap_Player)
	{
		m_UIMap_Player->SetTileSiz(m_tileSiz);
	}

}
