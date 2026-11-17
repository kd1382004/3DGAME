#include "StaminaGage.h"

void StaminaGage::Init()
{


	if (!m_spStaminaBarBackTex)
	{
		m_spStaminaBarBackTex = std::make_shared<KdTexture>();
		m_spStaminaBarBackTex->Load("Asset/Textures/GameUI/StaminaGage/StaminaGageBackTex.png");

		m_staminaBarTexBackLength = { (float)m_spStaminaBarBackTex->GetWidth(),	(float)m_spStaminaBarBackTex->GetHeight() };
		m_staminaBarTexBackRect = { 0,0,(long)m_staminaBarTexBackLength.x,(long)m_staminaBarTexBackLength.y };
	}

	if (!m_spStaminaBarTex)
	{
		m_spStaminaBarTex = std::make_shared<KdTexture>();
		m_spStaminaBarTex->Load("Asset/Textures/GameUI/StaminaGage/StaminaGageTex.png");

		m_staminaBarTexLength = { (float)m_spStaminaBarTex->GetWidth(),	(float)m_spStaminaBarTex->GetHeight() };
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
	if (m_spStaminaBarBackTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spStaminaBarBackTex, m_staminaBarTexBack2DPos.x, m_staminaBarTexBack2DPos.y, m_staminaBarTexBackLength.x * m_siz, m_staminaBarTexBackLength.y * m_siz, &m_staminaBarTexBackRect, &kWhiteColor, pivot);
	}

	//スタミナゲージ本体
	if (m_spStaminaBarTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spStaminaBarTex, m_staminaBarTex2DPos.x, m_staminaBarTex2DPos.y, m_staminaBarTexLength.x * m_siz * m_staminaBarTexPercent, m_staminaBarTexLength.y * m_siz, &m_staminaBarTexRect, &m_staminaBarTexColor, pivot);
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

