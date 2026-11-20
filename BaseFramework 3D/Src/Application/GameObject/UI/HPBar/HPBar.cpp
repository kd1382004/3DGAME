#include "HPBar.h"

#include"../../../Info/NumDraw/NumDraw.h"

void HPBar::Init()
{

	if (!m_spHPBarBackTex)
	{
		m_spHPBarBackTex = std::make_shared<KdTexture>();
		m_spHPBarBackTex->Load("Asset/Textures/GameUI/HPBar/HPBarBackTex.png");

		m_HPBarBackTexLength = { (float)m_spHPBarBackTex->GetWidth(),	(float)m_spHPBarBackTex->GetHeight() };
		m_HPBarBackTexRect = { 0,0,(long)m_HPBarBackTexLength.x,(long)m_HPBarBackTexLength.y };
	}

	if (!m_spHPMemoryTex)
	{
		m_spHPMemoryTex = std::make_shared<KdTexture>();
		m_spHPMemoryTex->Load("Asset/Textures/GameUI/HPBar/HPBarScaleTex.png");
	}

	if (!m_spHPBarTex)
	{
		m_spHPBarTex = std::make_shared<KdTexture>();
		m_spHPBarTex->Load("Asset/Textures/GameUI/HPBar/HPBarTex.png");

		m_HPBarTexLength = { (float)m_spHPBarTex->GetWidth(),	(float)m_spHPBarTex->GetHeight() };
		m_HPBarTexRect = { 0,0,(long)m_HPBarTexLength.x,(long)m_HPBarTexLength.y };
	}

	if (!m_spHPBarIconFrameTex)
	{
		m_spHPBarIconFrameTex = std::make_shared<KdTexture>();
		m_spHPBarIconFrameTex->Load("Asset/Textures/GameUI/HPBar/HPBarIconFrameTex.png");

		m_HPBarIconFrameTexLength = { (float)m_spHPBarIconFrameTex->GetWidth(),	(float)m_spHPBarIconFrameTex->GetHeight() };
		m_HPBarIconFrameTexRect = { 0,0,(long)m_HPBarIconFrameTexLength.x,(long)m_HPBarIconFrameTexLength.y };
	}

	m_siz = 1;
	m_HPBarTexPercent = std::clamp(m_HPBarTexPercent, 0.0f, 1.0f);
}

void HPBar::Update()
{

}

void HPBar::PreDraw()
{

	if (!m_drawFlg) { return; }

	HPBarDelayPreDraw();

	m_HPBarTexRect = { 0, 0, static_cast<long>(m_HPBarTexLength.x * m_HPBarTexPercent), static_cast<long>(m_HPBarTexLength.y) };

	//遅延バーの座標補正
	float hpBarDelayOffset = m_HPBarTexLength.x * m_siz * m_HPBarTexPercent;
	m_HPBarDelayTex2DPos = { m_HPBarTex2DPos.x + hpBarDelayOffset, m_HPBarTex2DPos.y };
}

void HPBar::DrawSprite()
{
	if (!m_drawFlg) { return; }

	Math::Vector2 pivot = { 0.0f, 0.5f };

	//HPバー背景
	if (m_spHPBarBackTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spHPBarBackTex, m_HPBarBackTex2DPos.x, m_HPBarBackTex2DPos.y, m_HPBarBackTexLength.x * m_siz, m_HPBarBackTexLength.y * m_siz, &m_HPBarBackTexRect, &kWhiteColor, pivot);
	}

	

	//HPバー本体
	if (m_spHPBarTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spHPBarTex, m_HPBarTex2DPos.x, m_HPBarTex2DPos.y, m_HPBarTexLength.x * m_siz * m_HPBarTexPercent, m_HPBarTexLength.y * m_siz, &m_HPBarTexRect, &m_HPBarTexColor, pivot);
	}


	//HPバー遅延分
	if (m_spHPBarTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spHPBarTex, m_HPBarDelayTex2DPos.x, m_HPBarDelayTex2DPos.y, m_HPBarTexLength.x * m_siz * m_HPBarDelayTexPercent * m_HPBarTexProgress, m_HPBarTexLength.y * m_siz, &m_HPBarDelayTexRect, &m_HPBarDelayTexColor, pivot);
	}

	//Hpバーメモリ
	if (m_spHPMemoryTex&& m_drawMemoryFlg)
	{
		//バー本体の左端と横幅(画面上)
		const float barLeft = m_HPBarTex2DPos.x;
		const float barWidth = m_HPBarTexLength.x * m_siz;

		//目盛りの大きさ(画像の大きさ × 拡大率)
		const int memoryW = static_cast<int>(m_spHPMemoryTex->GetWidth() * m_siz);
		const int memoryH = static_cast<int>(m_spHPMemoryTex->GetHeight() * m_siz);

		for (float hp = m_hpPerScale; hp < m_maxHP; hp += m_hpPerScale)
		{
			//最大HPに対する割合(0〜1)
			float rate = hp / m_maxHP;

			//バーの左端から、その割合だけ右(ドット絵なので整数に丸める)
			int x = static_cast<int>(std::round(barLeft + barWidth * rate));

			//減った部分の目盛りは少し薄くする
			Math::Color memoryColor = { 0,0,0,1 };
			if ((rate >= m_HPBarTexPercent))
			{
				memoryColor = { 0,0,0,0.4 };
			}

			//x の位置に目盛りを描く(バーと同じく左端・上下中央を基準)
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_spHPMemoryTex,
				x, m_HPBarTex2DPos.y,
				memoryW, memoryH,
				nullptr, &memoryColor, pivot);
		}
	}

	//アイコン用フレーム
	if (m_spHPBarIconFrameTex)
	{
		pivot = { 0.5f, 0.5f };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spHPBarIconFrameTex, m_HPBarIconFrameTex2DPos.x, m_HPBarIconFrameTex2DPos.y, m_HPBarIconFrameTexLength.x * m_siz, m_HPBarIconFrameTexLength.y * m_siz, &m_HPBarIconFrameTexRect, &kWhiteColor, pivot);

		// アイコンの枠の内側(8割)に収める
		NumDraw::GetInstance().Draw(m_LV, CAligned, m_HPBarIconFrameTex2DPos, kWhiteColor, m_siz, false, -1, { m_HPBarIconFrameTexLength.x * m_siz * 0.8f, m_HPBarIconFrameTexLength.y * m_siz * 0.8f });
	}
}

void HPBar::SetHPBarTexPercent(float _percent)
{
	float percent = std::clamp(_percent, 0.0f, 1.0f);

	m_HPBarDelayTexPercent = m_HPBarTexPercent - percent;

	m_HPBarTexPercent = percent;

	//遅延分を計算
	float hpBarDelayOffset = m_HPBarTexLength.x * m_siz * m_HPBarTexPercent;

	// 遅延バーの描画位置
	m_HPBarDelayTex2DPos.x = m_HPBarTex2DPos.x + hpBarDelayOffset;

	// rectを計算
	float totalSrcWidth = m_HPBarTexLength.x;
	long left = static_cast<long>(totalSrcWidth * percent);
	long w = static_cast<long>(totalSrcWidth * m_HPBarDelayTexPercent);
	long top = 0;
	long height = static_cast<long>(m_HPBarTexLength.y);
	m_HPBarDelayTexRect = { left, top, w, height };
	m_HPBarTexProgress = 1.0f;

}

void HPBar::SetHPBarTexPercentNODelayOffset(float _percent)
{
	float percent = std::clamp(_percent, 0.0f, 1.0f);

	m_HPBarDelayTexPercent = m_HPBarTexPercent - percent;

	m_HPBarTexPercent = percent;
}

void HPBar::SetLV(int _LV)
{
	m_LV = _LV;
}

void HPBar::Delete()
{
	m_isExpired = true;
}

void HPBar::HPBarDelayPreDraw()
{
	if (m_HPBarTexProgress <= 0) { return; }

	m_HPBarTexProgress -= DeltaTime::Instance().GetGameDeltaTime() / m_HPBarDelayTexPercentDownSpeed;
	float progress = std::clamp(m_HPBarTexProgress, 0.0f, 1.0f);

	if (m_HPBarTexProgress <= 0)
	{
		m_HPBarTexProgress = 0;
	}
}
