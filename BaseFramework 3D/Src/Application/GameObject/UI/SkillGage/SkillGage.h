#pragma once

#include"../UIBase.h"
//スキルゲージ
class SkillGage :public UIBase
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	SkillGage() {};

	//デストラクタ
	~SkillGage() {};

	//---- 基本の処理 ----

	//初期化
	void Init()override;

	//更新
	void Update()override;

	//2Dの描画
	void DrawSprite()override;

	//---- 設定 ----

	//ゲージの割合をセット
	// _percent ... 割合(0〜1)
	void SetGaugePercent(float _percent) { m_gaugePercent = _percent; }

private:

	//---- メンバ変数 ----

	//サイズ
	Math::Vector2 m_size;

	//ゲージの画像
	std::shared_ptr<KdTexture> m_spGaugeTexture = nullptr;

	//枠の画像
	std::shared_ptr<KdTexture> m_spFrameTexture = nullptr;

	//ゲージの割合(0〜1)
	float m_gaugePercent = 0.0f;
};
