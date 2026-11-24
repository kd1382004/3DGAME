//---- 自分のヘッダー ----
#include "StaminaGage.h"		//スタミナゲージ

//=============================================
// 基本の処理
//=============================================

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

	if (!m_spstaminaMemoryTex)
	{
		m_spstaminaMemoryTex = std::make_shared<KdTexture>();
		m_spstaminaMemoryTex->Load("Asset/Textures/GameUI/StaminaGage/StaminaGageScaleTex.png");
	}

	m_size = 1;
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
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spStaminaBarBackTex, m_staminaBarTexBack2DPos.x, m_staminaBarTexBack2DPos.y, m_staminaBarTexBackLength.x * m_size, m_staminaBarTexBackLength.y * m_size, &m_staminaBarTexBackRect, &kWhiteColor, pivot);
	}

	//スタミナゲージ本体
	if (m_spStaminaBarTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spStaminaBarTex, m_staminaBarTex2DPos.x, m_staminaBarTex2DPos.y, m_staminaBarTexLength.x * m_size * m_staminaBarTexPercent, m_staminaBarTexLength.y * m_size, &m_staminaBarTexRect, &m_staminaBarTexColor, pivot);
	}


	//Hpバーメモリ
	if (m_spstaminaMemoryTex)
	{
		//バー本体の左端と横幅(画面上)
		const float barLeft = m_staminaBarTex2DPos.x;
		const float barWidth = m_staminaBarTexLength.x * m_size;

		//目盛りの大きさ(画像の大きさ × 拡大率)
		const int memoryW = static_cast<int>(m_spstaminaMemoryTex->GetWidth() * m_size);
		const int memoryH = static_cast<int>(m_spstaminaMemoryTex->GetHeight() * m_size);

		for (float hp = m_staminaPerScale; hp < m_maxStamina; hp += m_staminaPerScale)
		{
			//最大HPに対する割合(0〜1)
			float rate = hp / m_maxStamina;

			//バーの左端から、その割合だけ右(ドット絵なので整数に丸める)
			int x = static_cast<int>(std::round(barLeft + barWidth * rate));

			//減った部分の目盛りは少し薄くする
			Math::Color memoryColor = { 0,0,0,1 };
			if ((rate >= m_staminaBarTexPercent))
			{
				memoryColor = { 0,0,0,0.4 };
			}

			//x の位置に目盛りを描く(バーと同じく左端・上下中央を基準)
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_spstaminaMemoryTex,
				x, m_staminaBarTex2DPos.y,
				memoryW, memoryH,
				nullptr, &memoryColor, pivot);
		}
	}
}

//=============================================
// 設定
//=============================================

void StaminaGage::SetStaminaBarTexPercent(float _percent)
{
	float percent = std::clamp(_percent, 0.0f, 1.0f);

	m_staminaBarTexPercent = percent;
}

//=============================================
// その他の処理
//=============================================

void StaminaGage::Delete()
{

}
