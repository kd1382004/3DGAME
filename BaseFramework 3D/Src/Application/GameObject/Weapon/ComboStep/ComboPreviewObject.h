#pragma once

//=====================================================
// コンボエディターのプレビュー用モデル
//  ・ゲームの世界には描かず、専用のテクスチャ（レンダーターゲット）に描く
//  ・そのテクスチャを ImGui::Image でプレビュー用ウィンドウに貼る
//  ・シーンの m_objList に入れておくと、PreDraw で毎フレーム描き直す
//  ・アニメーションの時間はエディターが直接指定する（自分では進めない）
//  ・エディターが毎フレーム KeepAlive() を呼んでいる間だけ描く
//=====================================================
class ComboPreviewObject : public KdGameObject
{
public:

	// 映すモデル（nullptr で消す）
	void SetModel(const std::shared_ptr<KdModelData>& _spData)
	{
		m_spModel = _spData ? std::make_shared<KdModelWork>(_spData) : nullptr;
	}

	// 手に持たせる武器のモデル（空のパスで外す）。読み込めなければ false
	bool SetWeaponModel(const std::string& _path)
	{
		m_spWeapon = nullptr;
		if (_path.empty()) { return true; }

		// ファイルがないと、モデルの読み込み（テクスチャなど）がアサートで止まるので先に確かめる
		std::error_code ec;
		if (!std::filesystem::is_regular_file(_path, ec)) { return false; }

		auto spWeapon = std::make_shared<KdModelData>();
		if (!spWeapon->Load(_path)) { return false; }

		m_spWeapon = spWeapon;

		// 軌跡を出す切っ先の位置（Sword::Init と同じく "SwordTip" ノード。なければ手から 0.6m 先）
		m_tipLocal = { 0.0f, 0.6f, 0.0f };
		if (KdModelData::Node* pTip = spWeapon->FindNode("SwordTip"))
		{
			m_tipLocal = pTip->m_localTransform.Translation();
		}
		return true;
	}

	// ゲームのカメラ（プレビューを描いた後、シェーダーのカメラをこれに戻す）
	void SetMainCamera(const std::shared_ptr<KdGameObject>& _spCamera) { m_wpMainCamera = _spCamera; }

	// 軌跡を出す区間（trailStart〜trailEnd）にいるか
	void SetTrailActive(bool _active) { m_trailActive = _active; }

	// 当たり判定が出ている間は赤っぽく描く
	void SetHitActive(bool _active) { m_hitActive = _active; }

	// エディターが毎フレーム呼ぶ。呼ばれなくなったら数フレームで描くのをやめる
	void KeepAlive() { m_aliveFrames = 3; }

	// 描くテクスチャの大きさ（ピクセル）。ウィンドウの大きさに合わせて毎フレーム呼んでよい
	void SetViewSize(int _w, int _h)
	{
		m_reqW = std::max(_w, 16);
		m_reqH = std::max(_h, 16);
	}

	// ImGui::Image に渡すテクスチャ（まだ描いていなければ nullptr）
	ImTextureID GetTextureID() const
	{
		if (!m_rtPack.m_RTTexture) { return ImTextureID{}; }
		return (ImTextureID)m_rtPack.m_RTTexture->WorkSRView();
	}

	//==============================
	// 見た目の設定（エディターから触る）
	//==============================
	float         m_camYaw = 180.0f;    // カメラの水平角度（度）
	float         m_camPitch = 10.0f;   // カメラの見下ろし角度（度）
	float         m_camDistance = 3.0f; // カメラとの距離（m）
	float         m_camHeight = 0.9f;   // 注視する高さ（m）
	Math::Color   m_bgColor = { 0.35f, 0.38f, 0.42f, 1.0f }; // 背景色
	float         m_brightness = 1.5f;  // 明るさ（色に掛ける）
	float         m_emissive = 0.25f;   // 自己発光（暗いダンジョンの光でも見えるように）

	bool          m_showTrail = true;   // 軌跡を出すか

	// 武器を付けるノードと、そこからの回転（PlayerBase::Update と同じ値）
	std::string   m_weaponNodeName = "Weapon.R";
	Math::Vector3 m_weaponRot = { 0.0f, 0.0f, 70.0f };   // 度（x=ピッチ, y=ヨー, z=ロール）

	void ResetCamera()
	{
		m_camYaw = 180.0f; m_camPitch = 10.0f; m_camDistance = 3.0f; m_camHeight = 0.9f;
	}

	// アニメーション _animeName の _time（フレーム）の姿勢にする
	void SetPose(const std::string& _animeName, float _time)
	{
		if (!m_spModel || !m_spModel->GetData()) { return; }

		// 軌跡用：段（アニメーション）が変わったら軌跡を消す。時間が動いたときだけ点を足す
		if (_animeName != m_lastPoseAnime || _time < m_lastPoseTime)
		{
			m_trailClearRequest = true;
		}
		if (_animeName != m_lastPoseAnime || _time != m_lastPoseTime)
		{
			m_poseChanged = true;
		}
		m_lastPoseAnime = _animeName;
		m_lastPoseTime = _time;

		const std::shared_ptr<KdAnimationData> spAnime = m_spModel->GetData()->GetAnimation(_animeName);
		if (!spAnime) { return; }

		const float time = std::clamp(_time, 0.0f, spAnime->m_maxLength);

		// KdAnimator::AdvanceTime と同じく、ノードごとに補間した行列を入れる
		std::vector<KdModelWork::Node>& nodes = m_spModel->WorkNodes();
		for (KdAnimationData::Node& animeNode : spAnime->m_nodes)
		{
			const int idx = animeNode.m_nodeOffset;
			if (idx < 0 || idx >= static_cast<int>(nodes.size())) { continue; }

			animeNode.Interpolate(nodes[idx].m_localTransform, time);
		}

		m_spModel->CalcNodeMatrices();
	}

	void Update() override
	{
		if (m_aliveFrames > 0) { --m_aliveFrames; }
	}

	// 本番の描画（Draw）より前に、プレビュー用テクスチャへ描いておく
	void PreDraw() override
	{
		if (!m_spModel || m_aliveFrames <= 0) { return; }

		// 大きさが変わったらテクスチャを作り直す
		if (!m_rtPack.m_RTTexture || m_rtW != m_reqW || m_rtH != m_reqH)
		{
			m_rtW = m_reqW;
			m_rtH = m_reqH;
			m_rtPack = KdRenderTargetPack();
			m_rtPack.CreateRenderTarget(m_rtW, m_rtH, true);
		}

		// プレビュー用のカメラ（注視点のまわりを回る）
		const Math::Vector3 target = { 0.0f, m_camHeight, 0.0f };
		const Math::Matrix mCam =
			Math::Matrix::CreateTranslation(0.0f, 0.0f, -m_camDistance)
			* Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(m_camPitch))
			* Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_camYaw))
			* Math::Matrix::CreateTranslation(target);
		m_camera.SetProjectionMatrix(45.0f, 100.0f, 0.01f, static_cast<float>(m_rtW) / static_cast<float>(m_rtH));
		m_camera.SetCameraMatrix(mCam);

		// 描き先をプレビュー用テクスチャに変える（デストラクタ・Undo で元に戻る）
		KdRenderTargetChanger changer;
		changer.ChangeRenderTarget(m_rtPack);
		m_rtPack.ClearTexture(m_bgColor);

		m_camera.SetToShader();

		const float b = m_brightness;
		const Math::Color col = m_hitActive ? Math::Color(b, b * 0.45f, b * 0.45f, 1.0f) : Math::Color(b, b, b, 1.0f);
		const Math::Vector3 emissive = { m_emissive, m_emissive, m_emissive };

		KdShaderManager::Instance().m_StandardShader.BeginLit();
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, Math::Matrix::Identity, col, emissive);

		// 武器：手のノードの行列に、ゲームと同じ回転を掛けた位置に描く
		//  （PlayerBase の m_weponParentMatrix = rMat * handNode->m_worldTransform * modelWorld と同じ。
		//    プレビューではモデルを原点に置いているので modelWorld は単位行列）
		if (const KdModelWork::Node* pHand = m_spModel->FindNode(m_weaponNodeName))
		{
			const Math::Matrix rMat = Math::Matrix::CreateFromYawPitchRoll(
				DirectX::XMConvertToRadians(m_weaponRot.y),
				DirectX::XMConvertToRadians(m_weaponRot.x),
				DirectX::XMConvertToRadians(m_weaponRot.z));
			const Math::Matrix weaponMat = rMat * pHand->m_worldTransform;

			if (m_spWeapon)
			{
				KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spWeapon, weaponMat, col, emissive);
			}

			// 軌跡：ゲーム（Sword::Update）と同じく、切っ先の位置を毎フレーム足していく
			//  ビルボードの向きは「点を足したときのカメラ」で決まるので、プレビューのカメラを設定した後で足す
			UpdateTrail(Math::Vector3::Transform(m_tipLocal, weaponMat));
		}

		// 軌跡を描く（Sword::DrawLit と同じく Lit の中で描く）
		if (m_showTrail && m_trailActive && m_spTrail && m_spTrail->GetNumPoints() >= 2)
		{
			KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_spTrail);
		}
		KdShaderManager::Instance().m_StandardShader.EndLit();

		changer.UndoRenderTarget();

		// シェーダーのカメラをゲームのカメラに戻す
		if (auto spMainCamera = m_wpMainCamera.lock())
		{
			spMainCamera->PreDraw();
		}
	}

private:

	std::shared_ptr<KdModelWork> m_spModel = nullptr;
	std::shared_ptr<KdModelData> m_spWeapon = nullptr;   // 手に持たせる武器（アニメーションなし）
	std::weak_ptr<KdGameObject>  m_wpMainCamera;
	bool                         m_hitActive = false;
	int                          m_aliveFrames = 0;

	//==============================
	// 軌跡
	//==============================

	// 切っ先の位置を軌跡に足す（時間が動いたときだけ）
	void UpdateTrail(const Math::Vector3& _tipPos)
	{
		if (!m_spTrail)
		{
			// Sword::Init と同じ設定
			m_spTrail = std::make_shared<KdTrailPolygon>();
			m_spTrail->SetMaterial("Asset/Textures/Weapon/Daerre/jet.png");
			m_spTrail->SetPattern(KdTrailPolygon::Trail_Pattern::eBillboard);
		}

		// 軌跡が始まった・途切れたら、それまでの点を全部消す
		//（前の軌跡の最後の点と、次の軌跡の最初の点がつながらないように）
		if (m_trailActive != m_prevTrailActive)
		{
			m_trailClearRequest = true;
			m_prevTrailActive = m_trailActive;
		}

		if (m_trailClearRequest)
		{
			m_spTrail->ClearPoints();
			m_trailClearRequest = false;
		}

		// 点を足すのは、軌跡を出す区間にいて、時間が動いたときだけ
		if (m_poseChanged && m_trailActive)
		{
			m_spTrail->AddPoint(Math::Matrix::CreateTranslation(_tipPos));
		}
		m_poseChanged = false;
	}

	std::shared_ptr<KdTrailPolygon> m_spTrail = nullptr;
	Math::Vector3 m_tipLocal = { 0.0f, 0.6f, 0.0f };   // 武器のローカルでの切っ先の位置
	bool          m_trailActive = false;
	bool          m_prevTrailActive = false;   // 前のフレームの m_trailActive（始まり・途切れを見つける用）
	bool          m_trailClearRequest = false;
	bool          m_poseChanged = false;
	std::string   m_lastPoseAnime;
	float         m_lastPoseTime = -1.0f;

	KdCamera           m_camera;
	KdRenderTargetPack m_rtPack;
	int                m_rtW = 0, m_rtH = 0;     // 今のテクスチャの大きさ
	int                m_reqW = 480, m_reqH = 480; // 欲しい大きさ
};
