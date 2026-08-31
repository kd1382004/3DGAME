#pragma once
#include"../UIBase.h"

class AttackGage :public UIBase
{
public:
	AttackGage() {};
	~AttackGage()override {};

	void Init()override;
	void Update()override;
	void PreDraw()override;

	void DrawSprite()override;


	void Set2DPos(const Math::Vector2& pos) {
		m_2DPos = pos;
		m_attackGageBackTex2DPos = { m_2DPos.x - m_attackGageackBackTexLength.x / 2.0f * m_siz ,m_2DPos.y };
		/*m_HPBarTex2DPos = { m_2DPos.x - m_HPBarTexLength.x / 2.0f * m_siz ,m_2DPos.y };
		m_HPBarDelayTex2DPos = { m_2DPos.x - m_HPBarTexLength.x / 2.0f * m_siz ,m_2DPos.y };
		m_HPBarIconFrameTex2DPos = { m_HPBarBackTex2DPos.x - m_HPBarIconFrameTexLength.x / 2.0f * m_siz ,m_2DPos.y };*/
	}

	//パーセント(0.0f～1.0f)
	void SetAttackGageTexPercent(float _percent);

	void SetDrawFlg(bool _flg) { m_drawFlg = _flg; }

	void SetSiz(float _siz) { m_siz = _siz; }

	void Delete();

private:

	bool m_drawFlg = false;

	float m_siz = 1;

	//攻撃ゲージ
	std::shared_ptr<KdTexture>m_attackGageBackTex = nullptr;
	std::shared_ptr<KdTexture>m_attackGagekTex = nullptr;
	Math::Vector2 m_attackGageBackTex2DPos = Math::Vector2::Zero;

	//画像の立て横の長さ
	Math::Vector2 m_attackGageackBackTexLength = Math::Vector2::Zero;

	//画像の切り取り範囲
	Math::Rectangle m_attackGageBackTexRect;

	//攻撃ゲージ
	std::shared_ptr<KdTexture>m_attackGagTex = nullptr;
	Math::Vector2 m_attackGageTex2DPos = Math::Vector2::Zero;

	//画像の立て横の長さ
	Math::Vector2 m_attackGageackTexLength = Math::Vector2::Zero;

	//画像の切り取り範囲
	Math::Rectangle m_attackGageTexRect;

	float m_attackGageTexPercent;
};
