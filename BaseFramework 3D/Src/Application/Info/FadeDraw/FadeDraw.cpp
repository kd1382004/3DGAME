#include "FadeDraw.h"

void FadeDraw::Init()
{
	if (!m_tex)
	{
		m_tex = std::make_shared<KdTexture>();
		m_tex->Load("Asset/Textures/FadeDraw/FadeDraw.png");
	}
}

void FadeDraw::Update()
{
	if (!m_fadeFlg) return;

	// 経過時間を 0.0 ～ 1.0 に正規化
	m_nowTime += DeltaTime::Instance().GetRealDeltaTime();

	float t = m_nowTime / m_time;
	if (t > 1.0f) t = 1.0f;

	// -------------------------
	// フェードタイプ別の処理
	// -------------------------
	switch (m_fadeType)
	{
	case FedeIn:
		// 0 → 1 に上がる（画面が明るくなる）
		m_color.w = t;
		break;

	case FedeOut:
		// 1 → 0 に下がる（画面が暗くなる）
		m_color.w = 1.0f - t;
		break;
	case NoFede:
		m_color.w = 0.0f;
		break;
	}

	// 終了判定
	if (t >= 1.0f)
	{
		m_fadeFineFlg = true;
	}
}

void FadeDraw::DrawSprite()
{
	if (!m_fadeFlg) return;
	if (!m_tex) { return; }

	KdShaderManager::Instance().m_spriteShader.DrawTex(m_tex, 0, 0, 1280, 720, nullptr, &m_color);

	if (m_fadeFineFlg)
	{
		m_fadeFineFlg = false;
		m_fadeFlg = false;
	}
}
