//---- 自分のヘッダー ----
#include "UIManager.h"									//UIの管理

//---- プレイヤー ----
#include"../Character/Player/PlayerBase.h"				//プレイヤー

//---- ポーション ----
#include"../Potions/PotionTexInfo/PotionTexInfo.h"		//アイテムの名前・画像情報

//---- UI ----
#include"UIBase.h"										//UIの基底クラス
#include"NextFloorGaugeUI/NextFloorGaugeUI.h"			//次の階へ進むゲージ
#include"HPBar/HPBar.h"									//HPバー
#include"UI2/UIBase2.h"									//キー表示付きUIの基底クラス
#include"UIMap/UIMapManager.h"							//ミニマップの管理
#include"PlayerInventoryUI/PlayerInventoryUI.h"			//インベントリ画面

//=============================================
// 基本の処理
//=============================================

void UIManager::Init()
{
	m_pauseStop = false;

	m_spUIBase2 = std::make_shared<UIBase2>();
	m_spUIList.push_back(m_spUIBase2);

	if (!m_spTreasureChestUiTex)
	{
		m_spTreasureChestUiTex = std::make_shared<KdTexture>();
		m_spTreasureChestUiTex->Load("Asset/Textures/GameUI/TreasureChest/TreasureChestOpenUI.png");
	}
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

	if (m_spUIBase2)
	{
		m_spUIBase2->SetDrawFlg(false);
	}


	for (auto ui : m_spUIList)
	{
		ui->PreUpdate();
	}
}

void UIManager::Update()
{
	for (auto ui : m_spUIList)
	{
		ui->Update();
	}
}

void UIManager::PreDraw()
{
	//ソートする(描画優先数字が大きいほど後に)
	if (m_needSort)
	{
		m_spUIList.sort(
			[](const std::shared_ptr<UIBase>& _spA, const std::shared_ptr<UIBase>& _spB)
			{
				return _spA->GetLayerPriority() > _spB->GetLayerPriority();
			}
		);

		m_needSort = false;
	}

	for (auto ui : m_spUIList)
	{
		ui->PreDraw();
	}	
}

void UIManager::DrawSprite()
{
	for (auto ui : m_spUIList)
	{
		ui->DrawSprite();
	}
}

//=============================================
// 設定
//=============================================

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

void UIManager::AddUIObj(std::shared_ptr<UIBase> _spUI)
{
	m_spUIList.push_back(_spUI); 
	m_needSort = true;
}

void UIManager::SetPotionUseController(std::shared_ptr<PotionUseController> _spPotionUseController)
{
	m_spPlayerInventoryUI->SetPotionUseController(_spPotionUseController);
}

void UIManager::AddUIMapManager()
{
	std::shared_ptr<UIMapManager> spUIMapManager = std::make_shared<UIMapManager>();
	spUIMapManager->Init();
	m_wpUIMapManager = spUIMapManager;
	m_spUIList.push_back(spUIMapManager);
}

void UIManager::AddPlayerInventoryUI()
{
	m_spPlayerInventoryUI = std::make_shared<PlayerInventoryUI>();
	m_spPlayerInventoryUI->Init();
	m_spPlayerInventoryUI->SetPotionTexInfo(m_spPotionTexInfo);
	m_spUIList.push_back(m_spPlayerInventoryUI);
}

void UIManager::SetPotionTexInfo()
{
	if (!m_spPotionTexInfo)
	{
		m_spPotionTexInfo = std::make_shared<PotionTexInfo>();
	}
}

void UIManager::SetMinimapRevealAll(bool _enable)
{
	std::shared_ptr<UIMapManager>spUIMap = m_wpUIMapManager.lock();
	if (spUIMap)
	{
		spUIMap->SetMinimapRevealAll(_enable);
	}
}
