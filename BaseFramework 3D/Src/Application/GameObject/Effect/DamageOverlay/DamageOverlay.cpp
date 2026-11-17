#include "DamageOverlay.h"

void DamageOverlay::Init()
{
	if (!m_spDamageOverlayTex)
	{
		m_spDamageOverlayTex = std::make_shared<KdTexture>();
		m_spDamageOverlayTex->Load("Asset/Textures/GameUI/Effect/DamageOverlay/DamageOverlay.png");
	}
}

void DamageOverlay::PreUpdate()
{
}

void DamageOverlay::Update()
{
	float dt = DeltaTime::Instance().GetGameDeltaTime();

	if (m_damageOverlayAlpha > 0.0f)
	{
		m_damageOverlayAlpha -= m_damageOverlayFadeSpeed * dt;
		if (m_damageOverlayAlpha < 0.0f)
		{
			m_damageOverlayAlpha = 0.0f;
		}

	}
}

void DamageOverlay::DrawSprite()
{
	if (!m_spDamageOverlayTex) { return; }
	Math::Color darkRed = { 0.706f, 0.0f, 0.0f, m_damageOverlayAlpha };

	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spDamageOverlayTex, 0, 0, nullptr, &darkRed);
}