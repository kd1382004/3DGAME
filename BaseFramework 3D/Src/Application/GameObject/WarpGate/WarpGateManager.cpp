#include "WarpGateManager.h"
#include"WarpGateBase.h"

#include"../../Scene/SceneManager.h"
void WarpGateManager::Init()
{
}
void WarpGateManager::Update()
{
	for (auto warp : m_warpGateList)
	{
		warp->Update();
	}
}
void WarpGateManager::PreDraw()
{
	for (auto warp : m_warpGateList)
	{
		warp->PreDraw();
	}
}
void WarpGateManager::DrawLit()
{
	for (auto warp : m_warpGateList)
	{
		warp->DrawLit();
	}
}
void WarpGateManager::WarpGateInit(Math::Vector3 _setPos)
{
	std::shared_ptr<WarpGateBase>_spWarpGateBase = std::make_shared<WarpGateBase>();
	_spWarpGateBase->Init();
	_spWarpGateBase->SetPos(_setPos);
	_spWarpGateBase->SetPlayer(m_wpPlayer.lock());

	m_warpGateList.push_back(_spWarpGateBase);
}
