#include "WarpGateManager.h"
#include"WarpGateBase.h"

#include"../../Scene/SceneManager.h"

#include"EscapeGate/EscapeGate.h"
#include"NextStageGate/NextStageGate.h"
void WarpGateManager::Init()
{

}

void WarpGateManager::PreUpdate()
{
	// Updateの前の更新処理
	// オブジェクトリストの整理 ・・・ 無効なオブジェクトを削除
	auto it = m_warpGateList.begin();

	while (it != m_warpGateList.end())
	{
		if ((*it)->IsExpired())	// IsExpired() ・・・ 無効ならtrue
		{
			// 無効なオブジェクトをリストから削除
			it = m_warpGateList.erase(it);
		}
		else
		{
			++it;	// 次の要素へイテレータを進める
		}
	}
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

void WarpGateManager::WarpGateInit(Math::Vector3 _setNextGatePos, Math::Vector3 _setEacapeGatePos)
{
	auto self = std::static_pointer_cast<WarpGateManager>(shared_from_this());

	std::shared_ptr<EscapeGate>_spEscapeGate = std::make_shared<EscapeGate>();
	_spEscapeGate->Init();
	_spEscapeGate->SetPos(_setEacapeGatePos);
	_spEscapeGate->SetPlayer(m_wpPlayer.lock());
	_spEscapeGate->SetGameScene(m_wpGameScene.lock());
	_spEscapeGate->SetWarpGateManager(self);

	m_warpGateList.push_back(_spEscapeGate);

	std::shared_ptr<NextStageGate>_spNextStageGate = std::make_shared<NextStageGate>();
	_spNextStageGate->Init();
	_spNextStageGate->SetPos(_setNextGatePos);
	_spNextStageGate->SetPlayer(m_wpPlayer.lock());
	_spNextStageGate->SetGameScene(m_wpGameScene.lock());
	_spNextStageGate->SetWarpGateManager(self);

	m_warpGateList.push_back(_spNextStageGate);
}

void WarpGateManager::WarpGateReset()
{
	for (auto obj : m_warpGateList)
	{
		obj->SetIsExpired(true);
	}
}

