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

void CameraBase::Update()
{
	if (m_isEvasionCam)
	{
		EvasionCamera();
	}
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

	Math::Vector3 targetPos = spTarget->GetPos() + Math::Vector3(0, 1.0f, 0);
	Math::Vector3 camPos = GetPos();
	// 視線エリアの最小・最大座標
	Math::Vector3 minPos = Math::Vector3::Min(targetPos, camPos);
	Math::Vector3 maxPos = Math::Vector3::Max(targetPos, camPos);
	constexpr float margin = 10.0f;

	KdCollider::RayInfo rayInfo;

	rayInfo.m_pos = spTarget->GetPos() + Math::Vector3(0, 1, 0);
	rayInfo.m_dir = camPos - rayInfo.m_pos;
	rayInfo.m_range = rayInfo.m_dir.Length();
	rayInfo.m_dir.Normalize();
	rayInfo.m_type = KdCollider::TypeCameraOcclusion;

	float maxOverLap = 0;
	Math::Vector3 hitPos = {};
	bool hit = false;

	m_detectRange = (camPos - spTarget->GetPos()).Length() + 1.0f;
	if (rayInfo.m_dir.LengthSquared() == 0.0f) { return; }
	for (auto& wpObj : m_cameraOcclusionObjects)
	{
		if (auto spObj = wpObj.lock())
		{
			Math::Vector3 objPos = spObj->GetPos();

			// 視線エリアの外にあるオブジェクトは Raycast 前にスキップ
			if (objPos.x < minPos.x - margin || objPos.x > maxPos.x + margin ||
				objPos.z < minPos.z - margin || objPos.z > maxPos.z + margin)
			{
				continue; // 高速スキップ
			}

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

void CameraBase::SetEvasionCamera(float _time)
{
	m_evasionCamTime = _time;
	m_isEvasionCam = true;
	m_evasionCamTimer = 0;
}

void CameraBase::EvasionCamera()
{
	m_evasionCamTimer += DeltaTime::Instance().GetRealDeltaTime();
	float t = m_evasionCamTimer / m_evasionCamTime;

	// 0〜1 にクランプ
	t = std::clamp(t, 0.0f, 1.0f);

	// 0.0〜0.5：近づく
	if (t < 0.5f)
	{
		float k = t / 0.5f;
		m_cameraPos = Math::Vector3::Lerp(m_cameraDefaultPos, m_cameraEvasionPos, k);

		m_fov = std::lerp(m_defaultFov, m_evasionFov, k);
		m_spCamera->SetProjectionMatrix(m_fov);
	}
	// 0.5〜0.7：近距離維持
	else if (t < 0.7f)
	{
		m_cameraPos = m_cameraEvasionPos;
		m_fov = m_evasionFov;
		m_spCamera->SetProjectionMatrix(m_fov);
	}
	// 0.7〜1.0：ゆっくり戻る
	else
	{
		float k = (t - 0.7f) / 0.3f;
		m_cameraPos = Math::Vector3::Lerp(m_cameraEvasionPos, m_cameraDefaultPos, k);

		// FOV補間
		m_fov = std::lerp(m_evasionFov, m_defaultFov, k);
		m_spCamera->SetProjectionMatrix(m_fov);
	}

	// 終了
	if (t >= 1.0f)
	{
		m_isEvasionCam = false;
		m_cameraPos = m_cameraDefaultPos;
		m_fov = m_defaultFov;
		m_spCamera->SetProjectionMatrix(m_fov);
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



