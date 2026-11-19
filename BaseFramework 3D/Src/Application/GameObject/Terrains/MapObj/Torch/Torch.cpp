#include "Torch.h"

#include"../../../Camera/CameraBase.h"

void Torch::Init()
{
	if (!m_spTorchModel)
	{
		m_spTorchModel = std::make_shared<KdModelWork>();
		m_spTorchModel->SetModelData("Asset/Models/Terrains/MapObj/Torch/Torch.gltf");

		auto spData = m_spTorchModel->GetData();

		if (!spData) { return; }

		const auto& meshNodeIndices = spData->GetDrawMeshNodeIndices();

		if (meshNodeIndices.empty()) { return; }

		// 全メッシュの頂点をまとめる（複数メッシュ対応）
		std::vector<Math::Vector3> allPositions;
		for (int nodeIndex : meshNodeIndices)
		{
			const auto& node = spData->GetOriginalNodes()[nodeIndex];
			if (!node.m_spMesh) { continue; }
			const auto& positions = node.m_spMesh->GetVertexPositions();
			allPositions.insert(allPositions.end(), positions.begin(), positions.end());
		}

		if (allPositions.empty()) { return; }

		// ローカル空間の OBB を生成
		DirectX::BoundingOrientedBox localOBB;
		DirectX::BoundingOrientedBox::CreateFromPoints(
			localOBB,
			allPositions.size(),
			allPositions.data(),
			sizeof(Math::Vector3)
		);

		m_frustumBox = KdCollider::BoxInfo(0, localOBB);
	}

	if (!m_pCollider)
	{
		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("TreasureChest", m_spTorchModel, KdCollider::TypeBump | KdCollider::TypeCameraOcclusion);
	}


	if (!m_spFireSE)
	{

		m_spFireSE = KdAudioManager::Instance().Play3D("Asset/Sound/SE/Torch/Fire.wav", m_pos, false);
		if (m_spFireSE)
		{
			m_spFireSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
			m_spFireSE->Stop();
			m_spFireSE->SetCurveDistanceScaler(20.0f);
		}
	}

	m_effectLocalPos = { 0.0f, 1.0f,-0.7f };


	m_effectLocalColor = { 20,8,0 };

	m_radius = 25;
}

void Torch::PostUpdate()
{



	std::shared_ptr<CameraBase>spCamera = m_wpCamera.lock();
	if (!spCamera) { return; }
	Math::Vector3 worldEffectPos = DirectX::SimpleMath::Vector3::Transform(m_effectLocalPos, m_mWorld);


	m_flickerTime += DeltaTime::Instance().GetGameDeltaTime() * m_flickerSpeed;

	// 周期の違う波を重ねて、規則的に見えないようにする
	float wave = std::sin(m_flickerTime * 7.0f) * 0.6f
		+ std::sin(m_flickerTime * 13.0f + 1.3f) * 0.3f
		+ std::sin(m_flickerTime * 23.0f + 2.7f) * 0.1f; // -1～1の範囲

	float radius = m_radius * (1.0f + wave * m_flickerAmount);

	if (CheckInScreen(spCamera->GetBoundingFrustum(), worldEffectPos, radius))
	{
		// 画面内に映っている時：一時停止解除（再生）
		KdShaderManager::Instance().WorkAmbientController().AddPointLight(
			m_effectLocalColor,								//色
			radius,										//半径	
			worldEffectPos		//座標
		);
	}

	if (m_spFireSE)
	{
		m_spFireSE->SetPos(worldEffectPos);
		m_spFireSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
		if (!m_spFireSE->IsPlaying())
		{
			m_spFireSE->Play();
		}

	}
}

void Torch::PreDraw()
{
	//カメラに映ってるかどうか
	std::shared_ptr<CameraBase>spCamera = m_wpCamera.lock();
	if (spCamera)
	{
		m_isInView = CheckInScreen(spCamera->GetBoundingFrustum(), m_frustumBox);
	}


	// 炎のエフェクト
	// 描画範囲内(出現中・表示中) かつ 画面内 のときだけ出す
	// 描画範囲から出て消え始めたら(FadingOut)止める
	FadeState fadeState = m_fader.GetState();
	bool isDrawRange = (fadeState == FadeState::FadingIn || fadeState == FadeState::Visible);

	if (isDrawRange && m_isInView)
	{
		PlayFireEffect();
	}
	else
	{
		StopFireEffect();
	}
}

void Torch::DrawLit()
{
	if (!m_spTorchModel) { return; }
	if (!m_isInView) { return; }

	DrawModelWithFade(*m_spTorchModel);
}

void Torch::GenerateDepthMapFromLight()
{
	if (!m_spTorchModel) { return; }
	if (!m_isInView) { return; }

	DrawModelWithFade(*m_spTorchModel, false);
}

void Torch::SetPos(const Math::Vector3& _pos)
{
	m_pos = _pos;
	m_mWorld.Translation(_pos);
}

void Torch::SetRotation(Math::Matrix _rMat)
{
	Math::Vector3 pos = GetPos();
	Math::Vector3 siz = GetScale();

	Math::Matrix tMat = Math::Matrix::CreateTranslation(pos);
	Math::Matrix sMat = Math::Matrix::CreateScale(siz);

	m_mWorld = sMat * _rMat * tMat;
}

void Torch::Release()
{
	StopFireEffect();

	if (m_spFireSE)
	{
		m_spFireSE->Stop();
	}
}

void Torch::SetSoundUpdate(bool _flg)
{
	// 鳴らすほうは PostUpdate で再生している(更新チャンク内だけ呼ばれる)
	if (!_flg)
	{
		if (m_spFireSE && m_spFireSE->IsPlaying())
		{
			m_spFireSE->Stop();
		}
	}
}

void Torch::PlayFireEffect()
{
	Math::Vector3 worldEffectPos = DirectX::SimpleMath::Vector3::Transform(m_effectLocalPos, m_mWorld);

	auto spEffect = m_wpAuraEffect.lock();
	if (spEffect && spEffect->IsPlaying())
	{
		// もう出ているなら位置だけ合わせる
		KdEffekseerManager::GetInstance().SetPos(spEffect->GetHandle(), worldEffectPos);
		return;
	}

	// 出ていなければ出す
	m_wpAuraEffect = KdEffekseerManager::GetInstance().Play(
		"Fire/Fire.efkefc", worldEffectPos, 1.0f, 1.0f, true
	);
}

void Torch::StopFireEffect()
{
	if (auto spEffect = m_wpAuraEffect.lock())
	{
		// SetLoop(false) してから StopEffect を呼ぶことで自動再再生を防ぎ、消去＆停止させる
		spEffect->SetLoop(false);
		spEffect->StopEffect();
	}

	// 止めたものは使い回さず、次は新しく出す
	m_wpAuraEffect.reset();
}
