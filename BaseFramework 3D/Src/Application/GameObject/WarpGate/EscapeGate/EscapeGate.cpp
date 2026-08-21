#include "EscapeGate.h"
#include"../../../Info/KeyInfo/KeyInfo.h"
#include"../../Character/Player/PlayerBase.h"
#include"../../../Scene/GameScene/GameScene.h"
void EscapeGate::Init()
{

	if (!m_spWarpGate)
	{
		m_spWarpGate = std::make_shared<KdModelData>();
		m_spWarpGate->Load("Asset/Models/WarpGate/WarpGateblend.gltf");
	}


	WarpGateBase::Init();
}

void EscapeGate::Update()	
{
	if (PlayerHit())
	{
		std::shared_ptr<PlayerBase>spPlayer = m_wpPlayer.lock();
		if (!spPlayer) { return; }

		std::shared_ptr<GameScene>spGameScene = m_wpGameScene.lock();

		if (!spGameScene) { return; }

		int key = spPlayer->GetActionKeyConfig().interact;

		if (KeyInfo::Instance().GetValidKeyPush(key))
		{
			spGameScene->ChangeResultScene();
			m_isExpired = true;
		}


	}
}
