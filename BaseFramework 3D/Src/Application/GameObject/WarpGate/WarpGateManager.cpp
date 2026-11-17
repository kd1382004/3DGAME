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

void WarpGateManager::WarpGateInit(Math::Vector3 _setNextGatePos, Math::Vector3 _setEscapeGatePos)
{
	auto spSelf = std::static_pointer_cast<WarpGateManager>(shared_from_this());

	std::shared_ptr<EscapeGate>spEscapeGate = std::make_shared<EscapeGate>();
	spEscapeGate->Init();
	spEscapeGate->SetPos(_setEscapeGatePos);
	spEscapeGate->SetPlayer(m_wpPlayer.lock());
	spEscapeGate->SetGameScene(m_wpGameScene.lock());
	spEscapeGate->SetWarpGateManager(spSelf);

	m_warpGateList.push_back(spEscapeGate);

	std::shared_ptr<NextStageGate>spNextStageGate = std::make_shared<NextStageGate>();
	spNextStageGate->Init();
	spNextStageGate->SetPos(_setNextGatePos);
	spNextStageGate->SetPlayer(m_wpPlayer.lock());
	spNextStageGate->SetGameScene(m_wpGameScene.lock());
	spNextStageGate->SetWarpGateManager(spSelf);

	m_warpGateList.push_back(spNextStageGate);
}

void WarpGateManager::WarpGateReset()
{
	for (auto obj : m_warpGateList)
	{
		obj->SetIsExpired(true);
	}
}

