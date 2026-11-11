#include "WarpGateBase.h"
#include"../Character/Player/PlayerBase.h"
#include"../../Scene/GameScene/GameScene.h"
#include"../UI/UIManager.h"
#include"../UI/UI2/UIBase2.h"
#include"../Camera/TPSCamera/TPSCamera.h"
#include"../../Info/DeltaTime/DeltaTime.h"

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

void WarpGateBase::UpdateMagicCircleAnime()
{
	if (!m_spWarpGateWork) { return; }

	m_animTime += DeltaTime::Instance().GetGameDeltaTime();
	// 長時間放置しても誤差が溜まらないように戻す（約1時間で一周）
	if (m_animTime > 3600.0f) { m_animTime -= 3600.0f; }

	// パーツ名（ノード名に含まれる文字列）と回転速度（度/秒）
	struct Part
	{
		const char* key;
		float degPerSec;
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
			if (node.m_name.find(part.key) == std::string::npos) { continue; }

			Math::Matrix mat = Math::Matrix::CreateRotationY(
				DirectX::XMConvertToRadians(part.degPerSec * m_animTime));

			// 浮いている輪は上下にふわふわ
			if (std::string_view(part.key) == "Halo")
			{
				float y = 0.45f + 0.08f * std::sin(m_animTime * 1.6f);
				mat *= Math::Matrix::CreateTranslation(0.0f, y, 0.0f);
			}

			node.m_localTransform = mat;
			break;
		}
	}
}

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

void WarpGateBase::ShowGuideUI(int key)
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
		spUI->SetDrawKey(key);
	}
}
