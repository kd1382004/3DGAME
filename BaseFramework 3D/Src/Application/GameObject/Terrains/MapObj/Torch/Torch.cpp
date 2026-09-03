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


	//m_wpAuraEffect = KdEffekseerManager::GetInstance().Play(
	//	"Light.efkefc",
	//	m_pos,
	//	1.0f,
	//	1.0f,
	//	true // ループ再生
	//);
}

void Torch::PreDraw()
{
	//カメラに映ってるかどうか
	std::shared_ptr<CameraBase>spCamera = m_wpCamera.lock();
	if (spCamera)
	{
		m_isInView = CheckInScreen(spCamera->GetBoundingFrustum(), m_frustumBox);
	}


	//// エフェクトオブジェクトを取得
	//if (auto spEffect = m_wpAuraEffect.lock())
	//{
	//	int handle = spEffect->GetHandle();
	//	if (m_isInView)
	//	{
	//		// 画面内に映っている時：一時停止解除（再生）
	//		KdEffekseerManager::GetInstance().SetPause(handle, false);
	//	}
	//	else
	//	{
	//		// 画面外に出ている時：一時停止（負荷を0にする）
	//		KdEffekseerManager::GetInstance().SetPause(handle, true);
	//	}
	//}
}

void Torch::DrawLit()
{
	if (!m_spTorchModel) { return; }
	if (!m_isInView) { return; }


	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spTorchModel, m_mWorld);
}

void Torch::SetPos(const Math::Vector3& pos)
{
	m_pos = pos;
	m_mWorld.Translation(pos);
}



void Torch::SetRotation(Math::Matrix _rMat)
{
	Math::Vector3 pos = GetPos();
	Math::Vector3 siz = GetScale();

	Math::Matrix tMat = Math::Matrix::CreateTranslation(pos);
	Math::Matrix sMat = Math::Matrix::CreateScale(siz);

	m_mWorld = sMat * _rMat * tMat;
}
