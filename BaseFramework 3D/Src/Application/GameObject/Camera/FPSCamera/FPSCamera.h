#pragma once
#include "../CameraBase.h"

//一人称カメラ
class FPSCamera : public CameraBase
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	FPSCamera()							{}

	//デストラクタ
	~FPSCamera()			override	{}

	//---- 基本の処理 ----

	//初期化
	void Init()				override;

	//更新後の処理(一人称視点の位置を決める)
	void PostUpdate()		override;
};