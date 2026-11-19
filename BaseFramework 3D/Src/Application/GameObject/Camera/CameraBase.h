#pragma once

class CameraBase : public KdGameObject
{
public:
	//コンストラクタ
	CameraBase() {}
	//デストラクタ
	virtual ~CameraBase()	override {}

	//初期化
	void Init()				override;
	//更新
	void Update()				override;
	//更新後の処理
	void PostUpdate()			override;
	//描画前の処理(カメラをセット)
	void PreDraw()			override;

	//追いかける相手をセット
	// _spTarget ... 追いかける相手(プレイヤーなど)
	void SetTarget(const std::shared_ptr<KdGameObject>& _spTarget);

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

	//カメラの回転行列を取得
	//戻り値 ... 回転行列
	const Math::Matrix GetRotationMatrix()const
	{
		return Math::Matrix::CreateFromYawPitchRoll(
			DirectX::XMConvertToRadians(m_degAng.y),
			DirectX::XMConvertToRadians(m_degAng.x),
			DirectX::XMConvertToRadians(m_degAng.z));
	}

	//カメラのY軸回転だけの行列を取得
	//戻り値 ... Y軸の回転行列
	const Math::Matrix GetRotationYMatrix() const
	{
		return Math::Matrix::CreateRotationY(
			DirectX::XMConvertToRadians(m_degAng.y));
	}

	//カメラの当たり判定をするオブジェクトを追加
	// _spObject ... 当たり判定をするオブジェクト
	void RegistHitObject(const std::shared_ptr<KdGameObject>& _spObject)
	{
		m_wpHitObjectList.push_back(_spObject);
	}




	//自分追加
	//ターゲットとカメラの間にオブジェクトがあるかどうか
	void ResolveCameraOcclusion();

	//ターゲットとカメラの間にあるかどうか確認したいものを入れる
	// _spObject ... 確認したいオブジェクト
	void ResolveCameraOcclusionObject(const std::shared_ptr<KdGameObject>& _spObject)
	{
		m_cameraOcclusionObjects.push_back(_spObject);
	}

	//カメラに映る範囲(視錐台)をワールド座標で取得
	//戻り値 ... 視錐台
	DirectX::BoundingFrustum GetBoundingFrustum()
	{	
		// プロジェクション行列からフラスタムを作成
		DirectX::BoundingFrustum localFrustum;
		DirectX::BoundingFrustum::CreateFromMatrix(localFrustum, m_spCamera->GetProjMatrix());

		// View の逆行列でワールド空間へ
		DirectX::XMMATRIX invView = DirectX::XMMatrixInverse(nullptr, m_spCamera->GetCameraViewMatrix());
		localFrustum.Transform(m_worldFrustum, invView);
		
		return m_worldFrustum;
	}

	//回避成功したら呼ぶ
	//_time ...　近づけてから戻るまでの時間
	void SetEvasionCamera(float _time);

	// 目標のYaw角度(度数法)に向けて制限付きで回転させる
	// _maxSpeedDegPerSec ... 1秒あたりの最大回転角度
	//向き切ったかどうかを返す
	// _targetYawDeg ... 目標のYaw角度(度)
	bool RotateTowardsY(float _targetYawDeg, float _maxSpeedDegPerSec);


	//カメラとターゲットの間を調べるオブジェクトのリストを空にする
	void ClearCameraOcclusionObject() { m_cameraOcclusionObjects.clear(); }


	// カメラを揺らす
	// _time  : 揺らす秒数（この時間でだんだん弱まって止まる）
	// _power : 揺らす強さ（揺れ幅。目安は 0.05〜0.3 くらい）
	void Shake(float _time, float _power);

	// 追いかけている相手（プレイヤー）の位置。いなければカメラの位置
	Math::Vector3 GetTargetPos() const
	{
		if (auto spTarget = m_wpTarget.lock()) { return spTarget->GetPos(); }
		return GetPos();
	}
protected:


	//ワールド座標での視錐台
	DirectX::BoundingFrustum m_worldFrustum;

	//回避成功
	float m_evasionCamTime = 0.0f;
	//回避成功のカメラの残り時間
	float m_evasionCamTimer = 0.0f;
	//回避成功のカメラ中か
	bool  m_isEvasionCam = false;
	//回避成功のときにカメラを動かす位置
	Math::Vector3 m_cameraEvasionPos = { 0,0,0 };

	//回避成功のカメラの更新
	void EvasionCamera();

	// カメラ回転用角度
	Math::Vector3								m_degAng = Math::Vector3::Zero;

	//マウスでカメラを回転させる
	void UpdateRotateByMouse();

	//カメラ本体
	std::shared_ptr<KdCamera>					m_spCamera = nullptr;
	//追いかける相手
	std::weak_ptr<KdGameObject>					m_wpTarget;
	//カメラの当たり判定をするオブジェクトのリスト
	std::vector<std::weak_ptr<KdGameObject>>	m_wpHitObjectList{};

	//ターゲットからのカメラの位置
	Math::Matrix								m_mLocalPos = Math::Matrix::Identity;
	//カメラの回転
	Math::Matrix								m_mRotation = Math::Matrix::Identity;

	//カメラの基準点・注視点
	Math::Vector3 m_cameraPos = { 0,0,0 };
	//カメラのデフォルトの位置
	Math::Vector3 m_cameraDefaultPos = { 0,0,0 };



	//自分追加
	//ターゲットとカメラの間にあるかどうか確認したいものを入れる
	std::list<std::weak_ptr<KdGameObject>>	m_cameraOcclusionObjects;


	//当たり判定をする半径(この円ないならあたり判定)
	float m_detectRange = 0.0f;

	//ターゲットの1フレーム前の座標
	Math::Vector3 m_targetOldPos;


	//今の視野角(度)
	float m_fov = 60;
	//通常の視野角(度)
	float m_defaultFov = 60;
	//回避成功したときの視野角(度)
	float m_evasionFov = 80;

	//---------- カメラの揺れ ----------
	// 揺れの残り時間（秒）。0 になったら揺れていない
	float m_shakeTimer = 0.0f;

	// 揺らし始めたときの全体の時間（秒）。残り時間との割合で、だんだん弱めるのに使う
	float m_shakeTime = 0.0f;

	// 揺らす強さ（揺れ幅）
	float m_shakePower = 0.0f;

	//カメラの揺れの更新
	void ShakeUpdate();

	// 揺れで今ずらしている量（x：右方向、y：上方向）
	Math::Vector3 m_shakeOffset = Math::Vector3::Zero;
};