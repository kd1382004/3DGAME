#pragma once

class EffectBase:public KdGameObject
{
public:
	//コンストラクタ
	EffectBase() {};
	//デストラクタ
	~EffectBase() override{};

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

private:

};
