#include "PrepareScene.h"
#include "../SceneManager.h"

#include"../../Info/DebugInfo/DebugInfo.h"
#include"../../Info/MouseInfo/MouseInfo.h"
#include"../../Info/KeyInfo/KeyInfo.h"

#include"../../GameObject/UI/UIManager.h"
#include"../../GameObject/UI/PrepareSceneUI/PrepareSceneDisplay/PrepareSceneDisplay.h"

#include"../../GameObject/Button/GameButton/GameButton.h"
#include"../../GameObject/Button/TitleButton/TitleButton.h"

#include"../../GameObject/UI/PrepareSceneUI/PrepareSceneDisplay/PrepareSceneDisplay.h"
#include"../../GameObject/UI/PrepareSceneUI/EquipDisplay/EquipDisplay.h"
#include"../../GameObject/UI/PrepareSceneUI/ShopDisplay/ShopDisplay.h"
#include"../../GameObject/UI/PrepareSceneUI/RecordsDisplay/RecordsDisplay.h"


PrepareScene::~PrepareScene()
{
	if (m_spPrepareBGM)
	{
		m_spPrepareBGM->Stop();
	}
}

void PrepareScene::Init()
{
	DebugInfo::Instance().SetSceneManagerImGUIFlg(false);

	// 現在の装備を初期選択にする
	m_selectWeapon = SceneManager::Instance().GetPlayerWeaponInfo();

	/////////////////////////////////////////
	// UI
	/////////////////////////////////////////
	m_spUIManager = std::make_shared<UIManager>();
	m_spUIManager->Init();
	m_objList.push_back(m_spUIManager);

	// プレイ記録の読み込み(メニューとPlay履歴で使う)
	const GameRecord gameRecord = LoadGameRecord();
	
	m_spPrepareSceneDisplay = std::make_shared<PrepareSceneDisplay>();
	m_spPrepareSceneDisplay->SetPrepareScene(shared_from_this());
	m_spPrepareSceneDisplay->SetGameRecord(gameRecord);
	m_spPrepareSceneDisplay->Init();
	m_spUIManager->AddUIObj(m_spPrepareSceneDisplay);


	// ショップ
	m_spShopDisplay = std::make_shared<ShopDisplay>();
	m_spShopDisplay->SetPrepareScene(shared_from_this());
	m_spShopDisplay->Init();
	m_spUIManager->AddUIObj(m_spShopDisplay);

	// 装備変更
	m_spEquipDisplay = std::make_shared<EquipDisplay>();
	m_spEquipDisplay->SetPrepareScene(shared_from_this());
	m_spEquipDisplay->Init();
	m_spUIManager->AddUIObj(m_spEquipDisplay);

	// Play履歴
	m_spRecordsDisplay = std::make_shared<RecordsDisplay>();
	m_spRecordsDisplay->SetPrepareScene(shared_from_this());
	m_spRecordsDisplay->SetGameRecord(gameRecord);
	m_spRecordsDisplay->Init();
	m_spUIManager->AddUIObj(m_spRecordsDisplay);

	ChangeTab(PrepareTab::None);

	/////////////////////////////////////////
	// 音
	/////////////////////////////////////////
	// TODO : 準備画面用のBGMを用意したら差し替える
	//if (!m_spPrepareBGM)
	//{
	//	m_spPrepareBGM = KdAudioManager::Instance().Play("Asset/Sound/BGM/Prepare/PrepareBGM.wav", true);
	//	if (m_spPrepareBGM)
	//	{
	//		m_spPrepareBGM->SetVolume(VOLInfo::Instance().GetMSBGMVol() * 0.5);
	//	}
	//}
}

void PrepareScene::Event()
{
	MouseInfo::Instance().SetMouseFreeFlg(true);

	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		DebugInfo::Instance().SetSceneManagerImGUIFlg(true);
	}

	// TABでタブ切り替え
	if (KeyInfo::Instance().GetValidKeyPush(VK_TAB, true, true))
	{
		int next = (static_cast<int>(m_currentTab) + 1) % static_cast<int>(PrepareTab::TabSize);
		ChangeTab(static_cast<PrepareTab>(next));
	}

	switch (m_currentTab)
	{
	case PrepareTab::Shop:
		ShopEvent();
		break;
	case PrepareTab::Equip:
		EquipEvent();
		break;
	default:
		break;
	}

	// 選択中の装備は毎フレームSceneManagerへ反映しておく
	// (どのボタンでGameへ移っても装備が持ち越されるように)
	ApplyEquip();
}

void PrepareScene::ChangeTab(PrepareTab _tab)
{
	m_currentTab = _tab;

	// 開いているタブの画面だけを表示する
	if (m_spShopDisplay)    m_spShopDisplay->SetActive(_tab == PrepareTab::Shop);
	if (m_spEquipDisplay)   m_spEquipDisplay->SetActive(_tab == PrepareTab::Equip);
	if (m_spRecordsDisplay) m_spRecordsDisplay->SetActive(_tab == PrepareTab::Records);
	if (m_spPrepareSceneDisplay) m_spPrepareSceneDisplay->SetActive(_tab == PrepareTab::None);

}

void PrepareScene::ShopEvent()
{

}

void PrepareScene::EquipEvent()
{
	
}

void PrepareScene::ApplyEquip()
{
	SceneManager::Instance().SetPlayerWeaponInfo(m_selectWeapon);
}

void PrepareScene::ImGUi()
{
	if (ImGui::Begin(U8("PrepareScene")))
	{
		const char* tabName = (m_currentTab == PrepareTab::Shop) ? "Shop" : "Equip";
		ImGui::Text(U8("現在のタブ : %s"), tabName);

		if (ImGui::Button(U8("ショップ")))	{ ChangeTab(PrepareTab::Shop); }
		ImGui::SameLine();
		if (ImGui::Button(U8("装備変更")))	{ ChangeTab(PrepareTab::Equip); }

		ImGui::Separator();

		if (ImGui::Button(U8("武器をダガーに"))) { m_selectWeapon = PlayerWeaponInfo::PlayerWeaponInfo_Dagger; }
		if (ImGui::Button(U8("武器を剣に")))	 { m_selectWeapon = PlayerWeaponInfo::PlayerWeaponInfo_Sword; }

		ImGui::Separator();

		if (ImGui::Button(U8("Gameへ")))
		{
			ApplyEquip();
			SceneManager::Instance().SetNextScene(SceneManager::SceneType::Game);
		}
	}
	ImGui::End();
}
