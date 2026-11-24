#pragma once
#include"../WarpGateBase.h"

//脱出ゲート
class EscapeGate:public WarpGateBase
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	EscapeGate() {};

	//デストラクタ
	~EscapeGate()override;

	//---- 基本の処理 ----

	//初期化
	void Init()override;

	//更新(乗ったら脱出)
	void Update()override;
};
