#include "StaminaGage.h"

void StaminaGage::Init()
{


	if (!m_staminaBarBackTex)
	{
		m_staminaBarBackTex = std::make_shared<KdTexture>();
		m_staminaBarBackTex->Load("Asset/Textures/GameUI/StaminaGage/StaminaGageBackTex.png");

		m_staminaBarTexBackLength = { (float)m_staminaBarBackTex->GetWidth(),	(float)m_staminaBarBackTex->GetHeight() };
		m_staminaBarTexBackRect = { 0,0,(long)m_staminaBarTexBackLength.x,(long)m_staminaBarTexBackLength.y };
	}

	if (!m_staminaBarTex)
	{
		m_staminaBarTex = std::make_shared<KdTexture>();
		m_staminaBarTex->Load("Asset/Textures/GameUI/StaminaGage/StaminaGageTex.png");

		m_staminaBarTexLength = { (float)m_staminaBarTex->GetWidth(),	(float)m_staminaBarTex->GetHeight() };
		m_staminaBarTexRect = { 0,0,(long)m_staminaBarTexLength.x,(long)m_staminaBarTexLength.y };
	}

	m_siz = 1;
	m_staminaBarTexPercent = std::clamp(m_staminaBarTexPercent, 0.0f, 1.0f);
}

void StaminaGage::Update()
{
}

void StaminaGage::PreDraw()
{
	m_staminaBarTexColor = Math::Color{ 1.0f,0.9f,0.1f,1.0f };
	m_staminaBarTexRect = { 0, 0, static_cast<long>(m_staminaBarTexLength.x * m_staminaBarTexPercent), static_cast<long>(m_staminaBarTexLength.y) };
}

void StaminaGage::DrawSprite()
{
	Math::Vector2 pivot = { 0.0f, 0.5f };

	//スタミナゲージ背景
	if (m_staminaBarBackTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_staminaBarBackTex, m_staminaBarTexBack2DPos.x, m_staminaBarTexBack2DPos.y, m_staminaBarTexBackLength.x * m_siz, m_staminaBarTexBackLength.y * m_siz, &m_staminaBarTexBackRect, &kWhiteColor, pivot);
	}

	//スタミナゲージ本体
	if (m_staminaBarTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_staminaBarTex, m_staminaBarTex2DPos.x, m_staminaBarTex2DPos.y, m_staminaBarTexLength.x * m_siz * m_staminaBarTexPercent, m_staminaBarTexLength.y * m_siz, &m_staminaBarTexRect, &m_staminaBarTexColor, pivot);
	}
}

void StaminaGage::SetStaminaBarTexPercent(float _percent)
{
	float percent = std::clamp(_percent, 0.0f, 1.0f);

	m_staminaBarTexPercent = percent;
}

void StaminaGage::Delete()
{

}

