#include "TreasureChest.h"
#include"../Character/Player/PlayerBase.h"

#include"../../Info/KeyInfo/KeyInfo.h"
void TreasureChest::Init()
{
	if (!m_treasureChestModel)
	{
		m_treasureChestModel = std::make_shared<KdModelWork>();
		m_treasureChestModel->SetModelData("Asset/Models/TreasureChest/TreasureBox.gltf");

		m_treasureChestAnimetor = std::make_shared<KdAnimator>();
	}
}


void TreasureChest::Update()
{
	
	if (m_IsOpen)
	{ 
		m_treasureChestAnimetor->AdvanceTime(m_treasureChestModel->WorkNodes(), 100);
		m_treasureChestModel->CalcNodeMatrices();
		return;
	}

	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayer.lock();
	if (spPlayer)
	{
		Math::Vector3 PlayerPos = spPlayer->GetPos();
		Math::Vector3 Pos = GetPos();

		float dist = (PlayerPos - Pos).Length();

		if (dist < 10)
		{


			int key = spPlayer->GetActionKeyConfig().interact;


			if (KeyInfo::Instance().GetValidKeyPush(key, true))
			{
				m_IsOpen = true;
				m_treasureChestAnimetor->SetAnimation(m_treasureChestModel->GetAnimation("Open"), false);
			}
		}
	}
}

void TreasureChest::GenerateDepthMapFromLight()
{
	if (m_treasureChestModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_treasureChestModel, m_mWorld);
	}
}

void TreasureChest::DrawLit()
{
	if (m_treasureChestModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_treasureChestModel, m_mWorld);
	}
}
