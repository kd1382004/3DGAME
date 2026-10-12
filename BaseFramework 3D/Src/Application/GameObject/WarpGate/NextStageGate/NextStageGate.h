#pragma once
#include"../WarpGateBase.h"

class NextStageGate :public WarpGateBase
{
public:
	NextStageGate() {};
	~NextStageGate()override;

	void Init()override;
	void Update()override;
private:

};
