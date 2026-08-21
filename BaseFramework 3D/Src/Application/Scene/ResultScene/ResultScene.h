#pragma once

#include"../BaseScene/BaseScene.h"


class ResultScene : public BaseScene
{
public:

	ResultScene() {}
	~ResultScene() {}

	void ImGUi()override;
	void Init()  override;
private:

	void Event() override;

};
