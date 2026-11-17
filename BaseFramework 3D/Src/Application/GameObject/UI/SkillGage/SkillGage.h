#pragma once

#include"../UIBase.h"
class SkillGage :public UIBase
{
public:
	//コンストラクタ
	SkillGage() {};
	//デストラクタ
	~SkillGage() {};

	//初期化
	void Init()override;
	//更新
	void Update()override;
	//2Dの描画
	void DrawSprite()override;

	//ゲージの割合をセット
	// _percent ... 割合(0〜1)
	void SetGaugePercent(float _percent) { m_gaugePercent = _percent; }
private:


	//サイズ
	Math::Vector2 m_siz;

	//ゲージの画像
	std::shared_ptr<KdTexture> m_spGaugeTexture = nullptr;
	//枠の画像
	std::shared_ptr<KdTexture> m_spFrameTexture = nullptr;

	//ゲージの割合(0〜1)
	float m_gaugePercent;

};
