#pragma once
#include "../CameraBase.h"

class FPSCamera : public CameraBase
{
public:
	//コンストラクタ
	FPSCamera()							{}
	//デストラクタ
	~FPSCamera()			override	{}

	//初期化
	void Init()				override;
	//更新後の処理(一人称視点の位置を決める)
	void PostUpdate()		override;
};