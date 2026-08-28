#pragma once

#include"../MapBase.h"

class Slope :public MapBase
{
public:
	Slope() {};
	~Slope() override {};

	void Init()override;
	void Update()override;

private:

};
