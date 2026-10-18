#include "BuffUI.h"

#include"../../Potions/PotionTexInfo/PotionTexInfo.h"

//プレイヤー
#include"../../Character/Player/PlayerBase.h"
#include"../../Character/Player/PlayerBuffManager/PlayerBuffManager.h"
void BuffUI::Init()
{
	if (!m_buffTex)
	{
		m_buffTex = std::make_shared<KdTexture>();
		m_buffTex->Load("Asset/Textures/GameUI/Buff/Buff.png");


		m_iconSiz = { (float)m_buffTex->GetHeight() * 2,(float)m_buffTex->GetHeight() * 2 };
	}
}

void BuffUI::PreDraw()
{
	m_PotionBuffUIInfoList.clear();

	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayer.lock();
	if (!spPlayer) { return; }

	std::shared_ptr<PlayerBuffManager>spPlayerBuffManager = spPlayer->GetPlayerBuffManager();


	if (!spPlayerBuffManager) { return; }

	int i = 0;
	for (const auto& buff : spPlayerBuffManager->GetStatusBuff())
	{
		BuffUIInfo buffUI;
		buffUI.m_ID = buff.m_ID;
		buffUI.m_angle = buff.m_nowSeconds / buff.m_maxSeconds * 360;
		buffUI.m_pos = m_2DPos;
		buffUI.m_pos.x += m_iconSiz.x * i;

		m_PotionBuffUIInfoList.push_back(buffUI);
		i++;
	}

}



void BuffUI::DrawSprite()
{

	std::shared_ptr<PotionTexInfo>m_spPotionTexInfo = m_wpPotionTexInfo.lock();
	if (!m_spPotionTexInfo) { return; }

	for (auto buffIcon : m_PotionBuffUIInfoList)
	{
		Math::Vector2 iconPiv = { 0.5f, 0.5f };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spPotionTexInfo->GetPotionIcon(buffIcon.m_ID), buffIcon.m_pos.x, buffIcon.m_pos.y, m_iconSiz.x, m_iconSiz.y, nullptr, nullptr, iconPiv);

		Math::Vector2 needlePiv = { 0.5f, 0.0f };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_buffTex, buffIcon.m_pos.x, buffIcon.m_pos.y, nullptr, nullptr, needlePiv, buffIcon.m_angle);
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_buffTex, buffIcon.m_pos.x, buffIcon.m_pos.y, nullptr, nullptr, needlePiv);
	}
}
