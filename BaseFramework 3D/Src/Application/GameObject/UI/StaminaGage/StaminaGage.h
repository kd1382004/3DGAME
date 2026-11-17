#pragma once

#pragma once
#include"../UIBase.h"

class StaminaGage :public UIBase
{
public:
	//コンストラクタ
	StaminaGage() {};
	//デストラクタ
	~StaminaGage()override {};

	//初期化
	void Init()override;
	//更新
	void Update()override;
	//描画前の処理
	void PreDraw()override;

	//2Dの描画
	void DrawSprite()override;


	//画面上の座標をセット(バーの左端の位置も計算する)
	// _pos ... 画面上の座標(バーの中心)
	void Set2DPos(const Math::Vector2& _pos) {
		m_2DPos = _pos;
		m_staminaBarTexBack2DPos = { m_2DPos.x - m_staminaBarTexBackLength.x / 2.0f * m_siz ,m_2DPos.y };
		m_staminaBarTex2DPos = { m_2DPos.x - m_staminaBarTexLength.x / 2.0f * m_siz ,m_2DPos.y };
		}

	//パーセント(0.0f～1.0f)
	// _percent ... スタミナの割合(0〜1)
	void SetStaminaBarTexPercent(float _percent);

	//大きさの倍率をセット
	// _siz ... 倍率
	void SetSiz(float _siz) { m_siz = _siz; }

	//消すときの処理(今は何もしない)
	void Delete();
private:

	//サイズ
	float m_siz;

	/////////////////////////////////////////////
	//HPバー背景
	std::shared_ptr<KdTexture>m_spStaminaBarBackTex = nullptr;
	//背景の描画位置
	Math::Vector2 m_staminaBarTexBack2DPos = Math::Vector2::Zero;

	//画像の立て横の長さ
	Math::Vector2 m_staminaBarTexBackLength = Math::Vector2::Zero;

	//画像の切り取り範囲
	Math::Rectangle m_staminaBarTexBackRect;

	/////////////////////////////////////////////
	//HPバー本体
	std::shared_ptr<KdTexture>m_spStaminaBarTex = nullptr;
	//本体の描画位置
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

