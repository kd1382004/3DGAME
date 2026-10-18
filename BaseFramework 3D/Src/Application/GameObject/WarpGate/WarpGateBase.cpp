#include "WarpGateBase.h"
#include"../Character/Player/PlayerBase.h"
#include"../../Scene/GameScene/GameScene.h"
#include"../UI/UIManager.h"
#include"../UI/UI2/UIBase2.h"
#include"../Camera/TPSCamera/TPSCamera.h"

void WarpGateBase::Init()
{
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

}

void WarpGateBase::DrawLit()
{
	if (m_spWarpGate)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spWarpGate, m_mWorld);
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
