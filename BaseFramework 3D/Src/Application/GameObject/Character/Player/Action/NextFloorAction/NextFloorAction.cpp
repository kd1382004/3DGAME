//---- 自分のヘッダー ----
#include "NextFloorAction.h"										//次の階へ進む操作

//---- シーン ----
#include"../../../../../Scene/GameScene/GameScene.h"				//ゲームシーン

//---- UI ----
#include"../../../../UI/NextFloorGaugeUI/NextFloorGaugeUI.h"		//次の階へ進むゲージ

//---- 便利機能 ----
#include"../../../../../Info/VOL/VOLInfo.h"							//音量設定
#include"../../../../../Info/KeyInfo/KeyInfo.h"						//キー入力

//=============================================
// 基本の処理
//=============================================

void NextFloorAction::Init()
{
	m_nextFloorCount = 0;
}

void NextFloorAction::Update(bool _nextFloorActionFlg)
{

	//ゲージを描画するかどうかのフラグをUIに渡す
	std::shared_ptr<NextFloorGaugeUI> spNextFloorGaugeUI = m_wpNextFloorGaugeUI.lock();
	if (spNextFloorGaugeUI)
	{
		spNextFloorGaugeUI->SetDrawFlg(_nextFloorActionFlg);
	}



	if (!_nextFloorActionFlg) { return; }
	


	if (KeyInfo::Instance().GetValidKeyPush(m_actionKey))
	{
		m_nextFloorCount++;
	}
	else
	{
		m_nextFloorCount = 0;
	}

	//UIにゲージの割合を渡す
	if (spNextFloorGaugeUI)
	{
		spNextFloorGaugeUI->SetGaugePercent((float)m_nextFloorCount / (float)kNextFloorCountMax);
	}


	if (m_nextFloorCount >= kNextFloorCountMax)
	{
		//次の階に行く
		auto spGameScene = m_wpGameScene.lock();
		if (spGameScene)
		{
			// ワープの音
			{
				std::shared_ptr<KdSoundInstance> spSE = KdAudioManager::Instance().Play("Asset/Sound/SE/WarpGate/Warp.wav", false);
				if (spSE) { spSE->SetVolume(VOLInfo::Instance().GetMSSEVol() * 1.0f); }
			}

			spGameScene->RequestNextFloor();
		}

		m_nextFloorCount = 0;
	}
}

//=============================================
// 設定
//=============================================

void NextFloorAction::SetNextFloorGaugeUI(const std::shared_ptr<NextFloorGaugeUI>& _spNextFloorGaugeUI)
{
	m_wpNextFloorGaugeUI = _spNextFloorGaugeUI;
	if (_spNextFloorGaugeUI)
	{
		_spNextFloorGaugeUI->SetActionKey(m_actionKey);
	}
}
