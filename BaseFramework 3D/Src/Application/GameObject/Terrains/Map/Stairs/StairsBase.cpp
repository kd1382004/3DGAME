//---- 自分のヘッダー ----
#include "StairsBase.h"									//階段

//---- プレイヤー ----
#include"../../../Character/Player/PlayerBase.h"		//プレイヤー

//---- 便利機能 ----
#include"../../../../Info/VOL/VOLInfo.h"				//音量設定

//=============================================
// コンストラクタ・デストラクタ
//=============================================

StairsBase::~StairsBase()
{

	std::shared_ptr<PlayerBase>spPlayerBase = m_wpPlayerBase.lock();

	if (spPlayerBase)
	{
		spPlayerBase->SetNextFloorActionFlg(false);
	}


	// エフェクトオブジェクトを取得
	if (auto spEffect = m_wpAuraEffect.lock())
	{
		spEffect->StopEffect();
	}

	// 鳴り続ける音を止める
	if (m_spAmbient3DSE)
	{
		m_spAmbient3DSE->Stop();
		m_spAmbient3DSE = nullptr;
	}
}

//=============================================
// 基本の処理
//=============================================

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


	// 画面内に映っている時：一時停止解除（再生）
	DirectX::SimpleMath::Vector3 worldEffectPos = DirectX::SimpleMath::Vector3::Transform(m_effectLocalPos, m_mWorld);

	m_wpAuraEffect = KdEffekseerManager::GetInstance().Play(
		"Stairs/Stairs.efkefc",
		worldEffectPos,
		1.0f,
		1.0f,
		true // ループ再生
	);

	auto spEffect = m_wpAuraEffect.lock();
	if (spEffect)
	{
		spEffect->SetLoop(false);
	}

	m_effectLocalPos.z += 5;

	// 階段のまわりで鳴り続ける音（近づくと聞こえる）
	if (!m_spAmbient3DSE)
	{
		m_spAmbient3DSE = KdAudioManager::Instance().Play3D("Asset/Sound/SE/Stairs/Ambient.wav", worldEffectPos, true);
		if (m_spAmbient3DSE)
		{
			m_spAmbient3DSE->SetCurveDistanceScaler(15.0f);
			m_spAmbient3DSE->SetVolume(VOLInfo::Instance().GetMSSEVol() * 0.6f);
		}
	}

	MapBase::Init();
}

void StairsBase::Update()
{
	std::shared_ptr<PlayerBase>spPlayerBase = m_wpPlayerBase.lock();

	if (spPlayerBase)
	{
		Math::Vector3 playerPos = spPlayerBase->GetPos();

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


		spPlayerBase->SetNextFloorActionFlg(m_playerHit);
	}
}

void StairsBase::PostUpdate()
{
	// 鳴り続ける音の位置と音量（音量の設定が変わっても合わせる）
	if (m_spAmbient3DSE)
	{
		DirectX::SimpleMath::Vector3 sePos = DirectX::SimpleMath::Vector3::Transform(m_effectLocalPos, m_mWorld);
		m_spAmbient3DSE->SetPos(sePos);
		m_spAmbient3DSE->SetVolume(VOLInfo::Instance().GetMSSEVol() * 0.6f);
	}

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

//=============================================
// 取得
//=============================================

void StairsBase::IsHit()
{

}
