#pragma once
#include"../WarpGateBase.h"

//次のステージへのゲート
class NextStageGate :public WarpGateBase
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	NextStageGate() {};

	//デストラクタ
	~NextStageGate()override;

	//---- 基本の処理 ----

	//初期化
	void Init()override;

	//更新(乗ったら次の階へ)
	void Update()override;
};
