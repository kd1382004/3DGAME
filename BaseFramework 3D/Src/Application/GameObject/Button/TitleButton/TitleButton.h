#pragma once
#include"../ButtonBase.h"

//タイトルシーンへ戻るボタン
class TitleButton :public ButtonBase
{
public:

	//---- 基本の処理 ----

	//初期化
	void Init()override;

	//更新(押されたらタイトルシーンへ)
	void Update()override;
};
