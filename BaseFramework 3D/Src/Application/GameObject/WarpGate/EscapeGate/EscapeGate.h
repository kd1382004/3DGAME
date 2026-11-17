#pragma once
#include"../WarpGateBase.h"

class EscapeGate:public WarpGateBase
{
public:
	//コンストラクタ
	EscapeGate() {};
	//デストラクタ
	~EscapeGate()override;

	//初期化
	void Init()override;
	//更新(乗ったら脱出)
	void Update()override;
private:

};
