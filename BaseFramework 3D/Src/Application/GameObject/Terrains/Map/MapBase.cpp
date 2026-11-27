//---- 自分のヘッダー ----
#include "MapBase.h"						//マップ部品の基底クラス

//---- シーン ----
#include"../../../Scene/SceneManager.h"		//シーンの管理

//---- カメラ ----
#include"../../Camera/CameraBase.h"			//カメラの基底クラス

//=============================================
// 基本の処理
//=============================================

void MapBase::Init()
{
	if (!m_spModel) { return; }

	auto spData = m_spModel->GetData();

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

void MapBase::Update()
{

}

void MapBase::PostUpdate()
{
}

void MapBase::DrawLit()
{
	if (!m_spModel) { return; }
	//if (!m_isInView) { return; }
	if (m_fader.IsHidden()) { return; }

	bool isNoCull = (m_mapObjType == MapObjType::Wall || m_mapObjType == MapObjType::Ground);
	if (isNoCull)
	{
		KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);
	}

	// ディゾルブ
	float         edgeRange = 0.04f;
	Math::Vector3 edgeColor = { SceneManager::Instance().GetFogColor() };
	KdShaderManager::Instance().m_StandardShader.SetDissolve(m_fader.GetDissolve(), &edgeRange, &edgeColor);

	KdShaderManager::Instance().m_StandardShader.SetUVTiling(m_uvTiling);
	//発光は gltf の emissiveFactor × 発光テクスチャで決まる(工場マップの光る溝など)
	//emissiveFactor が無いモデル(ダンジョン・城)は 0 なので光らない
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld, kWhiteColor, { 1.0f, 1.0f, 1.0f });

	// 後片付け
	KdShaderManager::Instance().m_StandardShader.SetUVTiling({ 1.0f, 1.0f });
	KdShaderManager::Instance().m_StandardShader.SetDissolve(0.0f);

	if (isNoCull)
	{
		KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullBack);
	}
}

void MapBase::GenerateDepthMapFromLight()
{
	if (!m_spModel) { return; }

	// 床タイルは影を落とす必要がないためシャドウマップ生成から除外
	if (m_mapObjType == MapObjType::Ground) { return; }

	// 消えているものは影も落とさない
	if (m_fader.IsHidden()) { return; }

	// 影も本体と一緒に消えていくようにディゾルブをかける
	KdShaderManager::Instance().m_StandardShader.SetDissolve(m_fader.GetDissolve());
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	KdShaderManager::Instance().m_StandardShader.SetDissolve(0.0f);
}

void MapBase::PreDraw()
{
	//カメラに映ってるかどうか
	std::shared_ptr<CameraBase>spCamera = m_wpCamera.lock();
	if (spCamera)
	{
		m_isInView = CheckInScreen(spCamera->GetBoundingFrustum(), m_frustumBox);
	}

	//出現・消失のフェードを進める
	m_fader.Update(DeltaTime::Instance().GetRealDeltaTime());
}

//=============================================
// 設定
//=============================================

void MapBase::SetRotation(Math::Matrix _rMat)
{
	Math::Vector3 pos = GetPos();
	Math::Vector3 size = GetScale();

	Math::Matrix tMat = Math::Matrix::CreateTranslation(pos);
	Math::Matrix sMat = Math::Matrix::CreateScale(size);

	m_mWorld = sMat * _rMat * tMat;
}
