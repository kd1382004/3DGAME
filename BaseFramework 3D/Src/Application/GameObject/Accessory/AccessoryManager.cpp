#include "AccessoryManager.h"

#include"AccessoryBase.h"

void AccessoryManager::Init()
{}

void AccessoryManager::PreUpdate()
{
	auto it = m_accessoryList.begin();

	while (it != m_accessoryList.end())
	{
		if ((*it)->IsExpired())	// IsExpired() ・・・ 無効ならtrue
		{
			// 無効なオブジェクトをリストから削除
			it = m_accessoryList.erase(it);
		}
		else
		{
			++it;	// 次の要素へイテレータを進める
		}
	}
}

void AccessoryManager::Update()
{
	for (auto accessory : m_accessoryList)
	{
		accessory->Update();
	}
}

void AccessoryManager::PostUpdate()
{
	for (auto accessory : m_accessoryList)
	{
		accessory->PostUpdate();
	}
}

void AccessoryManager::DrawLit()
{
	for (auto accessory : m_accessoryList)
	{
		accessory->DrawLit();
	}
}

void AccessoryManager::DrawBright()
{
	for (auto accessory : m_accessoryList)
	{
		accessory->DrawBright();
	}
}


void AccessoryManager::GenerateDepthMapFromLight()
{
	for (auto accessory : m_accessoryList)
	{
		accessory->GenerateDepthMapFromLight();
	}
}
