//---- 自分のヘッダー ----
#include "TreasureChest.h"													//宝箱

//---- シーン ----
#include"../../../../Scene/GameScene/GameScene.h"							//ゲームシーン
#include"../../../../Scene/SceneManager.h"									//シーンの管理

//---- カメラ ----
#include"../../../Camera/CameraBase.h"										//カメラの基底クラス

//---- プレイヤー ----
#include"../../../Character/Player/PlayerBase.h"							//プレイヤー
#include"../../../Character/Player/PlayerInventory/PlayerInventory.h"		//プレイヤーの所持品

//---- マップオブジェクト ----
#include"LootTableManager/LootTableManager.h"								//宝箱のドロップテーブル管理
#include"TreasureDropItem/TreasureDropItem.h"								//宝箱から出るアイテム

//---- UI ----
#include"../../../UI/UIMap/UIMap_TreasureChest/UIMap_TreasureChest.h"		//ミニマップの宝箱アイコン
#include"../../../UI/UIManager.h"											//UIの管理
#include"../../../UI\UIMap/UIMapManager.h"									//ミニマップの管理
#include"../../../UI/UI2/UIBase2.h"											//キー表示付きUIの基底クラス

//---- 便利機能 ----
#include"../../../../Info/VOL/VOLInfo.h"									//音量設定
#include"../../../../Info/KeyInfo/KeyInfo.h"								//キー入力

//=============================================
// コンストラクタ・デストラクタ
//=============================================

TreasureChest::~TreasureChest()
{
	auto spUIMap_TreasureChest = m_wpUIMap_TreasureChest.lock();
	if (spUIMap_TreasureChest)
	{
		spUIMap_TreasureChest->SetExpired(true);
	}

	if (m_spTreasureChestSE)
	{
		m_spTreasureChestSE->Stop();
	}
}

//=============================================
// 基本の処理
//=============================================

void TreasureChest::Init()
{
	if (m_wpLootTableManager.expired())
	{
		// ゲーム中で共有するテーブル（エディタで保存すると読み直される）
		m_wpLootTableManager = LootTableManager::GetShared();
	}

	if (!m_spTreasureChestModel)
	{
		m_spTreasureChestModel = std::make_shared<KdModelWork>();
		m_spTreasureChestModel->SetModelData("Asset/Models/TreasureChest/TreasureBox.gltf");

		m_spTreasureChestAnimator = std::make_shared<KdAnimator>();
	}


	if (!m_pCollider)
	{
		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("TreasureChest", m_spTreasureChestModel, KdCollider::TypeBump | KdCollider::TypeCameraOcclusion | KdCollider::TypeGround);
	}




	if (!m_spTreasureChestModel) { return; }

	auto spData = m_spTreasureChestModel->GetData();

	if (!spData) { return; }

	const auto& meshNodeIndices = spData->GetDrawMeshNodeIndices();

	if (meshNodeIndices.empty()) { return; }

	// 全メッシュの頂点をまとめる（複数メッシュ対応）
	std::vector<Math::Vector3> allPositions;
	for (int nodeIndex : meshNodeIndices)
	{
		const auto& node = spData->GetOriginalNodes()[nodeIndex];
		if (!node.m_spMesh) { continue; }
		const auto& positions = node.m_spMesh->GetVertexPositions();
		allPositions.insert(allPositions.end(), positions.begin(), positions.end());
	}

	if (allPositions.empty()) { return; }

	// ローカル空間の OBB を生成
	DirectX::BoundingOrientedBox localOBB;
	DirectX::BoundingOrientedBox::CreateFromPoints(
		localOBB,
		allPositions.size(),
		allPositions.data(),
		sizeof(Math::Vector3)
	);

	m_frustumBox = KdCollider::BoxInfo(0, localOBB);


	if (!m_spTreasureChestSE)
	{

		m_spTreasureChestSE = KdAudioManager::Instance().Play3D("Asset/Sound/SE/TreasureChest/TreasureChest.wav", GetPos(), false);
		if (m_spTreasureChestSE)
		{
			m_spTreasureChestSE->Stop();
			m_spTreasureChestSE->SetCurveDistanceScaler(1.0f);
		}
	}

}

void TreasureChest::Update()
{
	if (m_open)
	{
		return;
	}


	if (m_isOpen)
	{
		m_spTreasureChestAnimator->AdvanceTime(m_spTreasureChestModel->WorkNodes(), 100);
		m_spTreasureChestModel->CalcNodeMatrices();
		if (m_spTreasureChestAnimator->IsAnimationEnd())
		{
			auto spUIMap_TreasureChest = m_wpUIMap_TreasureChest.lock();
			if (spUIMap_TreasureChest)
			{
				spUIMap_TreasureChest->SetExpired(true);
			}

			auto spGameScene = m_wpGameScene.lock();
			if (spGameScene)
			{
				spGameScene->AddTreasureChestsNum();
			}
			m_open = true;

		}




		float animePercent = m_spTreasureChestAnimator->GetAnimeProgress();
		if (!m_dropItemInitFlg && animePercent >= m_dropItemInitPercent)
		{
			std::shared_ptr<PlayerBase>spPlayer = m_wpPlayer.lock();
			if (spPlayer)
			{


				std::shared_ptr<TreasureDropItem>spTreasureDropItem = std::make_shared<TreasureDropItem>();
				spTreasureDropItem->Init();
				std::string path = m_treasureChestGetItemLootPath + std::to_string(m_treasureChestTableNum);
				spTreasureDropItem->SetPath(path);
				spTreasureDropItem->SetPlayer(spPlayer);
				std::shared_ptr<LootTableManager>spLootTableManager = m_wpLootTableManager.lock();
				spTreasureDropItem->SetLootTableManager(spLootTableManager);
				spTreasureDropItem->SetPos(GetPos());

				SceneManager::Instance().AddObject(spTreasureDropItem);
			}

			m_dropItemInitFlg = true;
		}
		return;
	}

	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayer.lock();
	if (spPlayer)
	{
		Math::Vector3 playerPos = spPlayer->GetPos();
		Math::Vector3 pos = GetPos();

		float dist = (playerPos - pos).Length();

		if (dist < m_openSize)
		{
			int key = spPlayer->GetActionKeyConfig().m_interact;

			auto spUIMapManager = m_wpUIManager.lock();
			std::shared_ptr<UIBase2>spUI = nullptr;
			if (spUIMapManager)
			{
				spUI = spUIMapManager->GetUIBase2();
			}
			if (spUI)
			{

				Math::Vector3 set2DPos;

				std::shared_ptr<CameraBase>spCamera = m_wpCamera.lock();
				if (!spCamera) { return; }

				Math::Vector3 set3DPos = GetPos();
				set3DPos.y += 2;
				spCamera->GetCamera()->ConvertWorldToScreenDetail(set3DPos, set2DPos);

				spUI->SetDrawFlg(true);
				spUI->Set2DPos({ set2DPos.x,set2DPos.y });
				spUI->SetTex(m_spUiTex);
				spUI->SetDrawKey(key);
			}

			if (KeyInfo::Instance().GetValidKeyPush(key, true))
			{
				m_isOpen = true;
				m_spTreasureChestAnimator->SetAnimation(m_spTreasureChestModel->GetAnimation("Open"), false);
				if (m_spTreasureChestSE)
				{
					m_spTreasureChestSE->Stop();
				}

				// 宝箱が開く光
				KdEffekseerManager::GetInstance().Play("TreasureChest/Chest_Open.efkefc", GetPos(), 1.0f, 1.0f, false);

				// 宝箱が開く音
				{
					std::shared_ptr<KdSoundInstance> spSE = KdAudioManager::Instance().Play("Asset/Sound/SE/TreasureChest/Open.wav", false);
					if (spSE) { spSE->SetVolume(VOLInfo::Instance().GetMSSEVol() * 1.0f); }
				}
			}
		}
	}
}

void TreasureChest::PostUpdate()
{
	if (m_open)
	{
		return;
	}

	//if (!m_isInView) { return; }
	//KdShaderManager::Instance().WorkAmbientController().AddPointLight(
	//	{ 20,20,20 },								//色
	//	10,										//半径	
	//	GetPos() + Math::Vector3(0, 1, 0)		//座標
	//);

	if (m_spTreasureChestSE)
	{
		ApplySoundVolume();
		if (!m_spTreasureChestSE->IsPlaying())
		{
			m_spTreasureChestSE->Play(true);
		}
		m_spTreasureChestSE->SetPos(GetPos());

	}
}

void TreasureChest::ApplySoundVolume()
{
	//開いた宝箱はもう鳴らさない
	if (m_open) { return; }

	if (m_spTreasureChestSE)
	{
		m_spTreasureChestSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
	}
}

void TreasureChest::GenerateDepthMapFromLight()
{
	if (!m_isInView) { return; }

	if (m_spTreasureChestModel)
	{
		DrawModelWithFade(*m_spTreasureChestModel, false);
	}
}

void TreasureChest::PreDraw()
{
	//カメラに映ってるかどうか
	std::shared_ptr<CameraBase>spCamera = m_wpCamera.lock();
	if (spCamera)
	{
		m_isInView = CheckInScreen(spCamera->GetBoundingFrustum(), m_frustumBox);
	}
}

void TreasureChest::DrawLit()
{
	if (!m_isInView) { return; }

	if (m_spTreasureChestModel)
	{
		DrawModelWithFade(*m_spTreasureChestModel);
	}
}

//=============================================
// 設定
//=============================================

void TreasureChest::SetUIManager(std::shared_ptr<UIManager> _spUIManager)
{
	if (!_spUIManager) { return; }

	m_wpUIManager = _spUIManager;

	std::shared_ptr<UIMap_TreasureChest> spUIMap_TreasureChest = std::make_shared<UIMap_TreasureChest>();
	m_wpUIMap_TreasureChest = spUIMap_TreasureChest;

	auto spUIMapManager = _spUIManager->GetUIMapManager();
	if (spUIMapManager)
	{
		spUIMapManager->AddUIMap_TreasureChest(spUIMap_TreasureChest, GetPos());
	}

}

void TreasureChest::SetRotation(Math::Matrix _rMat)
{
	Math::Vector3 pos = GetPos();
	Math::Vector3 size = GetScale();

	Math::Matrix tMat = Math::Matrix::CreateTranslation(pos);
	Math::Matrix sMat = Math::Matrix::CreateScale(size);

	m_mWorld = sMat * _rMat * tMat;
}
