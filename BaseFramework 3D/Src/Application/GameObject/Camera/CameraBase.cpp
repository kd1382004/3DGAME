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

	m_spCamera->SetCameraMatrix(m_mWorld);
	m_spCamera->SetToShader();
}

void CameraBase::SetTarget(const std::shared_ptr<KdGameObject>& target)
{
	if (!target) { return; }

	m_wpTarget = target;
}

void CameraBase::ResolveCameraOcclusion()
{
	// あたり判定オブジェクトから期限切れの要素を削除
	auto it = m_cameraOcclusionObjects.begin();
	while (it != m_cameraOcclusionObjects.end())
	{
		if (it->expired())
		{
			it = m_cameraOcclusionObjects.erase(it);
		}
		else
		{
			it++;
		}
	}


	std::shared_ptr<KdGameObject>spTarget = m_wpTarget.lock();

	if (!spTarget) { return; }

	KdCollider::RayInfo rayInfo;

	rayInfo.m_pos = spTarget->GetPos() +Math::Vector3(0,1,0);
	rayInfo.m_dir = GetPos() - rayInfo.m_pos;
	rayInfo.m_range = rayInfo.m_dir.Length();
	rayInfo.m_dir.Normalize();
	rayInfo.m_type = KdCollider::TypeCameraOcclusion;


	float maxOverLap = 0;
	Math::Vector3 hitPos = {};
	bool hit = false;

	m_detectRange = (GetPos() - spTarget->GetPos()).Length() + 1.0f;
	if (rayInfo.m_dir.LengthSquared() == 0.0f) { return; }
	for (auto& wpObj : m_cameraOcclusionObjects)
	{
		if (auto spObj = wpObj.lock())
		{
			std::list<KdCollider::CollisionResult> retRayList;

			spObj->Intersects(rayInfo, &retRayList);


			for (auto& ret : retRayList)
			{
				if (maxOverLap < ret.m_overlapDistance)
				{
					maxOverLap = ret.m_overlapDistance;
					hitPos = ret.m_hitPos;
					hit = true;
				}
			}
		}
	}



	if (hit)
	{
		SetPos(hitPos);
	}
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



