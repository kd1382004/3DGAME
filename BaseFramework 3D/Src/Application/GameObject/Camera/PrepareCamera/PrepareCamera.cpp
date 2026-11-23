#include "PrepareCamera.h"

void PrepareCamera::MoveTo(const std::string& _name, bool _immediate)
{
	if (!m_cams.contains(_name)) { return; }

	// 今いる場所から動き出す(移動中に切り替えても飛ばない)
	m_from = m_now;
	m_toName = _name;
	m_moveTimer = 0.0f;

	if (_immediate)
	{
		m_moveTimer = m_moveTime;
		m_now = m_cams[_name];
		ApplyNow();
	}
}

void PrepareCamera::Update()
{
	CameraBase::Update();

	if (!m_cams.contains(m_toName)) { return; }
	const CampCameraData& to = m_cams[m_toName];

	// 準備画面はゲームの時間が止まっていても動かしたいので RealDeltaTime
	m_moveTimer = std::min(m_moveTimer + DeltaTime::Instance().GetRealDeltaTime(), m_moveTime);

	// 0〜1 の割合。ゆっくり始まってゆっくり止まるようにする
	float t = m_moveTimer / m_moveTime;
	t = t * t * (3.0f - 2.0f * t);

	m_now.m_pos = Math::Vector3::Lerp(m_from.m_pos, to.m_pos, t);
	m_now.m_target = Math::Vector3::Lerp(m_from.m_target, to.m_target, t);
	m_now.m_fovY = std::lerp(m_from.m_fovY, to.m_fovY, t);

	ApplyNow();
}

void PrepareCamera::ApplyNow()
{
	if (!m_spCamera) { return; }

	// 左手系の「見る行列」を作って、その逆行列をカメラのワールド行列にする
	// 画面への反映は CameraBase::PreDraw が m_mWorld を使ってやってくれる
	Math::Matrix view = DirectX::XMMatrixLookAtLH(m_now.m_pos, m_now.m_target, Math::Vector3::Up);
	m_mWorld = view.Invert();

	// 縦の画角(度)
	m_spCamera->SetProjectionMatrix(m_now.m_fovY);
}