#include "StairsBase.h"
#include"../../../Character/Player/PlayerBase.h"


StairsBase::~StairsBase()
{

	std::shared_ptr<PlayerBase>spPalyerBase = m_wpPlayerBase.lock();

	if (spPalyerBase)
	{
		spPalyerBase->SetNextFloorActionFlg(false);
	}


	// エフェクトオブジェクトを取得
	if (auto spEffect = m_wpAuraEffect.lock())
	{
		spEffect->StopEffect();
	}
}

void StairsBase::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Terrains/Map/Castle/Stairs/Stairs.gltf");
	}

	if (!m_pCollider)
	{
		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("Stairs", m_spModel, KdCollider::TypeBump | KdCollider::TypeCameraOcclusion | KdCollider::TypeGround);
	}


	m_wpAuraEffect = KdEffekseerManager::GetInstance().Play(
		"Stairs/Stairs.efkefc",
		{},
		1.0f,
		1.0f,
		true // ループ再生
	);

	m_effectLocalPos.z += 5;

	MapBase::Init();
}

void StairsBase::Update()
{
	std::shared_ptr<PlayerBase>spPalyerBase = m_wpPlayerBase.lock();

	if (spPalyerBase)
	{
		Math::Vector3 playerPos = spPalyerBase->GetPos();

		KdCollider::RayInfo rayInfo;
		rayInfo.m_pos = playerPos;
		rayInfo.m_pos.y += 0.3;
		rayInfo.m_dir = Math::Vector3::Down;
		rayInfo.m_range = 5;
		rayInfo.m_type = KdCollider::TypeGround;

		if (Intersects(rayInfo, nullptr))
		{
			m_playerHit = true;
		}
		else
		{
			m_playerHit = false;
		}


		spPalyerBase->SetNextFloorActionFlg(m_playerHit);
	}
}

void StairsBase::PostUpdate()
{
	// エフェクトオブジェクトを取得
	if (auto spEffect = m_wpAuraEffect.lock())
	{
		if (m_isInView)
		{
			// 画面内に映っている時：一時停止解除（再生）
			DirectX::SimpleMath::Vector3 worldEffectPos = DirectX::SimpleMath::Vector3::Transform(m_effectLocalPos, m_mWorld);
			spEffect->SetPos(worldEffectPos);
			spEffect->SetLoop(true);
		}
		else
		{
			// 画面外に出ている時：一時停止（負荷を0にする）
			spEffect->SetLoop(false);
		}
	}
}

void StairsBase::IsHit()
{

}
