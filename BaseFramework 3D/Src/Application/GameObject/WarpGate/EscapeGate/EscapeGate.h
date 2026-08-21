#pragma once
#include"../WarpGateBase.h"

class EscapeGate:public WarpGateBase
{
public:
	EscapeGate() {};
	~EscapeGate()override {};

	void Init()override;
	void Update()override;
private:

};
