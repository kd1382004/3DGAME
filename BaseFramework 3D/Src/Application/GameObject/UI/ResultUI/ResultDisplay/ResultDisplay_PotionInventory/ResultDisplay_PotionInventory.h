#pragma once
#include"../ResultDisplayBase.h"

class ResultDisplay_PotionInventory :public ResultDisplayBase
{
public:
	ResultDisplay_PotionInventory() {}
	~ResultDisplay_PotionInventory() {}


	void Init();

	void Update();

	void PreDraw();
	void DrawSprite();

private:

};
