#include "CameraBase.h"

#include"../../Info/MouseInfo/MouseInfo.h"

void CameraBase::Init()
{
	if (!m_spCamera)
	{
		m_spCamera = std::make_shared<KdCamera>();
	}

	m_gameObjectClass = KdGameObject::GameObjectClass::GameObjectClass_Camera;
}

void CameraBase::PreDraw()
{
	if (!m_spCamera) { return; }

	//デバック用
	float x=0;
	float z=0;
	if (!m_wpTarget.expired())
	{
		x = m_wpTarget.lock()->GetPos().x;
		z = m_wpTarget.lock()->GetPos().z;
	}


	Math::Matrix tMat = Math::Matrix::CreateTranslation({ x,100,z });
	Math::Matrix rMat = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(90));


	m_mWorld = rMat * tMat;
	m_spCamera->SetCameraMatrix(m_mWorld);
	m_spCamera->SetToShader();
}

void CameraBase::SetTarget(const std::shared_ptr<KdGameObject>& target)
{
	if (!target) { return; }

	m_wpTarget = target;
}

void CameraBase::UpdateRotateByMouse()
{
	if (MouseInfo::Instance().GetMouseFreeFlg()) { return; }
	if (!MouseInfo::Instance().GetFocusWindowFlg()) { return; }


	// マウスでカメラを回転させる処理
	POINT _nowPos;
	GetCursorPos(&_nowPos);

	POINT _mouseMove{};
	_mouseMove.x = _nowPos.x - MouseInfo::Instance().GetWindouMousePos().x;
	_mouseMove.y = _nowPos.y - MouseInfo::Instance().GetWindouMousePos().y;

	MouseInfo::Instance().SetMousePosFixMousePos();

	// 実際にカメラを回転させる処理(0.15はただの補正値)
	m_DegAng.x += _mouseMove.y * 0.15f;
	m_DegAng.y += _mouseMove.x * 0.15f;

	// 回転制御
	m_DegAng.x = std::clamp(m_DegAng.x, -45.f, 45.f);
}



