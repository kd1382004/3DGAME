#pragma once

class EffectBase:public KdGameObject
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	EffectBase() {};

	//デストラクタ
	~EffectBase() override{};

	//---- 基本の処理 ----

	//初期化
	void Init()override;

	//更新前の処理
	void PreUpdate()override;

	//更新
	void Update()override;

	//エフェクトの描画
	void DrawEffect()override;

	//2D描画
	void DrawSprite()override;
};
