#include "TreasureChest.h"
#include"../../../../Info/VOL/VOLInfo.h"
#include"../../../Character/Player/PlayerBase.h"
#include"../../../Character/Player/PlayerInventory/PlayerInventory.h"

#include"../../../../Scene/GameScene/GameScene.h"
#include"../../../../Scene/SceneManager.h"

#include"../../../../Info/KeyInfo/KeyInfo.h"

#include"../../../Camera/CameraBase.h"

#include"../../../UI/UIMap/UIMap_TreasureChest/UIMap_TreasureChest.h"
#include"../../../UI/UIManager.h"
#include"../../../UI\UIMap/UIMapManager.h"


#include"LootTableManager/LootTableManager.h"
#include"TreasureDropItem/TreasureDropItem.h"

#include"../../../UI/UI2/UIBase2.h"


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

		if (dist < m_openSiz)
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
		m_spTreasureChestSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
		if (!m_spTreasureChestSE->IsPlaying())
		{
			m_spTreasureChestSE->Play(true);
		}
		m_spTreasureChestSE->SetPos(GetPos());

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
