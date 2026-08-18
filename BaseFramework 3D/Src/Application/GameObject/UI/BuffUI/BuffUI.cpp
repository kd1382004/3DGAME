#include "BuffUI.h"

#include"../../Potions/PotionTexInfo/PotionTexInfo.h"
void BuffUI::Init()
{
	m_iconSiz = { 20,20 };
}

void BuffUI::Update()
{
}

void BuffUI::DrawSprite()
{

	for (auto buffIcon : m_PotionBuffUIInfoList)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spGaugeTexture, buffIcon.m_pos.x, buffIcon.m_pos.y, m_iconSiz.x, m_iconSiz.y);
	}
}
