//---- 自分のヘッダー ----
#include "AttackGage.h"		//攻撃(溜め)ゲージ

//=============================================
// 基本の処理
//=============================================

void AttackGage::Init()
{
	if (!m_spAttackGaugeBackTex)
	{
		m_spAttackGaugeBackTex = std::make_shared<KdTexture>();
		m_spAttackGaugeBackTex->Load("Asset/Textures/GameUI/AttackGage/AttackGageBackTex.png");


		long w = m_spAttackGaugeBackTex->GetWidth();
		long h = m_spAttackGaugeBackTex->GetHeight();

		m_attackGaugeBackTexLength = { (float)w,(float)h };
		m_attackGaugeBackTexRect = { 0, 0, w, h };
	}


	if (!m_spAttackGaugeTex)
	{
		m_spAttackGaugeTex = std::make_shared<KdTexture>();
		m_spAttackGaugeTex->Load("Asset/Textures/GameUI/AttackGage/AttackGageTex.png");

		long w = m_spAttackGaugeTex->GetWidth();
		long h = m_spAttackGaugeTex->GetHeight();

		m_attackGaugeTexLength = { (float)w,(float)h };
		m_attackGaugeTexRect = { 0, 0, w, h };
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
	if (m_spAttackGaugeBackTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spAttackGaugeBackTex, m_attackGaugeBackTex2DPos.x, m_attackGaugeBackTex2DPos.y, m_attackGaugeBackTexLength.x * m_size, m_attackGaugeBackTexLength.y * m_size, &m_attackGaugeBackTexRect, &kWhiteColor, pivot);
	}

	//攻撃ゲージ背景
	if (m_spAttackGaugeBackTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spAttackGaugeTex, m_attackGaugeTex2DPos.x, m_attackGaugeTex2DPos.y, m_attackGaugeTexLength.x * m_size * m_attackGaugeTexPercent, m_attackGaugeTexLength.y * m_size, &m_attackGaugeTexRect, &kRedColor, pivot);
	}
}

//=============================================
// 設定
//=============================================

void AttackGage::SetAttackGaugeTexPercent(float _percent)
{
	float percent = std::clamp(_percent, 0.0f, 1.0f);

	m_attackGaugeTexPercent = 1 - percent;

	// rectを計算
	float totalSrcWidth = m_attackGaugeTexLength.x;
	long left = static_cast<long>(totalSrcWidth * percent);
	long w = static_cast<long>(totalSrcWidth * m_attackGaugeTexPercent);
	long top = 0;
	long height = static_cast<long>(m_attackGaugeTexLength.y);
	m_attackGaugeTexRect = { left, top, w, height }; 
}

//=============================================
// その他の処理
//=============================================

void AttackGage::Delete()
{
	m_isExpired = true;
}
