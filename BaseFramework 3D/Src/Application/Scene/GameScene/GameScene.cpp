//---- 自分のヘッダー ----
#include "GameScene.h"																								//ゲームシーン

//---- カメラ ----
#include"../../GameObject/Camera/TPSCamera/TPSCamera.h"																//三人称カメラ

//---- プレイヤー ----
#include"../../GameObject/Character/Player/PlayerBase.h"															//プレイヤー
#include"../../GameObject/Character/Player/PlayerBuffManager/PlayerBuffManager.h"									//プレイヤーのバフ管理
#include"../../GameObject/Character/Player/PlayerInventory/PlayerInventory.h"										//プレイヤーの所持品

//---- 敵 ----
#include"../../GameObject/Character/Enemy/EnemyManager.h"															//敵の管理
#include"../../GameObject/Character/Enemy/EnemyBase.h"																//敵の基底クラス

//---- 武器 ----
#include"../../GameObject/Weapon/Dagger/Dagger.h"																	//短剣
#include"../../GameObject/Weapon/Sword/Sword.h"																		//剣
#include"../../GameObject/Weapon/WeaponStrengtheningEditor/WeaponStrengtheningEditor.h"								//武器強化データ編集エディター
#include"../../GameObject/Weapon/ComboStep/ComboStepEditor.h"														//コンボ編集エディター

//---- アクセサリー ----
#include"../../GameObject/Accessory/AccessoryManager.h"																//アクセサリーの管理

//---- ポーション ----
#include"../../GameObject/Potions/PotionUseController.h"															//ポーションの使用処理

//---- マップ ----
#include"../../GameObject/Terrains/Map/MapBase.h"																	//マップ部品の基底クラス
#include"../../GameObject/Terrains/Map/MapManager.h"																//マップの管理・経路探索

//---- マップオブジェクト ----
#include"../../GameObject/Terrains/MapObj/MapObjBase.h"																//マップオブジェクトの基底クラス
#include"../../GameObject/Terrains/MapObj/MapObjManager.h"															//マップオブジェクトの管理
#include"../../GameObject/Terrains/MapObj/TreasureChest/TreasureChestTableEditor/TreasureChestTableEditor.h"		//宝箱のドロップテーブル編集エディター

//---- ワープゲート ----
#include"../../GameObject/WarpGate/WarpGateManager.h"																//ワープゲートの管理

//---- エフェクト ----
#include"../../GameObject/Effect/EffectManager.h"																	//エフェクトの管理

//---- UI ----
#include"../../GameObject/UI/UIManager.h"																			//UIの管理
#include"../../GameObject/UI/BuffUI/BuffUI.h"																		//バフの表示
#include"../../GameObject/UI/HitDamage/HitDamage.h"																	//ダメージ数字の表示
#include"../../GameObject/UI/UI2/UIBase2.h"																			//キー表示付きUIの基底クラス
#include"../../GameObject/UI/ItemBar/ItemBar.h"																		//アイテムバー
#include"../../GameObject/UI/FloorTitleUI/FloorTitleUI.h"

//---- エディター ----
#include"../../Editor/CharaStatusEditor/CharaStatusEditor.h"														//キャラステータス編集エディター
#include"../../Editor/EnemyAttackPatternEditor/EnemyAttackPatternEditor.h"											//敵の攻撃パターン編集エディター

//---- 便利機能 ----
#include"../../Info/MouseInfo/MouseInfo.h"																			//マウス入力
#include"../../Info/FadeDraw/FadeDraw.h"																			//画面フェードの描画
#include"../../../Application/Info/DebugInfo/DebugInfo.h"															//デバッグ用の状態
#include"../../Info/SE/SEPlayer.h"																			//効果音

//---- 標準ライブラリ ----
#include <ctime>																									//時刻

//=============================================
// コンストラクタ・デストラクタ
//=============================================

GameScene::~GameScene()
{
	if (m_spBossBGM)
	{
		m_spBossBGM->Stop();
	}

	if (m_spGameBGM)
	{
		m_spGameBGM->Stop();
	}

	if (m_spFactoryAmbientSE)
	{
		m_spFactoryAmbientSE->Stop();
	}
}

//=============================================
// 基本の処理
//=============================================

void GameScene::ImGUI()
{
	for (auto camera : m_spCharacterStatus)
	{
		camera->ImGUI();
	}

	//ImGUI
	if (ImGui::TreeNode(U8("階数")))
	{
		if (ImGui::Button(U8("次の階")))
		{
			GenerateMap();
		}
		ImGui::TreePop();
	}


	//武器のMasterデータ
	// ImGui の更新処理の中で毎フレーム呼ぶ
	if (ImGui::TreeNode(U8("エディター")))
	{
		for (EditorEntry& editor : m_editors)
		{
			ImGui::Checkbox(editor.m_name, &editor.m_open);
		}
		ImGui::TreePop();
	}

	//開いているエディターだけ描画（×で閉じると open が false になる）
	for (EditorEntry& editor : m_editors)
	{
		if (editor.m_open && editor.m_draw)
		{
			ImGui::SetNextWindowBgAlpha(1.0f);
			editor.m_draw(&editor.m_open);
		}
	}

	//敵の攻撃パターンを保存したら、敵がすぐ読み直す
	if (m_spEnemyAttackPatternEditor && m_spEnemyAttackPatternEditor->PopSaved())
	{
		ReloadEnemyAttackPatterns();
	}
}

void GameScene::Init()
{
	DebugInfo::Instance().SetSceneManagerImGUIFlg(false);
	MouseInfo::Instance().SetMouseFreeFlg(false);

	m_displayFloorInfo = DisplayFloorInfo{};



	/////////////////////////////////////////
	//ゲームシーン
	/////////////////////////////////////////
	auto self = shared_from_this();
	m_displayFloor = 0;

	/////////////////////////////////////////
	//カメラ
	/////////////////////////////////////////
	m_spCamera = std::make_shared<TPSCamera>();
	m_spCamera->Init();
	m_objList.push_back(m_spCamera);


	/////////////////////////////////////////
	//プレイヤー
	/////////////////////////////////////////
	m_spPlayer = std::make_shared<PlayerBase>();
	m_spPlayer->Init();
	m_spPlayer->SetPlayerWeaponInfo(SceneManager::Instance().GetPlayerWeaponInfo());
	m_spPlayer->LoadCharaStatus(m_playerStatusFilePath);
	m_objList.push_back(m_spPlayer);


	std::vector<std::vector<Inventory>> inventoryData = LoadInventoryData(m_inventoryFilePath);;
	m_spPlayer->SetPotionsInventoryList(inventoryData[0]);
	m_spPlayer->SetWeaponsStrengtheningInventoryList(inventoryData[1]);
	m_spPlayer->GetPlayerInventory()->SetMoney(LoadMoney());	// 所持金

	//出発したときの持ち込みを覚えておく(冒険の記録で使う)
	m_carryIn.m_weapon = SceneManager::Instance().GetPlayerWeaponInfo();
	m_carryIn.m_potionNum = 0;
	for (const auto& potion : inventoryData[0])
	{
		m_carryIn.m_potionNum += potion.m_num;
	}
	m_carryIn.m_money = m_spPlayer->GetPlayerInventory()->GetMoney();
	m_playDate = static_cast<long long>(std::time(nullptr));

	m_spCharacterStatus.push_back(m_spPlayer);

	/////////////////////////////////////////
	//武器
	/////////////////////////////////////////

	switch (SceneManager::Instance().GetPlayerWeaponInfo())
	{
	case PlayerWeaponInfo_Dagger:
		m_spWeapon = std::make_shared<Dagger>();
		break;
	case PlayerWeaponInfo_Sword:
		m_spWeapon = std::make_shared<Sword>();
		break;
	case PlayerWeaponInfoSize:
		break;
	default:
		break;
	}


	m_spWeapon->Init();
	// マスタを土台にセーブの進行状況を重ねる
	m_spWeapon->SetWeaponsStrengtheningInfo(LoadMergedWeaponStrengtheningData(m_weaponStrengtheningFilePath));


	m_objList.push_back(m_spWeapon);

	/////////////////////////////////////////
	//マップ
	/////////////////////////////////////////
	m_spMapManager = std::make_shared<MapManager>();
	m_spMapManager->Init();
	m_objList.push_back(m_spMapManager);


	/////////////////////////////////////////
	//敵
	/////////////////////////////////////////
	m_spEnemyManager = std::make_shared<EnemyManager>();
	m_spEnemyManager->Init();
	m_objList.push_back(m_spEnemyManager);






	/////////////////////////////////////////
	//マップオブジェクト
	/////////////////////////////////////////
	m_spMapObjManager = std::make_shared<MapObjManager>();
	m_spMapObjManager->Init();
	m_objList.push_back(m_spMapObjManager);


	/////////////////////////////////////////
	//ワープゲートマネージャー
	/////////////////////////////////////////	
	m_spWarpGateManager = std::make_shared<WarpGateManager>();
	m_spWarpGateManager->Init();
	m_objList.push_back(m_spWarpGateManager);


	/////////////////////////////////////////
	//エフェクトマネージャー
	/////////////////////////////////////////	
	std::shared_ptr<EffectManager>spEffect = std::make_shared<EffectManager>();
	spEffect->Init();
	m_objList.push_back(spEffect);


	/////////////////////////////////////////
	//ポーション使用コントローラー
	/////////////////////////////////////////	
	m_spPotionUseController = std::make_shared<PotionUseController>();


	/////////////////////////////////////////
	//UI
	/////////////////////////////////////////	
	m_spUIManager = std::make_shared<UIManager>();
	m_spUIManager->Init();
	m_spUIManager->SetPotionTexInfo();
	m_spUIManager->AddPlayerInventoryUI();
	m_spUIManager->AddUIMapManager();
	m_objList.push_back(m_spUIManager);



	/////////////////////////////////////////
	//アクセサリー
	/////////////////////////////////////////	
	std::shared_ptr<AccessoryManager> spAccessoryManager = std::make_shared<AccessoryManager>();
	spAccessoryManager->Init();
	m_objList.push_back(spAccessoryManager);

	/////////////////////////////////////////
	//UIにセット
	/////////////////////////////////////////
	m_spUIManager->SetPlayer(m_spPlayer);
	m_spUIManager->SetGameScene(self);
	m_spUIManager->SetPotionUseController(m_spPotionUseController);

	std::shared_ptr<BuffUI>spBuffUI = std::make_shared<BuffUI>();
	spBuffUI->Init();
	spBuffUI->Set2DPos({ -180,-320 });
	spBuffUI->SetPotionTexInfo(m_spUIManager->GetPotionTexInfo());
	spBuffUI->SetPlayer(m_spPlayer);
	m_spUIManager->AddUIObj(spBuffUI);

	//アイテムバー(ホイールで選んでFで使う)
	std::shared_ptr<ItemBar> spItemBar = std::make_shared<ItemBar>();
	spItemBar->Init();
	spItemBar->SetPlayer(m_spPlayer);
	spItemBar->SetPotionUseController(m_spPotionUseController);
	spItemBar->SetPotionTexInfo(m_spUIManager->GetPotionTexInfo());
	spItemBar->SetPlayerInventoryUI(m_spUIManager->GetPlayerInventoryUI());
	m_spUIManager->AddUIObj(spItemBar);

	std::shared_ptr<HitDamage>spHitDamage = std::make_shared<HitDamage>();
	spHitDamage->Init();
	spHitDamage->SetCamera(m_spCamera);
	m_spUIManager->AddUIObj(spHitDamage);

	std::shared_ptr<UIBase2>spUIBase2 = std::make_shared<UIBase2>();
	std::shared_ptr<KdTexture>spTex = std::make_shared<KdTexture>();
	spTex->Load("Asset/Textures/GameUI/Item/PlayerInventory/inventoryType/Potion.png");
	spUIBase2->SetTex(spTex);
	spUIBase2->Set2DPos({ 580, 320 });
	spUIBase2->SetDrawKey(VK_TAB);
	spUIBase2->SetDrawKeyPos(DrawKeyPos_Bot);
	spUIBase2->SetDrawFlg(true);
	m_spUIManager->AddUIObj(spUIBase2);

	//階層移動のときの「◯階層」表示
	m_spFloorTitleUI = std::make_shared<FloorTitleUI>();
	m_spFloorTitleUI->Init();
	m_spUIManager->AddUIObj(m_spFloorTitleUI);


	/////////////////////////////////////////
	//プレイヤーにセット
	/////////////////////////////////////////
	m_spPlayer->SetCamera(m_spCamera);
	m_spPlayer->SetGameScene(self);
	m_spPlayer->SetWeapon(m_spWeapon);
	m_spPlayer->AddUIList(m_spUIManager);
	m_spPlayer->SetEffectManager(spEffect);
	m_spPlayer->SetHitDamage(spHitDamage);
	m_spPlayer->SetAccessoryManager(spAccessoryManager);
	m_spPlayer->GetPlayerBuffManager()->SetPlayer(m_spPlayer);

	/////////////////////////////////////////
	//敵にセット
	/////////////////////////////////////////
	m_spEnemyManager->SetPlayer(m_spPlayer);
	m_spEnemyManager->SetMapManager(m_spMapManager);
	m_spEnemyManager->SetCamera(m_spCamera);
	m_spEnemyManager->AddUIList(m_spUIManager);
	m_spEnemyManager->SetGameScene(self);
	m_spEnemyManager->SetHitDamage(spHitDamage);
	m_spEnemyManager->SetEnemyManager(m_spEnemyManager);

	/////////////////////////////////////////
	//カメラにセット
	/////////////////////////////////////////
	m_spCamera->SetTarget(m_spPlayer);

	/////////////////////////////////////////
	//マップにセット
	/////////////////////////////////////////
	m_spMapManager->SetCamera(m_spCamera);
	m_spMapManager->SetPlayer(m_spPlayer);
	m_spMapManager->SetEnemyManager(m_spEnemyManager);
	m_spMapManager->SetUIManager(m_spUIManager);
	m_spMapManager->SetMapObjManager(m_spMapObjManager);
	m_mapType = MapType::MapType_Dungeon;


	/////////////////////////////////////////
	//ポーション使用コントローラーにセット
	/////////////////////////////////////////	
	m_spPotionUseController->SetPlayer(m_spPlayer);

	/////////////////////////////////////////
	//ワープゲートマネージャーにセット
	/////////////////////////////////////////	
	m_spWarpGateManager->SetPlayer(m_spPlayer);
	m_spWarpGateManager->SetGameScene(self);

	/////////////////////////////////////////
	//マップオブジェクトにセット
	/////////////////////////////////////////
	m_spMapObjManager->SetGameScene(self);


	/////////////////////////////////////////
	//武器にセット
	/////////////////////////////////////////	
	m_spWeapon->SetParentChara(m_spPlayer);


	if (m_spCamera)
	{
		KdEffekseerManager::GetInstance().SetCamera(m_spCamera->GetCamera());
	}


	//マップの成長率を設定
	m_mapLinearGrowthPerFloor = 5;
	m_mapLinearGrowthPerFloorX = 5 * KdRandom::GetFloat(0.8f, 1.2f);
	m_mapLinearGrowthPerFloorY = 5 * KdRandom::GetFloat(0.8f, 1.2f);


	//音

	if (!m_spGameBGM)
	{
		m_spGameBGM = KdAudioManager::Instance().Play("Asset/Sound/BGM/Game/GameBGM.wav", true);
		m_spGameBGM->SetVolume(VOLInfo::Instance().GetMSBGMVol());
		m_spGameBGM->Stop();
	}

	if (!m_spBossBGM)
	{
		m_spBossBGM = KdAudioManager::Instance().Play("Asset/Sound/BGM/Game/BossBGM.wav", true);
		m_spBossBGM->SetVolume(VOLInfo::Instance().GetMSBGMVol());
		m_spBossBGM->Stop();
	}


	//最初の階を作って、いきなり「1階層」の表示から始める
	//(準備画面の出発で暗転しているので、もう一度暗くする必要はない)
	GenerateMap();
	if (m_spFloorTitleUI)
	{
		m_spFloorTitleUI->Show(m_displayFloor, static_cast<int>(m_mapType));
	}
	m_floorChangeState = FloorChangeState::ShowFloor;




	//ImGUI用
	m_spWSEditor = std::make_shared<WeaponStrengtheningEditor>(m_weaponStrengtheningMSDateFilePath);
	m_spTCTableEditor = std::make_shared<TreasureChestTableEditor>();
	m_spComboStepEditor = std::make_shared<ComboStepEditor>();
	m_spCharaStatusEditor = std::make_shared<CharaStatusEditor>(m_playerStatusFilePath);
	m_spEnemyAttackPatternEditor = std::make_shared<EnemyAttackPatternEditor>();

	// コンボのプレビュー用モデルをシーンに置く（専用ウィンドウに描く。描いた後はゲームのカメラに戻す）
	m_spComboStepEditor->SetPreviewMainCamera(m_spCamera);
	m_objList.push_back(m_spComboStepEditor->GetPreviewObject());

	// エディターで保存したら、プレイヤーが使っているファイルならすぐ読み直す
	m_spComboStepEditor->SetOnSaved([this](const std::string& _path)
		{
			//敵のコンボファイルかもしれないので、敵の攻撃パターンも読み直す
			ReloadEnemyAttackPatterns();

			if (!m_spPlayer) { return; }

			const std::string saved = std::filesystem::path(_path).lexically_normal().generic_string();
			const std::string using_ = std::filesystem::path(m_spPlayer->GetComboFilePath()).lexically_normal().generic_string();
			if (saved == using_)
			{
				m_spPlayer->ReloadCombo();
			}
		});

	// エディターを一覧に登録する
	m_editors.clear();
	m_editors.push_back({ U8("武器強化"), false, [this](bool* _open) { m_spWSEditor->Draw(_open); } });
	m_editors.push_back({ U8("宝箱のドロップテーブル"), false, [this](bool* _open) { m_spTCTableEditor->Draw(_open); } });
	m_editors.push_back({ U8("武器のコンボ"), false, [this](bool* _open) { m_spComboStepEditor->Draw(_open); } });
	m_editors.push_back({ U8("キャラの初期ステータス"), false, [this](bool* _open) { m_spCharaStatusEditor->Draw(_open); } });
	m_editors.push_back({ U8("敵の攻撃パターン"), false, [this](bool* _open) { m_spEnemyAttackPatternEditor->Draw(_open); } });
}

//=============================================
// 設定
//=============================================

void GameScene::SetGameResult()
{
	GameResult gameResult;
	gameResult.m_displayFloorInfo = m_displayFloorInfo;
	gameResult.m_carryIn = m_carryIn;
	gameResult.m_playDate = m_playDate;


	if (m_spPlayer)
	{
		gameResult.m_playerDead = m_spPlayer->GetIsDead();

		//手に入れたお金 = 今の所持金 - 出発したときの所持金
		gameResult.m_getMoney = std::max(0L, m_spPlayer->GetPlayerInventory()->GetMoney() - m_carryIn.m_money);


		bool saveFlg = !gameResult.m_playerDead;

		//セーブデータに保存
		SaveInventoryData(saveFlg, m_spPlayer->GetPotionsInventoryList(), m_spPlayer->GetWeaponsStrengtheningInventoryList(), m_inventoryFilePath, m_spPlayer->GetPlayerInventory()->GetMoney());
		SaveWeaponStrengtheningData(saveFlg, m_weaponStrengtheningFilePath, m_spWeapon->GetWeaponsStrengtheningInfoList());


		//ゲームResultに保存
		SaveInventoryData(true, m_spPlayer->GetPotionsInventoryList(), m_spPlayer->GetWeaponsStrengtheningInventoryList(), m_inventoryGameResultFilePath, m_spPlayer->GetPlayerInventory()->GetMoney());
		SaveWeaponStrengtheningData(true, m_weaponStrengtheningGameResultFilePath, m_spWeapon->GetWeaponsStrengtheningInfoList());
		//プレイヤーのステータス状況を保存
		m_spPlayer->ClearAllBuffs();
		m_spPlayer->SaveCharaStatus(m_playerStatusGameResultFilePath);
		if (saveFlg)
		{
			//セーブデータに保存
			m_spPlayer->SaveCharaStatus(m_playerStatusFilePath);
			m_spPlayer->SaveCharaStatus(m_playerStatusFilePath);
		}
		else
		{
			//セーブデータに保存
			//プレイヤーのステータス状況をリセット
			m_spPlayer->LoadCharaStatus(m_spPlayer->GetCharaDefaultStatusFilePath());
			m_spPlayer->SaveCharaStatus(m_playerStatusFilePath);
		}

	}

	SceneManager::Instance().SetGameResult(gameResult);
}

void GameScene::AddEnemyKill()
{
	m_displayFloorInfo.m_enemyKillCount++;
}

void GameScene::AddTreasureChestsNum()
{
	m_displayFloorInfo.m_treasureChestsNum++;
}

//=============================================
// その他の処理
//=============================================

//マップ生成
void GameScene::GenerateMap()
{
	if (!m_spMapManager) { return; }
	if (!m_spPlayer) { return; }
	if (!m_spEnemyManager) { return; }
	if (!m_spMapObjManager) { return; }
	if (!m_spCamera) { return; }
	if (!m_spUIManager) { return; }

	//全effect停止
	KdEffekseerManager::GetInstance().StopAllEffect();

	int baseSize = 30;              // 1階のマップサイズ
	float growth = 1.01f;



	m_mapGenerateDisplayFloor = m_displayFloor;

	if (m_displayFloor > m_mapGenerateDisplayFloorMax)
	{
		m_mapGenerateDisplayFloor = m_mapGenerateDisplayFloorMax;
	}

	m_displayFloor++;
	m_displayFloorInfo.m_displayFloor = m_displayFloor;

	int mapSizeX = static_cast<int>((baseSize + m_mapGenerateDisplayFloor * m_mapLinearGrowthPerFloorX) * std::pow(growth, m_mapGenerateDisplayFloor));
	int mapSizeY = static_cast<int>((baseSize + m_mapGenerateDisplayFloor * m_mapLinearGrowthPerFloorY) * std::pow(growth, m_mapGenerateDisplayFloor));

	int baseRoomCount = 3;
	int roomCount = baseRoomCount + m_mapGenerateDisplayFloor * 2;

	//何番目のエリアか(ボス階までを1エリアとする)
	//m_displayFloor は直前で ++ 済みなので 1 始まり
	const int area = (m_displayFloor - 1) / m_bossInterval;

	if (area < MapType::Maptype_Siz)
	{
		m_mapType = static_cast<MapType>(area);
	}
	else
	{
		if ((m_displayFloor - 1) % m_bossInterval == 0)
		{
			m_mapType = static_cast<MapType>(KdRandom::GetInt(0, static_cast<int>(MapType::Maptype_Siz) - 1));
		}
		
	}



	m_spEnemyManager->EnemyListReset();
	m_spMapObjManager->ResetMapObj();

	//工場マップだけ環境音を流す
	if (m_mapType == MapType::MapType_Factory)
	{
		if (!m_spFactoryAmbientSE)
		{
			m_spFactoryAmbientSE = KdAudioManager::Instance().Play("Asset/Sound/SE/Ambient/Factory.wav", true);
		}
		else if (!m_spFactoryAmbientSE->IsPlaying())
		{
			m_spFactoryAmbientSE->Play(true);
		}
		if (m_spFactoryAmbientSE)
		{
			m_spFactoryAmbientSE->SetVolume(VOLInfo::Instance().GetMSSEVol() * kFactoryAmbientVolRate);
		}
	}
	else if (m_spFactoryAmbientSE)
	{
		m_spFactoryAmbientSE->Stop();
	}

	if (m_displayFloor % m_bossInterval != 0)
	{
		if (m_spBossBGM)
		{
			m_spBossBGM->Stop();
		}

		if (m_spGameBGM && !m_spGameBGM->IsPlaying())
		{
			m_spGameBGM->SetVolume(VOLInfo::Instance().GetMSBGMVol() * kGameBGMVolRate);
			m_spGameBGM->Play();
		}

		m_spMapManager->GenerateMap({ (float)mapSizeX,(float)mapSizeY }, roomCount, m_mapType);

		SceneManager::Instance().SetFogEnable(true);
		SceneManager::Instance().SetDefaultAmbient();
	}
	else
	{
		//ボス戦用マップ生成
		mapSizeX = 7;
		mapSizeY = 7;
		m_spMapManager->GenerateBossMap({ (float)mapSizeX,(float)mapSizeY }, m_mapType);
		m_spUIManager->SetMinimapRevealAll(true);

		if (m_spBossBGM)
		{
			m_spBossBGM->SetVolume(VOLInfo::Instance().GetMSBGMVol() * kBossBGMVolRate);
			m_spBossBGM->Play();
		}

		//ボスの登場の音(工場はロボット、それ以外は巨人の雄たけび)
		SEPlayer::Play2D(m_mapType == MapType::MapType_Factory
			? "Asset/Sound/SE/Enemy/Boss/Appear_Metal.wav"
			: "Asset/Sound/SE/Enemy/Boss/Appear_Flesh.wav", 0.9f);

		if (m_spGameBGM)
		{
			m_spGameBGM->Stop();
		}

		SceneManager::Instance().SetFogEnable(false);
		SceneManager::Instance().SetAmbient({ 0.8,0.8,0.8,0.8 });
	}


	Math::Vector3 playerSpawn = m_spMapManager->GetPlayerSpawnPos();
	m_spPlayer->SetPos(playerSpawn);


	m_spMapObjManager->ObjSetCamera(m_spCamera);


	//更新チャンク決め
	m_spMapManager->SetPlayerChunk(m_spPlayer->GetPos());

	if (m_spMapObjManager)
	{
		m_spMapObjManager->SetMapObjUpdateList(m_spMapManager);
	}


	if (m_spEnemyManager)
	{
		m_spEnemyManager->SetEnemyUpdateList(m_spMapManager);
	}

	m_spEnemyManager->SetVisionFlg(true);
}

void GameScene::WarpGateInit(Math::Vector3 _setNextGatePos, Math::Vector3 _setEscapeGatePos)
{
	if (!m_spWarpGateManager) { return; }

	m_spWarpGateManager->WarpGateInit(_setNextGatePos, _setEscapeGatePos);
}

void GameScene::ChangeResultScene()
{
	SetGameResult();
	FadeDraw::Instance().SetFadeType(FadeIn, 3);	//リザルトは暗いところから明るくなる
	SceneManager::Instance().SetNextScene(SceneManager::SceneType::Result);
}

void GameScene::PlayerDefeat()
{
	m_playerDefeatFlg = true;

	FadeDraw::Instance().SetFadeType(FadeOut, 3);	//倒れたら暗転してからリザルトへ
}

void GameScene::RequestNextFloor()
{
	if (m_floorChangeState != FloorChangeState::None) { return; }
	FadeDraw::Instance().SetFadeType(FadeOut, kFloorFadeTime);	//画面 → 黒
	m_floorChangeState = FloorChangeState::FadeToBlack;
}

void GameScene::Event()
{
	//BGM の音量を毎フレーム設定に合わせる(ゲーム中に音量を変えてもすぐ反映される)
	//倍率は鳴らし始めたときと同じ(通常 0.4・ボス 0.1)
	if (m_spGameBGM && m_spGameBGM->IsPlaying())
	{
		m_spGameBGM->SetVolume(VOLInfo::Instance().GetMSBGMVol() * kGameBGMVolRate);
	}
	if (m_spBossBGM && m_spBossBGM->IsPlaying())
	{
		m_spBossBGM->SetVolume(VOLInfo::Instance().GetMSBGMVol() * kBossBGMVolRate);
	}
	if (m_spFactoryAmbientSE && m_spFactoryAmbientSE->IsPlaying())
	{
		m_spFactoryAmbientSE->SetVolume(VOLInfo::Instance().GetMSSEVol() * kFactoryAmbientVolRate);
	}

	if (m_playerDefeatFlg)
	{
		if (!FadeDraw::Instance().GetFadeFlg())
		{
			ChangeResultScene();
		}

		return;
	}

	//階層移動(暗転 → 階層表示 → 明るく)
	UpdateFloorChange();

	////////////////////////////////////////////
	//デバック
	static bool reFlg = false;

	if (GetAsyncKeyState('3') & 0x8000)
	{

		if (!reFlg)
		{
			RequestNextFloor();
			reFlg = true;
		}
	}
	else
	{
		reFlg = false;
	}

	if (GetAsyncKeyState('5') & 0x8000)
	{
		DebugInfo::Instance().SetSceneManagerImGUIFlg(true);
	}

	if (GetAsyncKeyState('6') & 0x8000)
	{
		DebugInfo::Instance().SetSceneManagerImGUIFlg(false);
	}


	if (GetAsyncKeyState('7') & 0x8000)
	{
		ChangeResultScene();
	}

	//////////////////////////////////////////

	///////////////////////////////////////////////////
	//更新チャンク更新
	///////////////////////////////////////////////////
	if (!m_spMapManager) { return; }
	//更新チャンク決め
	m_spMapManager->SetPlayerChunk(m_spPlayer->GetPos());

	// プレイヤーのチャンクが移動した時だけ、更新リストを再構築する
	if (m_spMapManager->GetIsChunkChanged())
	{
		if (m_spMapObjManager)
		{
			m_spMapObjManager->SetMapObjUpdateList(m_spMapManager);
		}
		if (m_spEnemyManager)
		{
			m_spEnemyManager->SetEnemyUpdateList(m_spMapManager);
		}


		if (m_spCamera)
		{
			m_spCamera->ClearCameraOcclusionObject();
			for (auto& map : m_spMapManager->GetUpdateChunks())
			{
				m_spCamera->ResolveCameraOcclusionObject(map);
			}
			for (auto& mapObj : m_spMapObjManager->GetMapUpdateObj())
			{
				m_spCamera->ResolveCameraOcclusionObject(mapObj);
			}
		}
	}

	//あたり判定セット

	//プレイヤーのあたり判定リストを毎フレーム更新
	m_spMapManager->MapHit(m_spPlayer);

	//プレイヤーとマップオブジェクトのあたり判定
	if (m_spMapObjManager)
	{
		m_spMapObjManager->MapObjHit(m_spPlayer);
	}

	//敵のあたり判定リストを毎フレーム更新
	if (!m_spEnemyManager) { return; }
	for (auto enemy : m_spEnemyManager->GetEnemyUpdateList())
	{
		m_spMapManager->MapHitEnemy(enemy);
		if (m_spMapObjManager)
		{
			m_spMapObjManager->MapObjHit(enemy);
		}
	}

	///////////////////////////////////////////////////
	if (!m_spWeapon) { return; }
	m_spWeapon->ClearAttackHitCharacterList();
	m_spWeapon->ClearObjList();
	std::vector<std::shared_ptr<MapBase>> mapList = m_spMapManager->GetUpdateChunks();
	for (std::shared_ptr<MapBase> spMap : mapList)
	{
		m_spWeapon->AddObjList(spMap);
	}


	//武器とのあたり判定
	auto& enemies = m_spEnemyManager->GetEnemyUpdateList();
	for (auto& enemyA : enemies)
	{
		// 武器とのあたり判定
		if (m_spWeapon)
		{
			m_spWeapon->AddAttackHitCharacterList(enemyA);
		}
	}


	//KdDebugGUI::Instance().ClearLog();
	//KdDebugGUI::Instance().AddLog("kaisuu: %d\n", m_displayFloor);



	float dt = DeltaTime::Instance().GetRealDeltaTime();

	m_displayFloorInfo.m_playTimeSc += dt;
}

void GameScene::ReloadEnemyAttackPatterns()
{
	if (!m_spEnemyManager) { return; }

	//パターンを使っていない敵は何もしない(EnemyBase::ReloadAttackPatterns)
	for (const std::shared_ptr<EnemyBase>& spEnemy : m_spEnemyManager->GetEnemyList())
	{
		if (spEnemy)
		{
			spEnemy->ReloadAttackPatterns();
		}
	}
}

void GameScene::UpdateFloorChange()
{
	switch (m_floorChangeState)
	{
	case FloorChangeState::None:
		//移動していない
		break;

	case FloorChangeState::FadeToBlack:
		//真っ黒になりきったら(FadeDraw が止まったら)
		if (!FadeDraw::Instance().GetFadeFlg())
		{
			GenerateMap();
			if (m_spFloorTitleUI)
			{
				m_spFloorTitleUI->Show(m_displayFloor, static_cast<int>(m_mapType));
			}
			m_floorChangeState = FloorChangeState::ShowFloor;
		}
		break;

	case FloorChangeState::ShowFloor:
		//階層表示の時間が終わったら
		if (!m_spFloorTitleUI || m_spFloorTitleUI->IsFinished())
		{
			if (m_spFloorTitleUI) { m_spFloorTitleUI->Hide(); }

			//黒 → 明るく(同じフレームで始めるので隙間ができない)
			FadeDraw::Instance().SetFadeType(FadeIn, kFloorFadeTime);
			m_floorChangeState = FloorChangeState::FadeFromBlack;
		}
		break;

	case FloorChangeState::FadeFromBlack:
		//明るくなりきったら終わり
		if (!FadeDraw::Instance().GetFadeFlg())
		{
			m_floorChangeState = FloorChangeState::None;
		}
		break;
	}
}
