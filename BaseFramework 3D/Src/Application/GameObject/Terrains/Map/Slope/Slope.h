#pragma once

#include"../MapBase.h"

class Slope :public MapBase
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	Slope() {};

	//デストラクタ
	~Slope() override {};

	//---- 基本の処理 ----

	//初期化
	void Init()override;

	//更新
	void Update()override;
};
