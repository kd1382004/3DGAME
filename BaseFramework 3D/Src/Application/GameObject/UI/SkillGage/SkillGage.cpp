#include "SkillGage.h"

void SkillGage::Init()
{
	if (!m_spGaugeTexture)
	{
		m_spFrameTexture = std::make_shared<KdTexture>();
		m_spFrameTexture->Load("Asset/Textures/GameUI/SkillGage/SkillGage.png");

		m_spGaugeTexture = std::make_shared<KdTexture>();
		m_spGaugeTexture->Load("Asset/Textures/GameUI/SkillGage/FrameTexture.png");

		m_siz.x = m_spGaugeTexture->GetInfo().Width;
		m_siz.y = m_spGaugeTexture->GetInfo().Height;

		m_gaugePercent = 0.5;
	}
}

void SkillGage::Update()
{

}

void SkillGage::DrawSprite()
{
	Math::Vector2 pivot = { 0.5f, 0.0f };

	Math::Color color = { 1,1,1,0.5 };

	// ① 白枠を描画
	KdShaderManager::Instance().m_spriteShader.DrawTex(
		m_spFrameTexture,
		m_2DPos.x, m_2DPos.y,
		m_siz.x, m_siz.y,
		nullptr,
		&color,
		pivot
	);

	// ② ゲージ部分を縦に切り抜いて描画
	Math::Rectangle srcRect = {
		0,
		(long)(m_spGaugeTexture->GetHeight() * (1.0f - m_gaugePercent)), // 上から切り落とす
		(long)m_spGaugeTexture->GetWidth(),
		(long)(m_spGaugeTexture->GetHeight() * m_gaugePercent)          // 下側だけ描画
	};

	KdShaderManager::Instance().m_spriteShader.DrawTex(
		m_spGaugeTexture,
		m_2DPos.x, m_2DPos.y,   // 白枠の底に合わせる
		m_siz.x, m_siz.y * m_gaugePercent,
		&srcRect,
		&color,
		pivot
	);
}
