#include "NextFloorAction.h"
#include"../../../../../Info/KeyInfo/KeyInfo.h"
#include"../../../../../Scene/GameScene/GameScene.h"

#include"../../../../UI/NextFloorGaugeUI/NextFloorGaugeUI.h"

void NextFloorAction::Init()
{
	m_nextFloorCount = 0;
}

void NextFloorAction::Update(bool _nextFloorActionFlg)
{

	//ゲージを描画するかどうかのフラグをUIに渡す
	std::shared_ptr<NextFloorGaugeUI> nextFloorGaugeUI = m_wpNextFloorGaugeUI.lock();
	if (nextFloorGaugeUI)
	{
		nextFloorGaugeUI->setDrawFlg(_nextFloorActionFlg);
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
	if (nextFloorGaugeUI)
	{
		nextFloorGaugeUI->SetGaugePercent((float)m_nextFloorCount / (float)m_nextFloorCountMax);
	}


	if (m_nextFloorCount >= m_nextFloorCountMax)
	{
		//次の階に行く
		auto gameScene = m_wpGameScene.lock();
		if (gameScene)
		{
			gameScene->GenerateMap();
		}

		m_nextFloorCount = 0;
	}
}

void NextFloorAction::SetNextFloorGaugeUI(const std::shared_ptr<NextFloorGaugeUI>& _NextFloorGaugeUI)
{
	m_wpNextFloorGaugeUI = _NextFloorGaugeUI;
	if (_NextFloorGaugeUI)
	{
		_NextFloorGaugeUI->SetActionKey(m_actionKey);
	}
}
