#include "EscapeGate.h"
#include"../../../Info/KeyInfo/KeyInfo.h"
#include"../../Character/Player/PlayerBase.h"
#include"../../../Scene/GameScene/GameScene.h"
#include"../WarpGateManager.h"
EscapeGate::~EscapeGate()
{
	auto spEffect = m_wpWarpGatEffect.lock();
	if (spEffect)
	{
		spEffect->StopEffect();
	}
}
void EscapeGate::Init()
{

	if (!m_spWarpGate)
	{
		m_spWarpGate = std::make_shared<KdModelData>();
		m_spWarpGate->Load("Asset/Models/WarpGate/Fine/WarpGateblend.gltf");
	}

	if (!m_spGuideUITex)
	{
		m_spGuideUITex = std::make_shared<KdTexture>();
		m_spGuideUITex->Load("Asset/Textures/GameUI/WarpGate/Escape.png");
	}

	WarpGateBase::Init();
}

void EscapeGate::Update()
{
	auto spEffect = m_wpWarpGatEffect.lock();
	if (spEffect)
	{

		if (!spEffect->IsPlaying())
		{
			m_wpWarpGatEffect = KdEffekseerManager::GetInstance().Play(
				"WarpGate/Fine/Fine.efkefc", GetPos(), 1.0f, 1.0f, true
			);
		}
	}
	else
	{
		m_wpWarpGatEffect = KdEffekseerManager::GetInstance().Play(
			"WarpGate/Fine/Fine.efkefc", GetPos(), 1.0f, 1.0f, true
		);
	}


	if (PlayerHit())
	{
		std::shared_ptr<PlayerBase>spPlayer = m_wpPlayer.lock();
		if (!spPlayer) { return; }

		std::shared_ptr<GameScene>spGameScene = m_wpGameScene.lock();

		if (!spGameScene) { return; }

		int key = spPlayer->GetActionKeyConfig().interact;
		ShowGuideUI(key);

		if (KeyInfo::Instance().GetValidKeyPush(key))
		{
			spGameScene->ChangeResultScene();
			m_isExpired = true;
			if (spEffect)
			{
				spEffect->StopEffect();
			}

			auto spWarpGateManager = m_wpWarpGateManager.lock();
			if (spWarpGateManager)
			{
				spWarpGateManager->WarpGateReset();
			}

		}


	}
}
