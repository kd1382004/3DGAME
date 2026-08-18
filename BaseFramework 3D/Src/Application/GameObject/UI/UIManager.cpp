#include "UIManager.h"
#include"UIBase.h"
#include"NextFloorGaugeUI/NextFloorGaugeUI.h"


#include"../Character/Player/PlayerBase.h"

#include"HPBar/HPBar.h"


#include"UIMap/UIMapManager.h"

#include"PlayerInventoryUI/PlayerInventoryUI.h"
void UIManager::Init()
{
	std::shared_ptr<UIMapManager> spUIMapManager = std::make_shared<UIMapManager>();
	spUIMapManager->Init();
	m_wpUIMapManager = spUIMapManager;
	m_spUIList.push_back(spUIMapManager);


	m_pauseStop = false;

	m_spPlayerInventoryUI = std::make_shared<PlayerInventoryUI>();
	m_spPlayerInventoryUI->Init();
	m_spUIList.push_back(m_spPlayerInventoryUI);
}

void UIManager::PreUpdate()
{
	// Updateの前の更新処理
	// オブジェクトリストの整理 ・・・ 無効なオブジェクトを削除
	auto it = m_spUIList.begin();

	while (it != m_spUIList.end())
	{
		if ((*it)->IsExpired())	// IsExpired() ・・・ 無効ならtrue
		{
			// 無効なオブジェクトをリストから削除
			it = m_spUIList.erase(it);
		}
		else
		{
			++it;	// 次の要素へイテレータを進める
		}
	}
	//ソートする(描画優先が大きいほど後に)
	if (m_spUIListOldSiz != m_spUIList.size())
	{
		m_spUIList.sort(
			[](const std::shared_ptr<UIBase>& a, const std::shared_ptr<UIBase>& b)
			{
				return a->GetLayerPriority() > b->GetLayerPriority();
			}
		);

		m_spUIListOldSiz = m_spUIList.size();
	}


	for (auto UI : m_spUIList)
	{
		UI->PreUpdate();
	}
}

void UIManager::Update()
{
	for (auto UI : m_spUIList)
	{
		UI->Update();
	}
}

void UIManager::PreDraw()
{
	for (auto UI : m_spUIList)
	{
		UI->PreDraw();
	}	
}

void UIManager::DrawSprite()
{
	for (auto UI : m_spUIList)
	{
		UI->DrawSprite();
	}
}

void UIManager::SetPlayer(std::shared_ptr<PlayerBase> _spPlayerBase)
{
	m_wpPlayerBase = _spPlayerBase;
	m_spPlayerInventoryUI->SetPlayerBase(_spPlayerBase);

	std::shared_ptr<NextFloorGaugeUI> spNextFloorGaugeUI = std::make_shared<NextFloorGaugeUI>();
	spNextFloorGaugeUI->Init();
	m_spUIList.push_back(spNextFloorGaugeUI);

	//プレイヤーにUIを渡す
	std::shared_ptr<PlayerBase> spPlayerBase = m_wpPlayerBase.lock();
	if (spPlayerBase)
	{
		spPlayerBase->SetNextFloorGaugeUI(spNextFloorGaugeUI);
	}

}

void UIManager::SetGameScene(std::shared_ptr<GameScene> _spGameScene)
{
	m_wpGameScene = _spGameScene;

	if (m_spPlayerInventoryUI)
	{
		m_spPlayerInventoryUI->SetGameScene(_spGameScene);
	}
}

void UIManager::SetPotionUseController(std::shared_ptr<PotionUseController> _spPotionUseController)
{
	m_spPlayerInventoryUI->SetPotionUseController(_spPotionUseController);
}

