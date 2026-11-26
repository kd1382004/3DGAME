//---- 自分のヘッダー ----
#include "ResultScene.h"															//リザルトシーン

//---- シーン ----
#include"../SceneManager.h"															//シーンの管理

//---- プレイヤー ----
#include"../../GameObject/Character/Player/PlayerInventory/PlayerInventory.h"		//プレイヤーの所持品

//---- 武器 ----
#include"../../GameObject/Weapon/WeaponBase.h"										//武器の基底クラス
#include"../../GameObject/Weapon/Dagger/Dagger.h"									//短剣
#include"../../GameObject/Weapon/Sword/Sword.h"										//剣

//---- UI ----
#include"../../GameObject/UI/UIManager.h"											//UIの管理
#include"../../GameObject/UI/ResultUI/ResultDisplay/ResultDisplay.h"				//リザルト画面

//---- ボタン ----
#include"../../GameObject/Button/PrepareButton/PrepareButton.h"						//準備シーンへ進むボタン
#include"../../GameObject/Button/TitleButton/TitleButton.h"							//タイトルシーンへ戻るボタン

//---- 便利機能 ----
#include"../../Info/FadeDraw/FadeDraw.h"											//画面フェードの描画
#include"../../Info/DebugInfo/DebugInfo.h"											//デバッグ用の状態
#include"../../Info/MouseInfo/MouseInfo.h"											//マウス入力

//=============================================
// コンストラクタ・デストラクタ
//=============================================

ResultScene::~ResultScene()
{
	if (m_spPlayerDeadBGM)
	{
		m_spPlayerDeadBGM->Stop();
	}

	if (m_spEscapeBGM)
	{
		m_spEscapeBGM->Stop();
	}
}

//=============================================
// 基本の処理
//=============================================

void ResultScene::ImGUI()
{

}

void ResultScene::Init()
{
	//ショップの再入荷
	RestockShop();

	DebugInfo::Instance().SetSceneManagerImGUIFlg(false);
	GameResult gameResult = SceneManager::Instance().GetGameResult();
	SaveGameRecord(gameResult);
	std::shared_ptr<UIManager>spUIManager = std::make_shared<UIManager>();
	spUIManager->Init();
	m_objList.push_back(spUIManager);

	std::shared_ptr<ResultDisplay>spResultDisplay = std::make_shared<ResultDisplay>();


	std::shared_ptr<WeaponBase>spWeaponBase = nullptr;
	switch (SceneManager::Instance().GetPlayerWeaponInfo())
	{
	case PlayerWeaponInfo_Dagger:
		spWeaponBase = std::make_shared<Dagger>();
		break;
	case PlayerWeaponInfo_Sword:
		spWeaponBase = std::make_shared<Sword>();
		break;
	default:
		spWeaponBase = std::make_shared<WeaponBase>();
		break;
	}
	spWeaponBase->Init();
	// セーブだけではLvMaxや前提ノードがないので、マスタと重ねてから渡す
	spWeaponBase->SetWeaponsStrengtheningInfo(LoadMergedWeaponStrengtheningData(m_weaponStrengtheningGameResultFilePath));

	spResultDisplay = std::make_shared<ResultDisplay>();
	spResultDisplay->Init();
	spResultDisplay->SetSpWeaponBase(spWeaponBase);
	spResultDisplay->Set2DPos({ 0,200 });
	spResultDisplay->SetDisplayFloorInfo(gameResult.m_displayFloorInfo);
	spResultDisplay->SetPlayerInventoryList(LoadInventoryData(m_inventoryGameResultFilePath));
	spResultDisplay->SetPlayerStatusGameResultFilePath(m_playerStatusGameResultFilePath);
	spResultDisplay->SetPlayerDead(gameResult.m_playerDead);
	spUIManager->AddUIObj(spResultDisplay);



	// 準備画面へ(→準備シーン)
	std::shared_ptr<PrepareButton>spPrepareButton = std::make_shared<PrepareButton>();
	spPrepareButton->SetTexPath("Asset/Textures/Button/PrepareButton/PrepareButton.png");
	spPrepareButton->Init();
	spPrepareButton->SetPos({ 420,-260,0 });
	m_objList.push_back(spPrepareButton);

	std::shared_ptr<TitleButton>spTitleButton = std::make_shared<TitleButton>();
	spTitleButton->Init();
	spTitleButton->SetPos({ -420,-260,0 });
	m_objList.push_back(spTitleButton);

	if (gameResult.m_playerDead)
	{
		if (!m_spPlayerDeadBGM)
		{
			m_spPlayerDeadBGM = KdAudioManager::Instance().Play("Asset/Sound/BGM/Result/PlayerDeadBGM.wav", true);
			if (m_spPlayerDeadBGM)
			{
				m_spPlayerDeadBGM->SetVolume(VOLInfo::Instance().GetMSBGMVol() * 0.3);
			}
		}
	}
	else
	{
		if (!m_spEscapeBGM)
		{
			m_spEscapeBGM = KdAudioManager::Instance().Play("Asset/Sound/BGM/Result/EscapeBGM.wav", true);
			if (m_spEscapeBGM)
			{
				m_spEscapeBGM->SetVolume(VOLInfo::Instance().GetMSBGMVol() * 0.3);
			}
		}
	}
}

//=============================================
// その他の処理
//=============================================

void ResultScene::ApplyBGMVolume()
{
	//倍率は鳴らし始めたときと同じ(0.3)
	if (m_spPlayerDeadBGM)
	{
		m_spPlayerDeadBGM->SetVolume(VOLInfo::Instance().GetMSBGMVol() * 0.3f);
	}
	if (m_spEscapeBGM)
	{
		m_spEscapeBGM->SetVolume(VOLInfo::Instance().GetMSBGMVol() * 0.3f);
	}
}

void ResultScene::Event()
{
	MouseInfo::Instance().SetMouseFreeFlg(true);
	if (GetAsyncKeyState('V') & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::Title
		);
	}
}
