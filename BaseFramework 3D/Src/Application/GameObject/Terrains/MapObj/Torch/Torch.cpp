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
}

void Torch::PreDraw()
{
	//カメラに映ってるかどうか
	std::shared_ptr<CameraBase>spCamera = m_wpCamera.lock();
	if (spCamera)
	{
		m_isInView = CheckInScreen(spCamera->GetBoundingFrustum(), m_frustumBox);
	}
}

void Torch::DrawLit()
{
	if (!m_spTorchModel) { return; }

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
