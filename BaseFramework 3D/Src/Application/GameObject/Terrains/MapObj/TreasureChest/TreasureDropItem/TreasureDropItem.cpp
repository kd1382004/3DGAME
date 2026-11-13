#include "TreasureDropItem.h"
#include"../../../../Character/Player/PlayerBase.h"
#include"../LootTableManager/LootTableManager.h"
#include"../../../../Character/Player/PlayerInventory/PlayerInventory.h"

#include"../../../../../Info/DeltaTime/DeltaTime.h"
void TreasureDropItem::Init()
{
	if (!m_dropItemModel)
	{
		m_dropItemModel = std::make_shared<KdModelData>();
		m_dropItemModel->Load("Asset/Models/TreasureChest/TreasureDropItem/TreasureDropItem.gltf");
	}
}

void TreasureDropItem::PreUpdate()
{

}

void TreasureDropItem::Update()
{
	PlayerChase();

	Math::Matrix tMat = Math::Matrix::CreateTranslation(m_pos);
	m_mWorld = tMat;
}

void TreasureDropItem::DrawEffect()
{
	if (m_dropItemModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_dropItemModel, m_mWorld);
	}
}

void TreasureDropItem::GetItem()
{
	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayer.lock();
	if (!spPlayer) { return; }


	std::shared_ptr<LootTableManager>m_spLootTableManager = m_wpLootTableManager.lock();
	if (m_spLootTableManager)
	{
		// 出る個数はテーブルの dropMin〜dropMax
		int itenNun = m_spLootTableManager->GetRandomDropNum(m_path);
		for (int i = 0; i < itenNun; i++)
		{
			LootItem item = m_spLootTableManager->GetRandomLoot(m_path);

			switch (item.type)
			{
			case Potion:
				spPlayer->GetPlayerInventory()->AddPotionsInventory(item.id);
				break;
			case Weapon:
				spPlayer->GetPlayerInventory()->AddWeaponsStrengtheningInventory(item.id);
				break;
			default:
				break;
			}


		}
	}

	m_isExpired = true;
}

void TreasureDropItem::PlayerChase()
{
	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayer.lock();
	if (!spPlayer) { return; }

	m_playerPos = spPlayer->GetPos();

	Math::Vector3 dir = m_playerPos - m_pos;
	dir.Normalize();
	Math::Vector3 toPlayer = m_playerPos - m_pos;
	float dist = toPlayer.Length();


	float speed = (m_spped + m_accel) * DeltaTime::Instance().GetGameDeltaTime();
	float maxSpeed = (m_maxSpeed)*DeltaTime::Instance().GetGameDeltaTime();

	speed = std::min(speed, maxSpeed);

	toPlayer = m_playerPos - m_pos;
	dist = toPlayer.Length();

	if (dist < speed || dist < m_getRadius)
	{
		GetItem();            // プレイヤーに入手を伝える
		return;
	}
	else
	{
		m_pos += dir * speed;
	}



	m_accel += DeltaTime::Instance().GetGameDeltaTime();

}
