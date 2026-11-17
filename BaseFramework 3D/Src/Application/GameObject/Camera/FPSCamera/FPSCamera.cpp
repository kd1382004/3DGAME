#include "FPSCamera.h"

void FPSCamera::Init()
{
	// 親クラスの初期化呼び出し
	CameraBase::Init();

	// 基準点(ターゲット)の目線
	m_cameraPos = { 0, 1.5f, 0.0f };
	m_mLocalPos = Math::Matrix::CreateTranslation(m_cameraPos);

	m_ImGUIName = "FPSCamera";
}

void FPSCamera::PostUpdate()
{
	// ターゲットの行列(有効な場合利用する)
	Math::Matrix								targetMat	= Math::Matrix::Identity;
	const std::shared_ptr<const KdGameObject>	spTarget	= m_wpTarget.lock();	
	if (spTarget)
	{
		targetMat = Math::Matrix::CreateTranslation(spTarget->GetPos());
	}

	// カメラの回転
	UpdateRotateByMouse();
	m_mRotation = GetRotationMatrix();
	m_mWorld	= m_mRotation * m_mLocalPos * targetMat;
}
