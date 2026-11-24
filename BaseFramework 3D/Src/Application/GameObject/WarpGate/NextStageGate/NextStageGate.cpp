//---- 自分のヘッダー ----
#include "NextStageGate.h"							//次のステージへのゲート

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

NextStageGate::~NextStageGate()
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

void NextStageGate::Init()
{

	if (!m_spWarpGate)
	{
		m_spWarpGate = std::make_shared<KdModelData>();
		m_spWarpGate->Load("Asset/Models/WarpGate/Next/WarpGateblend.gltf");
	}

	if (!m_spGuideUITex)
	{
		m_spGuideUITex = std::make_shared<KdTexture>();
		m_spGuideUITex->Load("Asset/Textures/GameUI/WarpGate/Next.png");
	}

	WarpGateBase::Init();
}

void NextStageGate::Update()
{
	auto spEffect = m_wpWarpGateEffect.lock();
	if (spEffect)
	{

		if (!spEffect->IsPlaying())
		{
			m_wpWarpGateEffect = KdEffekseerManager::GetInstance().Play(
				"WarpGate/Next/Next.efkefc", GetPos(), 1.0f, 1.0f, true
			);
		}
	}
	else
	{
		m_wpWarpGateEffect = KdEffekseerManager::GetInstance().Play(
			"WarpGate/Next/Next.efkefc", GetPos(), 1.0f, 1.0f, true
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

			spGameScene->GenerateMap();
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
