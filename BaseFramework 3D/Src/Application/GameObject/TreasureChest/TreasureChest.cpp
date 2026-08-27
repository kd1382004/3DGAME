#include "TreasureChest.h"
#include"../Character/Player/PlayerBase.h"
#include"../Character/Player/PlayerInventory/PlayerInventory.h"



#include"../../Info/KeyInfo/KeyInfo.h"

#include"../Camera/CameraBase.h"

#include"../UI/UIMap/UIMap_TreasureChest/UIMap_TreasureChest.h"
#include"../UI/UIManager.h"
#include"../UI\UIMap/UIMapManager.h"


#include"LootTableManager/LootTableManager.h"
void TreasureChest::Init()
{
	if (!m_treasureChestModel)
	{
		m_treasureChestModel = std::make_shared<KdModelWork>();
		m_treasureChestModel->SetModelData("Asset/Models/TreasureChest/TreasureBox.gltf");

		m_treasureChestAnimetor = std::make_shared<KdAnimator>();
	}


	if (!m_pCollider)
	{
		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("TreasureChest", m_treasureChestModel, KdCollider::TypeBump | KdCollider::TypeCameraOcclusion|KdCollider::TypeGround);
	}

	


	if (!m_treasureChestModel) { return; }

	auto spData = m_treasureChestModel->GetData();

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

}


void TreasureChest::Update()
{
	
	if (m_IsOpen)
	{ 
		m_treasureChestAnimetor->AdvanceTime(m_treasureChestModel->WorkNodes(), 100);
		m_treasureChestModel->CalcNodeMatrices();
		if (m_treasureChestAnimetor->IsAnimationEnd())
		{
			m_isExpired = true;
			m_wpUIMap_TreasureChest.lock()->SetExpired(true);
		}
		return;
	}

	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayer.lock();
	if (spPlayer)
	{
		Math::Vector3 PlayerPos = spPlayer->GetPos();
		Math::Vector3 Pos = GetPos();

		float dist = (PlayerPos - Pos).Length();

		if (dist < 10)
		{
			int key = spPlayer->GetActionKeyConfig().interact;


			if (KeyInfo::Instance().GetValidKeyPush(key, true))
			{
				m_IsOpen = true;
				m_treasureChestAnimetor->SetAnimation(m_treasureChestModel->GetAnimation("Open"), false);


				std::shared_ptr<LootTableManager>m_spLootTableManager = m_wpLootTableManager.lock();
				if (m_spLootTableManager)
				{
					LootItem item = m_spLootTableManager->GetRandomLoot("treasure_slot_1");

					spPlayer->GetPlayerInventory()->AddPotionsInventory(item.id);
				}
			}
		}
	}
}

void TreasureChest::PostUpdate()
{
	if (!m_isInView) { return; }
	KdShaderManager::Instance().WorkAmbientController().AddPointLight(
		{ 5,5,5 },								//色
		10,										//半径	
		GetPos() + Math::Vector3(0, 1, 0)		//座標
	);
}

void TreasureChest::GenerateDepthMapFromLight()
{
	if (!m_isInView) { return; }

	if (m_treasureChestModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_treasureChestModel, m_mWorld);
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


	if (m_treasureChestModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_treasureChestModel, m_mWorld);
	}
}

void TreasureChest::SetUIManager(std::shared_ptr<UIManager> _spUIManager)
{
	if (!_spUIManager) { return; }

	m_wpUIManager = _spUIManager;

	std::shared_ptr<UIMap_TreasureChest> spUIMap_TreasureChest = std::make_shared<UIMap_TreasureChest>();
	m_wpUIMap_TreasureChest = spUIMap_TreasureChest;

	_spUIManager->GetUIMapManager()->AddUIMap_TreasureChest(spUIMap_TreasureChest,GetPos());

}
