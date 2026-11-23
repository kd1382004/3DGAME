#pragma once
#include"../CampLayoutData/CampLayoutData.h"
class CampLight : public KdGameObject
{
public:
	void PostUpdate() override
	{
		// 色 × 明るさ を渡す(松明は {24,10,2} のように1を超える値を使っている)
		KdShaderManager::Instance().WorkAmbientController().AddPointLight(
			m_data.m_color * m_data.m_intensity,
			m_data.m_radius,
			m_data.m_pos);
	}

	void SetData(const CampLightData& _data) { m_data = _data; }
	CampLightData& WorkData() { return m_data; }

private:
	CampLightData m_data;
};