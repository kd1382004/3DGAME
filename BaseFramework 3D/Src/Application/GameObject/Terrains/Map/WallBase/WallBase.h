#pragma once
#include"../MapBase.h"

//壁
class WallBase:public MapBase
{
public:

	//---- 基本の処理 ----

	//初期化
	void Init()override;

	//更新
	void Update()override;
};
