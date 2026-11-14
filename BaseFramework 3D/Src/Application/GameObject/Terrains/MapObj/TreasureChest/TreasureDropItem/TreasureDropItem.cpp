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

	m_dropStateMode = DropState::Pop;
}

void TreasureDropItem::PreUpdate()
{

}

void TreasureDropItem::Update()
{
	const float dt = DeltaTime::Instance().GetGameDeltaTime();
	m_time += dt;
	m_rotY += m_rotSpeed * dt;
	if (m_rotY >= 360.0f) { m_rotY -= 360.0f; }

	switch (m_dropStateMode)
	{
	case DropState::Pop:
		PopUpdate();
		break;
	case DropState::Float:
		FloatUpdate();
		break;
	case DropState::Attract:
		AttractUpdate();
		break;
	case DropState::Get:
		GetItem();
		break;
	default:
		break;
	}


	Math::Matrix rMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_rotY));
	Math::Matrix tMat = Math::Matrix::CreateTranslation(m_pos);
	m_mWorld = rMat * tMat;
}

void TreasureDropItem::DrawEffect()
{
	if (m_dropItemModel)
	{
		// ふわっと明滅させて光らせる
		float pulse = (sinf(m_time * 6.0f) + 1.0f) * 0.5f;
		Math::Color emi = { 1.0f + pulse, 1.0f + pulse, 1.0f + pulse, 1.0f };
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_dropItemModel, m_mWorld, emi);
	}
}

void TreasureDropItem::PopUpdate()
{
	const float dt = DeltaTime::Instance().GetGameDeltaTime();

	// 最初の1回だけ：上向きの速度 ＋ ランダムな横向きの速度
	if (!m_popInit)
	{
		m_popInit = true;
		m_startY = m_pos.y;

		static std::mt19937 mt{ std::random_device{}() };
		std::uniform_real_distribution<float> angle(0.0f, DirectX::XM_2PI);
		std::uniform_real_distribution<float> side(0.3f, 1.0f);

		float a = angle(mt);
		float s = m_popSideSpeed * side(mt);
		m_vel = { cosf(a) * s, m_popUpSpeed, sinf(a) * s };
	}

	// 重力をかけて放物線で飛ばす
	m_vel.y -= m_gravity * dt;
	m_pos += m_vel * dt;

	// 頂点を過ぎて、止まる高さまで落ちてきたら浮遊へ
	const float stopY = m_startY + m_floatHeight;
	if (m_vel.y < 0.0f && m_pos.y <= stopY)
	{
		m_pos.y = stopY;
		m_floatBaseY = stopY;
		m_vel = Math::Vector3::Zero;
		m_floatTimer = 0.0f;
		m_dropStateMode = DropState::Float;
	}
}

void TreasureDropItem::FloatUpdate()
{
	const float dt = DeltaTime::Instance().GetGameDeltaTime();
	m_floatTimer += dt;

	// その場でふわふわ上下
	m_pos.y = m_floatBaseY + sinf(m_floatTimer * m_bobSpeed) * m_bobHeight;

	// 少し浮いたらプレイヤーへ
	if (m_floatTimer >= m_floatTime)
	{
		m_accel = 0.0f;
		m_dropStateMode = DropState::Attract;
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

void TreasureDropItem::AttractUpdate()
{
	std::shared_ptr<PlayerBase> spPlayer = m_wpPlayer.lock();
	if (!spPlayer) { return; }

	const float dt = DeltaTime::Instance().GetGameDeltaTime();

	// 足元ではなく胸あたりを狙う
	m_playerPos = spPlayer->GetPos();
	m_playerPos.y += m_targetHeight;

	Math::Vector3 toPlayer = m_playerPos - m_pos;
	float dist = toPlayer.Length();


	// このフレームで進む距離（だんだん速く、上限あり）
	float speed = std::min(m_spped + m_accel, m_maxSpeed) * dt;

	// 届く距離なら入手（行き過ぎ防止）
	if (dist <= speed || dist < m_getRadius)
	{
		m_pos = m_playerPos;
		m_dropStateMode = DropState::Get;
		return;
	}

	toPlayer.Normalize();
	m_pos += toPlayer * speed;

	m_accel += m_AddAccel * dt;
}
