#include "AttackGage.h"

void AttackGage::Init()
{
	if (!m_attackGageBackTex)
	{
		m_attackGageBackTex = std::make_shared<KdTexture>();
		m_attackGageBackTex->Load("Asset/Textures/GameUI/AttackGage/AttackGageBackTex.png");


		long w = m_attackGageBackTex->GetWidth();
		long h = m_attackGageBackTex->GetHeight();

		m_attackGageackBackTexLength = { (float)w,(float)h };
		m_attackGageBackTexRect = { 0, 0, w, h };
	}


	if (!m_attackGagekTex)
	{
		m_attackGagekTex = std::make_shared<KdTexture>();
		m_attackGagekTex->Load("Asset/Textures/GameUI/AttackGage/AttackGageTex.png");

		long w = m_attackGagekTex->GetWidth();
		long h = m_attackGagekTex->GetHeight();

		m_attackGageaTexLength = { (float)w,(float)h };
		m_attackGageTexRect = { 0, 0, w, h };
	}
}

void AttackGage::Update()
{
}

void AttackGage::PreDraw()
{
}

void AttackGage::DrawSprite()
{
	if (!m_drawFlg) { return; }

	Math::Vector2 pivot = { 0.0f, 0.5f };

	//攻撃ゲージ背景
	if (m_attackGageBackTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_attackGageBackTex, m_attackGageBackTex2DPos.x, m_attackGageBackTex2DPos.y, m_attackGageackBackTexLength.x * m_siz, m_attackGageackBackTexLength.y * m_siz, &m_attackGageBackTexRect, &kWhiteColor, pivot);
	}

	//攻撃ゲージ背景
	if (m_attackGageBackTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_attackGagekTex, m_attackGageTex2DPos.x, m_attackGageTex2DPos.y, m_attackGageaTexLength.x * m_siz * m_attackGageTexPercent, m_attackGageaTexLength.y * m_siz, &m_attackGageTexRect, &kRedColor, pivot);
	}
}

void AttackGage::SetAttackGageTexPercent(float _percent)
{
	float percent = std::clamp(_percent, 0.0f, 1.0f);

	m_attackGageTexPercent = 1 - percent;

	// rectを計算
	float totalSrcWidth = m_attackGageaTexLength.x;
	long left = static_cast<long>(totalSrcWidth * percent);
	long w = static_cast<long>(totalSrcWidth * m_attackGageTexPercent);
	long top = 0;
	long height = static_cast<long>(m_attackGageaTexLength.y);
	m_attackGageTexRect = { left, top, w, height }; 
}

void AttackGage::Delete()
{
	m_isExpired = true;
}
