#pragma once

class ResultDisplayBase:public KdGameObject
{
public:
	//コンストラクタ
	ResultDisplayBase() {}
	//デストラクタ
	~ResultDisplayBase() {}


	//初期化
	virtual void Init()override {}

	//更新
	virtual void Update()override {}

	//描画前の処理
	virtual void PreDraw()override {}
	//2Dの描画
	virtual void DrawSprite()override {}

private:

};
