#pragma once
#include"../UIBase.h"

class HPBar :public UIBase
{
public:
	HPBar() {};
	~HPBar()override {};

	void Init()override;
	void Update()override;
	void PreDraw()override;

	void DrawSprite()override;


	void Set2DPos(const Math::Vector2& pos) {
		m_2DPos = pos;
		m_HPBarBackTex2DPos = { m_2DPos.x - m_HPBarBackTexLength.x / 2.0f * m_siz ,m_2DPos.y };
		m_HPBarTex2DPos = { m_2DPos.x - m_HPBarTexLength.x / 2.0f * m_siz ,m_2DPos.y };
		m_HPBarDelayTex2DPos = { m_2DPos.x - m_HPBarTexLength.x / 2.0f * m_siz ,m_2DPos.y };
		m_HPBarIconFrameTex2DPos = { m_HPBarBackTex2DPos.x - m_HPBarIconFrameTexLength.x / 2.0f * m_siz ,m_2DPos.y };
	}

	//パーセント(0.0f～1.0f)
	void SetHPBarTexPercent(float _percent);

	void SetDrawFlg(bool _flg) { m_drawFlg = _flg; }

	void SetSiz(float _siz) { m_siz = _siz; }

	void Delete();


	void SetLV(int _LV) { m_LV = _LV; }
private:

	bool m_drawFlg = false;

	//サイズ
	float m_siz;

	/////////////////////////////////////////////
	//HPバー背景
	std::shared_ptr<KdTexture>m_HPBarBackTex = nullptr;
	Math::Vector2 m_HPBarBackTex2DPos = Math::Vector2::Zero;

	//画像の立て横の長さ
	Math::Vector2 m_HPBarBackTexLength = Math::Vector2::Zero;

	//画像の切り取り範囲
	Math::Rectangle m_HPBarBackTexRect;

	/////////////////////////////////////////////
	//HPバー本体
	std::shared_ptr<KdTexture>m_HPBarTex = nullptr;
	Math::Vector2 m_HPBarTex2DPos = Math::Vector2::Zero;

	//画像の立て横の長さ
	Math::Vector2 m_HPBarTexLength = Math::Vector2::Zero;

	//画像の切り取り範囲
	Math::Rectangle  m_HPBarTexRect;

	//色
	Math::Color  m_HPBarTexColor = kWhiteColor;

	//パーセント
	float m_HPBarTexPercent = 1;

	//進捗度
	float m_HPBarTexProgres = 0;

	//進捗を終わらせる秒数
	float m_HPBarDelayTexPercentDownSped = 1;

	////////////////////
	//HPバー遅延

	void HPBarDelayPreDraw();

	Math::Vector2 m_HPBarDelayTex2DPos = Math::Vector2::Zero;

	//色
	Math::Color  m_HPBarDelayTexColor = kRedColor;

	//画像の切り取り範囲
	Math::Rectangle  m_HPBarDelayTexRect;

	//パーセント
	float m_HPBarDelayTexPercent = 1;

	/////////////////////////////////////////////
	//アイコン用フレーム
	std::shared_ptr<KdTexture>m_HPBarIconFrameTex = nullptr;
	Math::Vector2 m_HPBarIconFrameTex2DPos = Math::Vector2::Zero;

	//画像の立て横の長さ
	Math::Vector2 m_HPBarIconFrameTexLength = Math::Vector2::Zero;

	//画像の切り取り範囲
	Math::Rectangle m_HPBarIconFrameTexRect;

	//LV
	int m_LV = 1;

};
