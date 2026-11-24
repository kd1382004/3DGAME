//---- 自分のヘッダー ----
#include "CameraBase.h"							//カメラの基底クラス

//---- 便利機能 ----
#include"../../Info/MouseInfo/MouseInfo.h"		//マウス入力

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

void CameraBase::PostUpdate()
{
	ShakeUpdate();
}

void CameraBase::PreDraw()
{
	if (!m_spCamera) { return; }

	// 画面に渡す用のコピー（m_mWorld 自体は変えない）
	Math::Matrix mWorld = m_mWorld;

	// 揺れの分だけ、カメラの右方向・上方向にずらす
	if (m_shakeOffset != Math::Vector3::Zero)
	{
		Math::Vector3 right = mWorld.Right();
		Math::Vector3 up = mWorld.Up();
		right.Normalize();
		up.Normalize();

		mWorld.Translation(mWorld.Translation() + right * m_shakeOffset.x + up * m_shakeOffset.y);
	}

	m_spCamera->SetCameraMatrix(mWorld);
	m_spCamera->SetToShader();
}

void CameraBase::SetTarget(const std::shared_ptr<KdGameObject>& _spTarget)
{
	if (!_spTarget) { return; }

	m_wpTarget = _spTarget;
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

bool CameraBase::RotateTowardsY(float _targetYawDeg, float _maxSpeedDegPerSec)
{
	bool isRotateFinished = false;


	// 目標角度と現在角度の差分
	float diff = _targetYawDeg - m_degAng.y;

	// 許容誤差（度数法）
	float toleranceDeg = 0.1f;

	// -180度 〜 +180度の範囲に正規化（最短ルートで旋回）
	while (diff > 180.0f)  diff -= 360.0f;
	while (diff < -180.0f) diff += 360.0f;

	// 1フレームあたりの最大回転量
	float maxStep = _maxSpeedDegPerSec * DeltaTime::Instance().GetGameDeltaTime();

	// 残りが許容範囲内、または今フレームで届く → ぴったり合わせて完了
	if (std::abs(diff) <= std::max(maxStep, toleranceDeg))
	{
		m_degAng.y += diff;   // 目標にスナップ（誤差を残さない）
		isRotateFinished = true;
	}
	else
	{
		m_degAng.y += (diff > 0.0f) ? maxStep : -maxStep;
		isRotateFinished = false;
	}

	// 0 〜 360度に収める
	if (m_degAng.y >= 360.0f) m_degAng.y -= 360.0f;
	if (m_degAng.y < 0.0f)    m_degAng.y += 360.0f;

	return isRotateFinished;
}

void CameraBase::Shake(float _time, float _power)
{
	// 今の揺れより弱いなら、上書きしない
	if (m_shakeTimer > 0.0f && _power < m_shakePower) { return; }

	m_shakeTimer = _time;
	m_shakePower = _power;
	m_shakeTime = _time;
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
	POINT nowPos;
	GetCursorPos(&nowPos);

	POINT mouseMove{};
	mouseMove.x = nowPos.x - MouseInfo::Instance().GetWindowMousePos().x;
	mouseMove.y = nowPos.y - MouseInfo::Instance().GetWindowMousePos().y;

	MouseInfo::Instance().SetMousePosFixMousePos();

	// 実際にカメラを回転させる処理(0.15はただの補正値)
	m_degAng.x += mouseMove.y * 0.15f;
	m_degAng.y += mouseMove.x * 0.15f;

	// 回転制御
	m_degAng.x = std::clamp(m_degAng.x, -45.f, 45.f);
}

void CameraBase::ShakeUpdate()
{
	// 揺れていないときは、ずらさない
	if (m_shakeTimer <= 0.0f)
	{
		m_shakeOffset = Math::Vector3::Zero;
		return;
	}

	// 残り時間を減らす（ヒットストップ中も揺れるように、実際の時間で減らす）
	m_shakeTimer -= DeltaTime::Instance().GetRealDeltaTime();
	if (m_shakeTimer <= 0.0f)
	{
		m_shakeTimer = 0.0f;
		m_shakeOffset = Math::Vector3::Zero;
		return;
	}

	//---------- 今の強さ ----------
	// 残りの割合（揺らし始め = 1 → 終わり = 0）
	float rate = 0.0f;
	if (m_shakeTime > 0.0f)
	{
		rate = std::clamp(m_shakeTimer / m_shakeTime, 0.0f, 1.0f);
	}

	// 2乗して、最初は強く・終わりはすっと収まるようにする
	const float nowPower = m_shakePower * rate * rate;

	//---------- ずらす量 ----------
	// -1 〜 1 のランダムな値
	static std::mt19937 randEngine{ std::random_device{}() };
	std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

	m_shakeOffset.x = dist(randEngine) * nowPower;			// 横揺れ
	m_shakeOffset.y = dist(randEngine) * nowPower * 0.7f;	// 縦揺れは少し弱く
	m_shakeOffset.z = 0.0f;
}
