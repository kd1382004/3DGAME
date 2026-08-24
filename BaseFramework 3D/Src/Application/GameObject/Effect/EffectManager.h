#pragma once

#include"EffectBase.h"
class EffectManager :public KdGameObject
{
public:
	EffectManager() {};
	~EffectManager()override {};

	void Init()override;
	void PreUpdate()override;
	void Update()override;

	void DrawEffect()override;
	void DrawSprite()override;

	void AddEffectList(std::shared_ptr<EffectBase> _effec) { m_effectBaseList.push_back(_effec); }
private:

	std::list< std::shared_ptr<EffectBase>>m_effectBaseList;

};
