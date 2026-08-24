#include "EffectManager.h"

void EffectManager::Init()
{
}

void EffectManager::PreUpdate()
{
	auto it = m_effectBaseList.begin();

	while (it != m_effectBaseList.end())
	{
		if ((*it)->IsExpired())	// IsExpired() ・・・ 無効ならtrue
		{
			// 無効なオブジェクトをリストから削除
			it = m_effectBaseList.erase(it);
		}
		else
		{
			++it;	// 次の要素へイテレータを進める
		}
	}
}

void EffectManager::Update()
{
	for (auto effect : m_effectBaseList)
	{
		effect->Update();
	}
}

void EffectManager::DrawEffect()
{
	for (auto effect : m_effectBaseList)
	{
		effect->DrawEffect();
	}
}

void EffectManager::DrawSprite()
{
	for (auto effect : m_effectBaseList)
	{
		effect->DrawSprite();
	}
}
