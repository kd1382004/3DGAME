#include "UIManager.h"
#include"UIBase.h"
#include"NextFloorGaugeUI/NextFloorGaugeUI.h"


#include"../Character/Player/PlayerBase.h"

#include"HPBar/HPBar.h"


#include"UIMap/UIMapManager.h"
void UIManager::Init()
{
	std::shared_ptr<NextFloorGaugeUI> spNextFloorGaugeUI = std::make_shared<NextFloorGaugeUI>();
	spNextFloorGaugeUI->Init();
	m_spUIList.push_back(spNextFloorGaugeUI);

	//プレイヤーにUIを渡す
	std::shared_ptr<PlayerBase> spPlayerBase = m_wpPlayerBase.lock();
	if (spPlayerBase)
	{
		spPlayerBase->SetNextFloorGaugeUI(spNextFloorGaugeUI);
	}


	std::shared_ptr<UIMapManager> spUIMapManager = std::make_shared<UIMapManager>();
	spUIMapManager->Init();
	m_wpUIMapManager = spUIMapManager;
	m_spUIList.push_back(spUIMapManager);

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

