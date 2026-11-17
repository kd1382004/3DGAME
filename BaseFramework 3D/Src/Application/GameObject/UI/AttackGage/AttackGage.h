#pragma once
#include"../UIBase.h"

class AttackGage :public UIBase
{
public:
	//コンストラクタ
	AttackGage() {};
	//デストラクタ
	~AttackGage()override {};

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
		m_attackGaugeBackTex2DPos = { m_2DPos.x - m_attackGaugeBackTexLength.x / 2.0f * m_siz ,m_2DPos.y };
		m_attackGaugeTex2DPos = { m_2DPos.x - m_attackGaugeTexLength.x / 2.0f * m_siz ,m_2DPos.y };
	}

	//パーセント(0.0f～1.0f)
	// _percent ... ゲージの割合(0〜1)
	void SetAttackGaugeTexPercent(float _percent);

	//表示するかをセット
	// _flg ... 表示するか
	void SetDrawFlg(bool _flg) { m_drawFlg = _flg; }

	//大きさの倍率をセット
	// _siz ... 倍率
	void SetSiz(float _siz) { m_siz = _siz; }

	//消すときの処理(今は何もしない)
	void Delete();

private:

	//表示するか
	bool m_drawFlg = false;

	//大きさの倍率
	float m_siz = 1;

	//攻撃ゲージ
	std::shared_ptr<KdTexture>m_spAttackGaugeBackTex = nullptr;
	//攻撃ゲージ本体の画像
	std::shared_ptr<KdTexture>m_spAttackGaugeTex = nullptr;
	//背景の描画位置
	Math::Vector2 m_attackGaugeBackTex2DPos = Math::Vector2::Zero;

	//画像の立て横の長さ
	Math::Vector2 m_attackGaugeBackTexLength = Math::Vector2::Zero;

	//画像の切り取り範囲
	Math::Rectangle m_attackGaugeBackTexRect;

	//攻撃ゲージ
	std::shared_ptr<KdTexture>m_spAttackGaugeTex2 = nullptr;
	//本体の描画位置
	Math::Vector2 m_attackGaugeTex2DPos = Math::Vector2::Zero;

	//画像の立て横の長さ
	Math::Vector2 m_attackGaugeTexLength = Math::Vector2::Zero;

	//画像の切り取り範囲
	Math::Rectangle m_attackGaugeTexRect;

	//ゲージの割合(0〜1)
	float m_attackGaugeTexPercent;
};
