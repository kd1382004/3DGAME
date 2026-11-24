#pragma once

#pragma once
#include"../UIBase.h"

//スタミナゲージ
class StaminaGage :public UIBase
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	StaminaGage() {};

	//デストラクタ
	~StaminaGage()override {};

	//---- 基本の処理 ----

	//初期化
	void Init()override;

	//更新
	void Update()override;

	//描画前の処理
	void PreDraw()override;

	//2Dの描画
	void DrawSprite()override;

	//---- 設定 ----

	//パーセント(0.0f～1.0f)
	// _percent ... スタミナの割合(0〜1)
	void SetStaminaBarTexPercent(float _percent);

	//大きさの倍率をセット
	// _size ... 倍率
	void SetSize(float _size) { m_size = _size; }

	//最大スタミナをセット(目盛りの数を決める)
	// _maxStamina ... 最大スタミナ
	void SetMaxStamina(float _maxStamina) { m_maxStamina = _maxStamina; }

	//---- その他の処理 ----

	//画面上の座標をセット(バーの左端の位置も計算する)
	// _pos ... 画面上の座標(バーの中心)
	void Set2DPos(const Math::Vector2& _pos) {
		m_2DPos = _pos;
		m_staminaBarTexBack2DPos = { m_2DPos.x - m_staminaBarTexBackLength.x / 2.0f * m_size ,m_2DPos.y };
		m_staminaBarTex2DPos = { m_2DPos.x - m_staminaBarTexLength.x / 2.0f * m_size ,m_2DPos.y };
		}

	//消すときの処理(今は何もしない)
	void Delete();

private:

	//---- メンバ変数 ----

	//サイズ
	float m_size = 1.0f;

	//HPバー背景
	std::shared_ptr<KdTexture>m_spStaminaBarBackTex = nullptr;

	//背景の描画位置
	Math::Vector2 m_staminaBarTexBack2DPos = Math::Vector2::Zero;

	//画像の立て横の長さ
	Math::Vector2 m_staminaBarTexBackLength = Math::Vector2::Zero;

	//画像の切り取り範囲
	Math::Rectangle m_staminaBarTexBackRect;

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


	//スタミナバーメモリ
	std::shared_ptr<KdTexture>m_spstaminaMemoryTex = nullptr;

	//1目盛りあたりのスタミナ
	const float m_staminaPerScale = 10.0f;

	//最大スタミナ(目盛りの数を決める)
	float m_maxStamina = 0;
};

