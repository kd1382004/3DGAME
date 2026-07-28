#pragma once

class CameraBase : public KdGameObject
{
public:
	CameraBase() {}
	virtual ~CameraBase()	override {}

	void Init()				override;
	void PreDraw()			override;

	void SetTarget(const std::shared_ptr<KdGameObject>& target);

	// 「絶対変更しません！見るだけ！」な書き方
	const std::shared_ptr<KdCamera>& GetCamera() const
	{
		return m_spCamera;
	}

	// 「中身弄るかもね」な書き方
	std::shared_ptr<KdCamera> WorkCamera() const
	{
		return m_spCamera;
	}

	const Math::Matrix GetRotationMatrix()const
	{
		return Math::Matrix::CreateFromYawPitchRoll(
			DirectX::XMConvertToRadians(m_DegAng.y),
			DirectX::XMConvertToRadians(m_DegAng.x),
			DirectX::XMConvertToRadians(m_DegAng.z));
	}

	const Math::Matrix GetRotationYMatrix() const
	{
		return Math::Matrix::CreateRotationY(
			DirectX::XMConvertToRadians(m_DegAng.y));
	}

	void RegistHitObject(const std::shared_ptr<KdGameObject>& object)
	{
		m_wpHitObjectList.push_back(object);
	}

	DirectX::BoundingFrustum GetBoundingFrustum()
	{
		DirectX::BoundingFrustum worldFrustum;

		// プロジェクション行列からフラスタムを作成
		DirectX::BoundingFrustum localFrustum;
		DirectX::BoundingFrustum::CreateFromMatrix(localFrustum, m_spCamera->GetProjMatrix());

		// View の逆行列でワールド空間へ
		DirectX::XMMATRIX invView = DirectX::XMMatrixInverse(nullptr, m_spCamera->GetCameraViewMatrix());
		localFrustum.Transform(worldFrustum, invView);

		return worldFrustum;
	}


	//自分追加
	//ターゲットとカメラの間にオブジェクトがあるかどうか
	void ResolveCameraOcclusion();

	//ターゲットとカメラの間にあるかどうか確認したいものを入れる
	void ResolveCameraOcclusionObject(const std::shared_ptr<KdGameObject>& object)
	{
		m_cameraOcclusionObjects.push_back(object);
	}

protected:
	// カメラ回転用角度
	Math::Vector3								m_DegAng = Math::Vector3::Zero;

	void UpdateRotateByMouse();

	std::shared_ptr<KdCamera>					m_spCamera = nullptr;
	std::weak_ptr<KdGameObject>					m_wpTarget;
	std::vector<std::weak_ptr<KdGameObject>>	m_wpHitObjectList{};

	Math::Matrix								m_mLocalPos = Math::Matrix::Identity;
	Math::Matrix								m_mRotation = Math::Matrix::Identity;

	//カメラの基準点・注視点
	Math::Vector3 m_cameraPos = { 0,0,0 };


	//自分追加
	//ターゲットとカメラの間にあるかどうか確認したいものを入れる
	std::list<std::weak_ptr<KdGameObject>>	m_cameraOcclusionObjects;


	//当たり判定をする半径(この円ないならあたり判定)
	float m_detectRange;

	//ターゲットの1フレーム前の座標
	Math::Vector3 m_targetOldPos;
};