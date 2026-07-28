#include "MapBase.h"
#include"../../Camera/CameraBase.h"

void MapBase::Init()
{
	if (!m_spModel) { return; }


	//視錐台用のBoxInfo生成
	auto spData = m_spModel->GetData();
	const auto& meshNodeIndices = spData->GetDrawMeshNodeIndices();
	int nodeIndex = meshNodeIndices[0];
	const auto& node = spData->GetOriginalNodes()[nodeIndex];

	auto spMesh = node.m_spMesh;
	const auto& positions = spMesh->GetVertexPositions();
	size_t vertexCount = positions.size();

	DirectX::BoundingOrientedBox localOBB;
	DirectX::BoundingOrientedBox::CreateFromPoints(
		localOBB,
		vertexCount,
		positions.data(),
		sizeof(Math::Vector3)
	);

	Math::Vector3 worldScale;
	worldScale.x = m_mWorld.Right().Length();
	worldScale.y = m_mWorld.Up().Length();
	worldScale.z = m_mWorld.Backward().Length();

	localOBB.Extents.x *= worldScale.x;
	localOBB.Extents.y *= worldScale.y;
	localOBB.Extents.z *= worldScale.z;

	m_frustumBox = KdCollider::BoxInfo(0, localOBB);
}

void MapBase::Update()
{

}

void MapBase::DrawLit()
{
	if (!m_spModel) { return; }
	if (!m_isInView) { return; }

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}

void MapBase::GenerateDepthMapFromLight()
{
	if (!m_spModel) { return; }
	if (!m_isInView) { return; }

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}

void MapBase::PreDraw()
{
	//カメラに映ってるかどうか
	std::shared_ptr<CameraBase>spCamera = m_wpCamera.lock();
	if (spCamera)
	{
		m_isInView = CheckInScreen(spCamera->GetBoundingFrustum(), m_frustumBox);
	}
}

void MapBase::SetRotation(Math::Matrix _rMat)
{
	Math::Vector3 pos = GetPos();
	Math::Vector3 siz = GetScale();

	Math::Matrix tMat = Math::Matrix::CreateTranslation(pos);
	Math::Matrix sMat = Math::Matrix::CreateScale(siz);

	m_mWorld = sMat * _rMat * tMat;
}
