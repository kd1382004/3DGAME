#pragma once
#include "../CameraBase.h"

//三人称カメラ
class TPSCamera : public CameraBase
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	TPSCamera()							{}

	//デストラクタ
	~TPSCamera()			override	{}

	//---- 基本の処理 ----

	//初期化
	void Init()				override;

	//更新後の処理(三人称視点の位置を決める)
	void PostUpdate()		override;
};