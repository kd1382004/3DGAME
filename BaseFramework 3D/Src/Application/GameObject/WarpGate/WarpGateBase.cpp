//---- 自分のヘッダー ----
#include "WarpGateBase.h"						//ワープゲートの基底クラス

//---- シーン ----
#include"../../Scene/GameScene/GameScene.h"		//ゲームシーン

//---- カメラ ----
#include"../Camera/TPSCamera/TPSCamera.h"		//三人称カメラ

//---- プレイヤー ----
#include"../Character/Player/PlayerBase.h"		//プレイヤー

//---- UI ----
#include"../UI/UIManager.h"						//UIの管理
#include"../UI/UI2/UIBase2.h"					//キー表示付きUIの基底クラス

//---- 便利機能 ----
#include"../../Info/DeltaTime/DeltaTime.h"		//デルタタイム
#include"../../Info/SE/SEPlayer.h"				//効果音

//=============================================
// コンストラクタ・デストラクタ
//=============================================

WarpGateBase::~WarpGateBase()
{
	//ループしている音は止めないと消えたあとも鳴り続ける
	if (m_spHum3DSE) { m_spHum3DSE->Stop(); }
}

//=============================================
// 基本の処理
//=============================================

void WarpGateBase::Init()
{
	if (m_spWarpGate && !m_spWarpGateWork)
	{
		m_spWarpGateWork = std::make_shared<KdModelWork>(m_spWarpGate);
	}

	if (!m_pCollider)
	{
		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("WarpGate", m_spWarpGate, KdCollider::TypeCameraOcclusion | KdCollider::TypeGround);
	}
}

void WarpGateBase::Update()
{
}

void WarpGateBase::PreDraw()
{
	UpdateMagicCircleAnime();
	UpdateHumSE();
}

void WarpGateBase::DrawLit()
{
	if (m_spWarpGateWork)
	{
		// 光の強さをゆっくり脈打たせる（Emissive はマテリアルの発光色 × この値）
		float pulse = 0.5f + 0.5f * std::sin(m_animTime * 2.5f);
		float emi = 0.8f + 0.8f * pulse;
		KdShaderManager::Instance().m_StandardShader.DrawModel(
			*m_spWarpGateWork, m_mWorld, kWhiteColor, { emi, emi, emi });
	}
	else if (m_spWarpGate)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spWarpGate, m_mWorld);
	}
}

//=============================================
// その他の処理
//=============================================

bool WarpGateBase::PlayerHit()
{
	std::shared_ptr<PlayerBase>spPlayer = m_wpPlayer.lock();
	if (!spPlayer) { return false; }


	KdCollider::RayInfo rayInfo;
	rayInfo.m_pos = spPlayer->GetPos();
	rayInfo.m_pos.y += 1;
	rayInfo.m_dir = { 0,-1,0 };
	rayInfo.m_type = KdCollider::TypeGround;
	rayInfo.m_range = 3;

	if (Intersects(rayInfo, nullptr))
	{
		return true;
	}

	return false;
}

void WarpGateBase::UpdateMagicCircleAnime()
{
	if (!m_spWarpGateWork) { return; }

	m_animTime += DeltaTime::Instance().GetGameDeltaTime();
	// 長時間放置しても誤差が溜まらないように戻す（約1時間で一周）
	if (m_animTime > 3600.0f) { m_animTime -= 3600.0f; }

	// パーツ名（ノード名に含まれる文字列）と回転速度（度/秒）
	struct Part
	{
		const char* m_key;
		float m_degPerSec;
	};
	static const Part parts[] =
	{
		{ "Outer",   8.0f },	// 外周のルーン：ゆっくり
		{ "Star",  -18.0f },	// 六芒星：逆回転
		{ "Inner",  30.0f },
		{ "Core",  -45.0f },	// 中心：速め
		{ "Halo",   22.0f },	// 浮いている輪
	};

	for (auto& node : m_spWarpGateWork->WorkNodes())
	{
		for (const auto& part : parts)
		{
			if (node.m_name.find(part.m_key) == std::string::npos) { continue; }

			Math::Matrix mat = Math::Matrix::CreateRotationY(
				DirectX::XMConvertToRadians(part.m_degPerSec * m_animTime));

			// 浮いている輪は上下にふわふわ
			if (std::string_view(part.m_key) == "Halo")
			{
				float y = 0.45f + 0.08f * std::sin(m_animTime * 1.6f);
				mat *= Math::Matrix::CreateTranslation(0.0f, y, 0.0f);
			}

			node.m_localTransform = mat;
			break;
		}
	}
}

void WarpGateBase::UpdateHumSE()
{
	//消える予定なら止める
	if (m_isExpired)
	{
		if (m_spHum3DSE) { m_spHum3DSE->Stop(); }
		return;
	}

	if (!m_spHum3DSE)
	{
		m_spHum3DSE = SEPlayer::Play3D("Asset/Sound/SE/WarpGate/Hum.wav", m_pos, 0.35f, true);
		if (m_spHum3DSE) { m_spHum3DSE->SetCurveDistanceScaler(6.0f); }
		return;
	}

	//位置と音量(設定の変更)を合わせる
	m_spHum3DSE->SetPos(m_pos);
	m_spHum3DSE->SetVolume(VOLInfo::Instance().GetMSSEVol() * 0.35f);
}

void WarpGateBase::ShowGuideUI(int _key)
{
	if (!m_spGuideUITex) { return; }

	auto spGameScene = m_wpGameScene.lock();
	if (!spGameScene) { return; }

	auto spUIManager = spGameScene->GetUIManager();
	if (!spUIManager) { return; }

	auto spUI = spUIManager->GetUIBase2();
	if (!spUI) { return; }

	auto spCamera = spGameScene->GetCamera();
	if (!spCamera || !spCamera->GetCamera()) { return; }

	Math::Vector3 set2DPos;
	Math::Vector3 set3DPos = GetPos();
	set3DPos.y += 2.0f;
	spCamera->GetCamera()->ConvertWorldToScreenDetail(set3DPos, set2DPos);

	if (set2DPos.z > 0.0f)
	{
		spUI->SetDrawFlg(true);
		spUI->Set2DPos({ set2DPos.x, set2DPos.y });
		spUI->SetTex(m_spGuideUITex);
		spUI->SetDrawKey(_key);
	}
}
