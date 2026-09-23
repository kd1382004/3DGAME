#include "TPSCamera.h"

void TPSCamera::Init()
{
	// 親クラスの初期化呼び出し
	CameraBase::Init();

	// 注視点
	m_cameraDefaultPos = { 0, 3.5f, -10.0f };
	m_cameraPos = m_cameraDefaultPos;
	m_mLocalPos = Math::Matrix::CreateTranslation(m_cameraPos);


	m_cameraEvasionPos = { 0,2.5,-4 };

	m_ImGUIName = "TPSCamera";
}

void TPSCamera::PostUpdate()
{
	// ターゲットの行列(有効な場合利用する)
	Math::Matrix								_targetMat = Math::Matrix::Identity;
	const std::shared_ptr<const KdGameObject>	_spTarget = m_wpTarget.lock();
	if (_spTarget)
	{
		_targetMat = Math::Matrix::CreateTranslation(_spTarget->GetPos());
	}

	m_mLocalPos = Math::Matrix::CreateTranslation(m_cameraPos);

	// カメラの回転
	UpdateRotateByMouse();
	m_mRotation = GetRotationMatrix();
	m_mWorld = m_mLocalPos * m_mRotation * _targetMat;

	//// ↓めり込み防止の為の座標補正計算↓
	ResolveCameraOcclusion();
}
