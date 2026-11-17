#pragma once
#include"../ButtonBase.h"

class TitleButton :public ButtonBase
{
public:

	//初期化
	void Init()override;
	//更新(押されたらタイトルシーンへ)
	void Update()override;

private:


};
