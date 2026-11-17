#pragma once
#include"../WarpGateBase.h"

class NextStageGate :public WarpGateBase
{
public:
	//コンストラクタ
	NextStageGate() {};
	//デストラクタ
	~NextStageGate()override;

	//初期化
	void Init()override;
	//更新(乗ったら次の階へ)
	void Update()override;
private:

};
