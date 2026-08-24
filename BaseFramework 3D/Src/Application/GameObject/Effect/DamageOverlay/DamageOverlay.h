#pragma once

#include"../EffectBase.h"
class DamageOverlay:public EffectBase
{
public:
	DamageOverlay() {};
	~DamageOverlay()override {};

	void Init()override;
	void PreUpdate()override;
	void Update()override;

	void DrawSprite()override;

	void OnDamaged(float damage, float maxHP)
	{
		// 最大体力に対してどれくらい削れたか（0〜1）
		float ratio = damage / maxHP;

		// 濃さを調整
		float intensity = std::min(0.7f, ratio * 2.0f);

		m_damageOverlayAlpha = intensity;
	}

private:

	std::shared_ptr<KdTexture>m_DamageOverlayTex;


	float m_damageOverlayAlpha;
	float m_damageOverlayFadeSpeed = 1.0f;

};
