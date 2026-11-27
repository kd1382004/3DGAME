//---- 自分のヘッダー ----
#include "PrepareScene.h"																	//準備シーン

//---- シーン ----
#include "../SceneManager.h"																//シーンの管理

//---- キャンプ ----
#include"../../GameObject/Terrains/Camp/CampManager.h"										//キャンプの管理

//---- UI ----
#include"../../GameObject/UI/UIManager.h"													//UIの管理
#include"../../GameObject/UI/PrepareSceneUI/PrepareSceneDisplay/PrepareSceneDisplay.h"		//準備シーンの画面管理
#include"../../GameObject/UI/PrepareSceneUI/EquipDisplay/EquipDisplay.h"					//準備シーンの装備画面
#include"../../GameObject/UI/PrepareSceneUI/ShopDisplay/ShopDisplay.h"						//準備シーンのショップ画面
#include"../../GameObject/UI/PrepareSceneUI/RecordsDisplay/RecordsDisplay.h"				//準備シーンの記録画面

//---- ボタン ----
#include"../../GameObject/Button/GameButton/GameButton.h"									//ゲームシーンへ進むボタン
#include"../../GameObject/Button/TitleButton/TitleButton.h"									//タイトルシーンへ戻るボタン

//---- エディター ----
#include"../../Editor/ShopEditor/ShopEditor.h"												//ショップの品ぞろえ編集エディター
#include"../../Editor/CharaStatusEditor/CharaStatusEditor.h"								//キャラステータス編集エディター
#include"../../Editor/PrepareLayoutEditor/PrepareLayoutEditor.h"							//準備シーンの配置編集エディター

//---- 便利機能 ----
#include"../../Info/DebugInfo/DebugInfo.h"													//デバッグ用の状態
#include"../../Info/MouseInfo/MouseInfo.h"													//マウス入力
#include"../../Info/KeyInfo/KeyInfo.h"														//キー入力

//=============================================
// コンストラクタ・デストラクタ
//=============================================

PrepareScene::~PrepareScene()
{
	if (m_spPrepareBGM)
	{
		m_spPrepareBGM->Stop();
	}

	// 焚き火の音とエフェクトは CampManager のデストラクタで止まる
}

//=============================================
// 基本の処理
//=============================================

void PrepareScene::ImGUI()
{
	if (ImGui::Begin(U8("PrepareScene")))
	{
		const char* tabName = (m_currentTab == PrepareTab::Shop) ? "Shop" : "Equip";
		ImGui::Text(U8("現在のタブ : %s"), tabName);

		if (ImGui::Button(U8("ショップ"))) { ChangeTab(PrepareTab::Shop); }
		ImGui::SameLine();
		if (ImGui::Button(U8("装備変更"))) { ChangeTab(PrepareTab::Equip); }

		ImGui::Separator();

		if (ImGui::Button(U8("武器をダガーに"))) { m_selectWeapon = PlayerWeaponInfo::PlayerWeaponInfo_Dagger; }
		if (ImGui::Button(U8("武器を剣に"))) { m_selectWeapon = PlayerWeaponInfo::PlayerWeaponInfo_Sword; }

		ImGui::Separator();

		if (ImGui::Button(U8("Gameへ")))
		{
			SceneManager::Instance().SetNextScene(SceneManager::SceneType::Game);
		}

		ImGui::Separator();

		// ショップの値段を決めるEditor
		ImGui::Checkbox(U8("ショップの値段"), &m_shopEditorOpenFlg);

		// キャラの初期ステータスを決めるEditor
		ImGui::Checkbox(U8("キャラの初期ステータス"), &m_charaStatusEditorOpenFlg);

		// 背景のカメラ・ライトを調整するEditor
		ImGui::Checkbox(U8("背景(置物・ライト・カメラ)"), &m_layoutEditorOpenFlg);
	}
	ImGui::End();

	if (m_shopEditorOpenFlg && m_spShopEditor)
	{
		m_spShopEditor->Draw(&m_shopEditorOpenFlg);

		// 保存したら、ショップ画面の値段を読み直す
		if (m_spShopEditor->PopSaved() && m_spShopDisplay)
		{
			m_spShopDisplay->LoadShopData();
		}
	}

	if (m_charaStatusEditorOpenFlg && m_spCharaStatusEditor)
	{
		m_spCharaStatusEditor->Draw(&m_charaStatusEditorOpenFlg);
	}

	if (m_layoutEditorOpenFlg && m_spLayoutEditor)
	{
		m_spLayoutEditor->Draw(&m_layoutEditorOpenFlg);
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

	// ショップの値段を決めるEditor
	m_spShopEditor = std::make_shared<ShopEditor>("Asset/Data/Shop/ShopMasterData.json");

	// キャラの初期ステータスを決めるEditor
	m_spCharaStatusEditor = std::make_shared<CharaStatusEditor>(m_playerStatusFilePath);

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

	// 出発ボタン(右下・「前回の記録」パネルの下)
	m_spGameButton = std::make_shared<GameButton>();
	m_spGameButton->SetTexPath("Asset/Textures/Button/GameButton/DepartButton.png");
	m_spGameButton->Init();
	m_spGameButton->SetPos({ 445.0f, -305.0f, 0.0f });
	m_spGameButton->SetOnPush([this]() { StartDepart(); });
	m_objList.push_back(m_spGameButton);

	ChangeTab(PrepareTab::None);
	OnShopDataChanged();
	/////////////////////////////////////////
	// 音
	/////////////////////////////////////////
	if (!m_spPrepareBGM)
	{
		m_spPrepareBGM = KdAudioManager::Instance().Play("Asset/Sound/BGM/Prepare/PrepareBGM.wav", true);
		if (m_spPrepareBGM)
		{
			m_spPrepareBGM->SetVolume(VOLInfo::Instance().GetMSBGMVol() * kPrepareBGMVolRate);
		}
	}

	/////////////////////////////////////////
	// キャンプ(背景・カメラ・置物・ライト・焚き火)
	/////////////////////////////////////////
	m_spCampManager = std::make_shared<CampManager>();
	m_spCampManager->Init();
	m_objList.push_back(m_spCampManager);	// キャンプの物の更新と描画は CampManager がまとめて行う

	// 背景の配置・カメラ・ライトを調整するEditor
	m_spLayoutEditor = std::make_shared<PrepareLayoutEditor>();
	m_spLayoutEditor->SetTargets(shared_from_this(), m_spCampManager);

	// 武器立ての武器を、持っている物・装備中の物に合わせる
	UpdateRackWeapons();
}

//=============================================
// その他の処理
//=============================================

void PrepareScene::ChangeTab(PrepareTab _tab)
{
	m_currentTab = _tab;

	// 開いているタブの画面だけを表示する
	if (m_spShopDisplay)    m_spShopDisplay->SetActive(_tab == PrepareTab::Shop);
	if (m_spEquipDisplay)   m_spEquipDisplay->SetActive(_tab == PrepareTab::Equip);
	if (m_spRecordsDisplay) m_spRecordsDisplay->SetActive(_tab == PrepareTab::Records);
	if (m_spPrepareSceneDisplay) m_spPrepareSceneDisplay->SetActive(_tab == PrepareTab::None);
	if (m_spPrepareSceneDisplay) m_spPrepareSceneDisplay->SetTabActive(_tab == PrepareTab::Notab);

	// 出発ボタンはメニューだけのときに出す(ショップなどの画面と重なるため)
	if (m_spGameButton) m_spGameButton->SetActive(_tab == PrepareTab::None);


	// タブに合わせてカメラを動かす
	if (m_spCampManager)
	{
		switch (_tab)
		{
		case PrepareTab::Equip:   m_spCampManager->MoveCamera("Equip");   break;
		case PrepareTab::Shop:    m_spCampManager->MoveCamera("Shop");    break;
		case PrepareTab::Records: m_spCampManager->MoveCamera("Records"); break;
		default:                  m_spCampManager->MoveCamera("Explore"); break;
		}
	}

}

void PrepareScene::OnShopDataChanged()
{
	if (!m_spShopDisplay) { return; }

	// メニュー画面の所持金(ファイルではなく、ショップが持っている値を使う)
	if (m_spPrepareSceneDisplay)
	{
		m_spPrepareSceneDisplay->SetMoney(m_spShopDisplay->GetMoney());
		m_spPrepareSceneDisplay->SetPotionNum(m_spShopDisplay->GetPotionTotal());
	}

	// 装備画面の武器の一覧を作り直す
	if (m_spEquipDisplay)
	{
		m_spEquipDisplay->SetOwnedWeapons(m_spShopDisplay->GetOwnedWeapons());
	}
}

void PrepareScene::OnMerchantReaction(MerchantReaction _reaction)
{
	if (!m_spCampManager) { return; }
	m_spCampManager->PlayMerchantReaction(_reaction);
}

void PrepareScene::StartDepart()
{
	if (!m_spCampManager || m_spCampManager->IsDeparting()) { return; }

	// UIを隠す
	ChangeTab(PrepareTab::Notab);
	if (m_spGameButton) { m_spGameButton->SetActive(false); }
	if (m_spPrepareSceneDisplay) { m_spPrepareSceneDisplay->SetActive(false); }

	// プレイヤーを入口へ走らせる
	m_spCampManager->StartDepart();
}

void PrepareScene::ApplyBGMVolume()
{
	//出発の暗転中は、暗くなるのに合わせて小さくしていく
	const float departRate = m_spCampManager ? m_spCampManager->GetDepartVolumeRate() : 1.0f;

	if (m_spPrepareBGM)
	{
		m_spPrepareBGM->SetVolume(VOLInfo::Instance().GetMSBGMVol() * kPrepareBGMVolRate * departRate);
	}

	//たき火の音も効果音の音量に合わせる(鳴らし始めと同じ倍率 1.0)
	if (m_spCampManager) { m_spCampManager->ApplyCampfireVolume(departRate); }
}

void PrepareScene::Event()
{
	//BGM の音量を毎フレーム設定に合わせる
	ApplyBGMVolume();

	MouseInfo::Instance().SetMouseFreeFlg(true);

	// 武器を買った・装備を変えたらすぐ反映されるように毎フレーム更新する
	UpdateRackWeapons();

	// 配置エディタを開いている間はUIを隠す(ImGui を閉じたらエディタも閉じたのと同じ)
	SetUIHiddenForEditor(m_layoutEditorOpenFlg && DebugInfo::Instance().GetSceneManagerImGUIFlg());

	//出発の演出中(真っ暗になったらゲームへ)
	if (m_spCampManager && m_spCampManager->IsDeparting())
	{
		if (m_spCampManager->UpdateDepart())
		{
			SceneManager::Instance().SetNextScene(SceneManager::SceneType::Game);
		}
		return;
	}
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
}

void PrepareScene::ShopEvent()
{

}

void PrepareScene::EquipEvent()
{

}

void PrepareScene::UpdateRackWeapons()
{
	if (!m_spShopDisplay || !m_spCampManager) { return; }

	m_spCampManager->UpdateRackWeapons(m_spShopDisplay->GetOwnedWeapons(), SceneManager::Instance().GetPlayerWeaponInfo());
}

void PrepareScene::SetUIHiddenForEditor(bool _hide)
{
	if (m_uiHiddenForEditorFlg == _hide) { return; }
	m_uiHiddenForEditorFlg = _hide;

	// 隠す物(メンバーで持っているので、一覧から外しても消えない)
	const std::shared_ptr<KdGameObject> uiObjects[] = { m_spUIManager, m_spGameButton };

	if (_hide)
	{
		for (const auto& spUI : uiObjects)
		{
			if (spUI) { m_objList.remove(spUI); }
		}
	}
	else
	{
		// 元の順番(UI → 出発ボタン)で戻す
		for (const auto& spUI : uiObjects)
		{
			if (spUI) { m_objList.push_back(spUI); }
		}

		// 選んでいる印を消す
		if (m_spLayoutEditor) { m_spLayoutEditor->OnClose(); }
	}
}

bool PrepareScene::CanOpenPause()
{
	//共通の判定(インベントリ中・暗転中)
	if (!BaseScene::CanOpenPause()) { return false; }

	//出発の演出中(走っていく〜暗転)は開かない
	if (m_spCampManager && m_spCampManager->IsDeparting()) { return false; }

	return true;
}

void PrepareScene::ApplyEquip()
{
	SceneManager::Instance().SetPlayerWeaponInfo(m_selectWeapon);
}
