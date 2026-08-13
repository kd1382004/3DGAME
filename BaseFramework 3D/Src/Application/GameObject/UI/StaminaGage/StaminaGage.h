#pragma once

#pragma once
#include"../UIBase.h"

class StaminaGage :public UIBase
{
public:
	StaminaGage() {};
	~StaminaGage()override {};

	void Init()override;
	void Update()override;
	void PreDraw()override;

	void DrawSprite()override;


	void Set2DPos(const Math::Vector2& pos) {
		m_2DPos = pos;
		m_staminaBarTexBack2DPos = { m_2DPos.x - m_staminaBarTexBackLength.x / 2.0f * m_siz ,m_2DPos.y };
		m_staminaBarTex2DPos = { m_2DPos.x - m_staminaBarTexLength.x / 2.0f * m_siz ,m_2DPos.y };
		}

	//パーセント(0.0f～1.0f)
	void SetStaminaBarTexPercent(float _percent);

	void SetSiz(float _siz) { m_siz = _siz; }

	void Delete();
private:

	//サイズ
	float m_siz;

	/////////////////////////////////////////////
	//HPバー背景
	std::shared_ptr<KdTexture>m_staminaBarBackTex = nullptr;
	Math::Vector2 m_staminaBarTexBack2DPos = Math::Vector2::Zero;

	//画像の立て横の長さ
	Math::Vector2 m_staminaBarTexBackLength = Math::Vector2::Zero;

	//画像の切り取り範囲
	Math::Rectangle m_staminaBarTexBackRect;

	/////////////////////////////////////////////
	//HPバー本体
	std::shared_ptr<KdTexture>m_staminaBarTex = nullptr;
	Math::Vector2 m_staminaBarTex2DPos = Math::Vector2::Zero;

	//画像の立て横の長さ
	Math::Vector2 m_staminaBarTexLength = Math::Vector2::Zero;

	//画像の切り取り範囲
	Math::Rectangle  m_staminaBarTexRect;

	//色
	Math::Color  m_staminaBarTexColor = kWhiteColor;

	//パーセント
	float m_staminaBarTexPercent = 1;
};

