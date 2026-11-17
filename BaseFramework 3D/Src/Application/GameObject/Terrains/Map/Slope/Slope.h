#pragma once

#include"../MapBase.h"

class Slope :public MapBase
{
public:
	//コンストラクタ
	Slope() {};
	//デストラクタ
	~Slope() override {};

	//初期化
	void Init()override;
	//更新
	void Update()override;

private:

};
