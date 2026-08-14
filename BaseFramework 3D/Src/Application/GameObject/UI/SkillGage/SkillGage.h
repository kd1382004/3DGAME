#pragma once

#include"../UIBase.h"
class SkillGage :public UIBase
{
public:
	SkillGage() {};
	~SkillGage() {};

	void Init()override;
	void Update()override;
	void DrawSprite()override;

	void SetGaugePercent(float _percent) { m_gaugePercent = _percent; }
private:


	//サイズ
	Math::Vector2 m_siz;

	std::shared_ptr<KdTexture> m_spGaugeTexture = nullptr;
	std::shared_ptr<KdTexture> m_spFrameTexture = nullptr;

	float m_gaugePercent;

};
