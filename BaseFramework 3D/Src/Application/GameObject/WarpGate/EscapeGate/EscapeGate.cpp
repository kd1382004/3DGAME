//---- 自分のヘッダー ----
#include "EscapeGate.h"								//脱出ゲート

//---- シーン ----
#include"../../../Scene/GameScene/GameScene.h"		//ゲームシーン

//---- プレイヤー ----
#include"../../Character/Player/PlayerBase.h"		//プレイヤー

//---- ワープゲート ----
#include"../WarpGateManager.h"						//ワープゲートの管理

//---- 便利機能 ----
#include"../../../Info/VOL/VOLInfo.h"				//音量設定
#include"../../../Info/KeyInfo/KeyInfo.h"			//キー入力

//=============================================
// コンストラクタ・デストラクタ
//=============================================

EscapeGate::~EscapeGate()
{
	auto spEffect = m_wpWarpGateEffect.lock();
	if (spEffect)
	{
		spEffect->StopEffect();
	}
}

//=============================================
// 基本の処理
//=============================================

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
	auto spEffect = m_wpWarpGateEffect.lock();
	if (spEffect)
	{

		if (!spEffect->IsPlaying())
		{
			m_wpWarpGateEffect = KdEffekseerManager::GetInstance().Play(
				"WarpGate/Fine/Fine.efkefc", GetPos(), 1.0f, 1.0f, true
			);
		}
	}
	else
	{
		m_wpWarpGateEffect = KdEffekseerManager::GetInstance().Play(
			"WarpGate/Fine/Fine.efkefc", GetPos(), 1.0f, 1.0f, true
		);
	}


	if (PlayerHit())
	{
		std::shared_ptr<PlayerBase>spPlayer = m_wpPlayer.lock();
		if (!spPlayer) { return; }

		std::shared_ptr<GameScene>spGameScene = m_wpGameScene.lock();

		if (!spGameScene) { return; }

		int key = spPlayer->GetActionKeyConfig().m_interact;
		ShowGuideUI(key);

		if (KeyInfo::Instance().GetValidKeyPush(key))
		{
			// ワープの音
			{
				std::shared_ptr<KdSoundInstance> spSE = KdAudioManager::Instance().Play("Asset/Sound/SE/WarpGate/Warp.wav", false);
				if (spSE) { spSE->SetVolume(VOLInfo::Instance().GetMSSEVol() * 1.0f); }
			}

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
