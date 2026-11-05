#pragma once

class ResultDisplayBase:public KdGameObject
{
public:
	ResultDisplayBase() {}
	~ResultDisplayBase() {}


	virtual void Init()override {}

	virtual void Update()override {}

	virtual void PreDraw()override {}
	virtual void DrawSprite()override {}

private:

};
