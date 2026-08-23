#pragma once
#include"../../UIBase.h"

class ResultDisplay:public UIBase
{
public:
	ResultDisplay();
	~ResultDisplay();

	void Init()override;

	void Update()override;

	void PreDraw()override;
	void DrawSprite()override;

private:

};
