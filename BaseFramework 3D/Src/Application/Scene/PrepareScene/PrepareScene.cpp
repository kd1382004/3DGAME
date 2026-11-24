//---- 自分のヘッダー ----
#include "PrepareScene.h"																	//準備シーン

//---- シーン ----
#include "../SceneManager.h"																//シーンの管理

//---- カメラ ----
#include"../../GameObject/Camera/PrepareCamera/PrepareCamera.h"								//準備シーン用カメラ

//---- キャンプ ----
#include"../../GameObject/Terrains/Camp/CampLayoutData/CampLayoutData.h"					//キャンプの配置データ
#include"../../GameObject/Terrains/Camp/CampLight/CampLight.h"								//キャンプのライト
#include"../../GameObject/Terrains/Camp/CampObject/CampObject.h"							//キャンプの置物

//---- マップオブジェクト ----
#include"../../GameObject/Terrains/MapObj/Torch/Torch.h"									//松明

//---- UI ----
#include"../../GameObject/UI/UIManager.h"													//UIの管理
#include"../../GameObject/UI/PrepareSceneUI/PrepareSceneDisplay/PrepareSceneDisplay.h"		//準備シーンの画面管理
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
#include"../../Info/FadeDraw/FadeDraw.h"													//画面フェードの描画
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

	for (const auto& wpEf : m_wpKdEffekseerObject)
	{
		auto spEf = wpEf.lock();
		if (spEf)
		{
			spEf->SetLoop(false);
			spEf->StopEffect();
		}

	}

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
		ImGui::Checkbox(U8("背景とカメラ"), &m_layoutEditorOpenFlg);
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
	// TODO : 準備画面用のBGMを用意したら差し替える
	//if (!m_spPrepareBGM)
	//{
	//	m_spPrepareBGM = KdAudioManager::Instance().Play("Asset/Sound/BGM/Prepare/PrepareBGM.wav", true);
	//	if (m_spPrepareBGM)
	//	{
	//		m_spPrepareBGM->SetVolume(VOLInfo::Instance().GetMSBGMVol() * 0.5);
	//	}
	//}







	// 背景の配置を読む
	m_campLayout = LoadCampLayout(m_campLayoutFilePath);
	ApplyCampLayoutTune(m_campLayout, m_campLayoutTuneFilePath);

	// 霧と環境光を準備画面用にする
	KdShaderManager::Instance().WorkAmbientController().SetFogEnable(true, false);
	KdShaderManager::Instance().WorkAmbientController().SetDistanceFog(m_campFogColor, m_campFogDensity);
	KdShaderManager::Instance().WorkAmbientController().SetAmbientLight(m_campAmbient);

	// カメラ(タブごとに動く)
	m_spPrepareCamera = std::make_shared<PrepareCamera>();
	m_spPrepareCamera->Init();
	m_spPrepareCamera->SetCameraData(m_campLayout.m_cameras);
	m_spPrepareCamera->MoveTo("Explore", true);	// 最初から入口を映す
	m_objList.push_back(m_spPrepareCamera);
	KdEffekseerManager::GetInstance().SetCamera(m_spPrepareCamera->GetCamera());

	// 置物
	for (const auto& data : m_campLayout.m_objects)
	{
		if (data.m_kind == "Torch")
		{

			continue;
		}

		if (data.m_kind == "Campfire")
		{
			// 炎(焚き火の少し上)。ループ再生
			auto ef = KdEffekseerManager::GetInstance().Play("Campfire/Campfire.efkefc",
				data.m_pos + Math::Vector3(0, 0, 0), 1.0f, 1.0f, true);

			m_wpKdEffekseerObject.push_back(ef);
		}

		auto spObj = std::make_shared<CampObject>();
		spObj->Init();
		spObj->SetData(data);
		m_objList.push_back(spObj);
		m_campObjects.push_back(spObj);	// エディタ用に覚えておく


		if (data.m_kind == "Player")
		{
			m_wpCampPlayer = spObj;
			spObj->PlayAnime("Idle", true);
		}
	}


	for (const auto& data : m_campLayout.m_lights)
	{
		if (data.m_name.rfind("Light_Torch", 0) == 0) { continue; }	// 松明は自分で光る
		auto spLight = std::make_shared<CampLight>();
		spLight->SetData(data);
		m_objList.push_back(spLight);
		m_campLights.push_back(spLight);	// エディタ用
	}

	// 入口の奥のエフェクト(ダンジョンの階段と同じもの)。ループ再生
	// 位置と大きさはエディタの「出発」タブで合わせて CampLayoutTune.json に保存する
	m_wpEntranceEffect = KdEffekseerManager::GetInstance().Play("Stairs/Stairs.efkefc",
		m_campLayout.m_depart.m_effectPos, m_campLayout.m_depart.m_effectScale, 1.0f, true);
	m_wpKdEffekseerObject.push_back(m_wpEntranceEffect);	// シーンを出るときに止める

	// 背景のカメラ・ライトを調整するEditor
	m_spLayoutEditor = std::make_shared<PrepareLayoutEditor>(m_campLayoutTuneFilePath);
	m_spLayoutEditor->SetTargets(shared_from_this(), m_spPrepareCamera, m_campLights, m_campObjects);
	m_spLayoutEditor->SetDepartData(&m_campLayout.m_depart);
	m_spLayoutEditor->SetEntranceEffect(m_wpEntranceEffect);
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

	// 出発ボタンはメニューだけのときに出す(ショップなどの画面と重なるため)
	if (m_spGameButton) m_spGameButton->SetActive(_tab == PrepareTab::None);


	// タブに合わせてカメラを動かす
	if (m_spPrepareCamera)
	{
		switch (_tab)
		{
		case PrepareTab::Equip:   m_spPrepareCamera->MoveTo("Equip");   break;
		case PrepareTab::Shop:    m_spPrepareCamera->MoveTo("Shop");    break;
		case PrepareTab::Records: m_spPrepareCamera->MoveTo("Records"); break;
		default:                  m_spPrepareCamera->MoveTo("Explore"); break;
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

void PrepareScene::StartDepart()
{
	if (m_departState != DepartState::None) { return; }
	m_departState = DepartState::Walk;
	m_departTimer = 0;
	m_departFadeTimer = 0;
	ChangeTab(PrepareTab::None);
	if (m_spGameButton) { m_spGameButton->SetActive(false); }
	if (m_spPrepareSceneDisplay) { m_spPrepareSceneDisplay->SetActive(false); }


	auto player = m_wpCampPlayer.lock();
	if (player)
	{
		player->PlayAnime("Run", true);
		m_departStartPos = player->WorkData().m_pos;
		const CampDepartData& dep = m_campLayout.m_depart;
		Math::Vector3 dir = dep.m_target - m_departStartPos;


		dir.y = 0.0f;
		if (dir.LengthSquared() > 0.0001f)
		{
			float yaw = DirectX::XMConvertToDegrees(atan2(dir.x, dir.z));
			player->WorkData().m_rotDeg.y = yaw;
			player->ApplyData();
		}
	}

}

void PrepareScene::UpdateDepart()
{
	const CampDepartData& dep = m_campLayout.m_depart;
	auto player = m_wpCampPlayer.lock();

	//---- 歩く(Walk でも Fade でも) ----
	m_departTimer += DeltaTime::Instance().GetGameDeltaTime();

	float t = 1.0f;
	if (dep.m_walkTime > 0.0f) { t = m_departTimer / dep.m_walkTime; }
	if (t > 1.0f) { t = 1.0f; }

	if (player)
	{
		player->WorkData().m_pos = Math::Vector3::Lerp(m_departStartPos, dep.m_target, t);
		player->ApplyData();
	}

	//---- 状態ごと ----
	switch (m_departState)
	{
	case DepartState::Walk:
		if (t >= dep.m_fadeStartRate)
		{
			FadeDraw::Instance().SetFadeType(FadeIn, dep.m_fadeTime);
			m_departState = DepartState::Fade;

			// FadeDraw はこのフレームから時間を進めるので、こちらも同じフレームから数える
			m_departFadeTimer = DeltaTime::Instance().GetRealDeltaTime();
		}
		break;

	case DepartState::Fade:
		// FadeDraw と同じ Real の時間で数えて、真っ暗になったらゲームへ
		m_departFadeTimer += DeltaTime::Instance().GetRealDeltaTime();
		if (m_departFadeTimer >= dep.m_fadeTime)
		{
			SceneManager::Instance().SetNextScene(SceneManager::SceneType::Game);
		}
		break;

	default:
		break;
	}
}

void PrepareScene::Event()
{
	MouseInfo::Instance().SetMouseFreeFlg(true);
	if (m_departState != DepartState::None) { UpdateDepart(); return; }
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

void PrepareScene::ApplyEquip()
{
	SceneManager::Instance().SetPlayerWeaponInfo(m_selectWeapon);
}
