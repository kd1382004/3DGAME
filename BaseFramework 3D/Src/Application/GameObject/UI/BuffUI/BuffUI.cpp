//---- 自分のヘッダー ----
#include "BuffUI.h"															//バフの表示

//---- プレイヤー ----
#include"../../Character/Player/PlayerBase.h"								//プレイヤー
#include"../../Character/Player/PlayerBuffManager/PlayerBuffManager.h"		//プレイヤーのバフ管理

//---- ポーション ----
#include"../../Potions/PotionTexInfo/PotionTexInfo.h"						//アイテムの名前・画像情報

void BuffUI::Init()
{
	if (!m_spBuffTex)
	{
		m_spBuffTex = std::make_shared<KdTexture>();
		m_spBuffTex->Load("Asset/Textures/GameUI/Buff/Buff.png");


		m_iconSize = { (float)m_spBuffTex->GetHeight() * 2,(float)m_spBuffTex->GetHeight() * 2 };
	}
}

void BuffUI::PreDraw()
{
	m_potionBuffUIInfoList.clear();

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
		buffUI.m_pos.x += m_iconSize.x * i;

		m_potionBuffUIInfoList.push_back(buffUI);
		i++;
	}

}

void BuffUI::DrawSprite()
{

	std::shared_ptr<PotionTexInfo>spPotionTexInfo = m_wpPotionTexInfo.lock();
	if (!spPotionTexInfo) { return; }

	for (auto buffIcon : m_potionBuffUIInfoList)
	{
		Math::Vector2 iconPiv = { 0.5f, 0.5f };
		KdShaderManager::Instance().m_spriteShader.DrawTex(spPotionTexInfo->GetPotionIcon(buffIcon.m_ID), buffIcon.m_pos.x, buffIcon.m_pos.y, m_iconSize.x, m_iconSize.y, nullptr, nullptr, iconPiv);

		Math::Vector2 needlePiv = { 0.5f, 0.0f };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spBuffTex, buffIcon.m_pos.x, buffIcon.m_pos.y, nullptr, nullptr, needlePiv, buffIcon.m_angle);
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spBuffTex, buffIcon.m_pos.x, buffIcon.m_pos.y, nullptr, nullptr, needlePiv);
	}
}
