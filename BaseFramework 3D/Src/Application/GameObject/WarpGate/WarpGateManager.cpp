#include "WarpGateManager.h"
#include"WarpGateBase.h"

#include"../../Scene/SceneManager.h"

#include"EscapeGate/EscapeGate.h"
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
void WarpGateManager::WarpGateInit(Math::Vector3 _setPos)
{
	std::shared_ptr<EscapeGate>_spWarpGate = std::make_shared<EscapeGate>();
	_spWarpGate->Init();
	_spWarpGate->SetPos(_setPos);
	_spWarpGate->SetPlayer(m_wpPlayer.lock());
	_spWarpGate->SetGameScene(m_wpGameScene.lock());

	m_warpGateList.push_back(_spWarpGate);
}
