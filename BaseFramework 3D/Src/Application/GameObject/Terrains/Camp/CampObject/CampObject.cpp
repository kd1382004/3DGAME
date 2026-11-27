//---- 自分のヘッダー ----
#include"CampObject.h"								//キャンプの置物

//---- キャンプ ----
#include"../CampLayoutData/CampLayoutData.h"		//キャンプの配置データ

namespace
{
	//エディタで選んでいる間の色(少し明るいオレンジ)
	const Math::Color kHighlightColor = { 1.5f, 1.25f, 0.6f, 1.0f };
}

//=============================================
// 基本の処理
//=============================================

void CampObject::Init()
{
	// 背景なのでカメラの遮蔽判定などは無し。モデルは SetData で読む
}

void CampObject::Update()
{
	if (!m_spAnimator || !m_spModel) { return; }
	m_spAnimator->AdvanceTime(m_spModel->WorkNodes(), 100.0f);
	m_spModel->CalcNodeMatrices();


	// CampObject::Update の最後
	if (!m_nextAnime.empty() && m_spAnimator->IsAnimationEnd())
	{
		PlayAnime(m_nextAnime, true);
		m_nextAnime.clear();
	}
}

void CampObject::DrawLit()
{
	if (!m_drawFlg) { return; }
	if (!m_spModel) { return; }
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld, m_highlightFlg ? kHighlightColor : kWhiteColor);
}

void CampObject::GenerateDepthMapFromLight()
{
	// 影を落とす(落としたくない物は描かない)
	if (!m_drawFlg) { return; }
	if (!m_spModel) { return; }
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}

//=============================================
// 設定
//=============================================

void CampObject::SetData(const CampObjectData& _data)
{
	m_data = _data;

	if (!m_spModel) { m_spModel = std::make_shared<KdModelWork>(); }
	m_spModel->SetModelData(m_data.m_modelPath);

	// エディタでマウスから選ぶための当たり判定(準備シーンではほかに使わない TypeEvent にする)
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("CampPick", m_spModel, KdCollider::TypeEvent);

	ApplyData();
}

//=============================================
// その他の処理
//=============================================

void CampObject::PlayAnime(const std::string& _name, bool _loop)
{
	if (!m_spModel || !m_spModel->GetData()) { return; }
	auto anime = m_spModel->GetData()->GetAnimation(_name);

	if (!anime) { return; }

	if (!m_spAnimator) { m_spAnimator = std::make_shared<KdAnimator>(); }


	m_spAnimator->SetAnimation(anime, _loop);
	m_spModel->CalcNodeMatrices();
}
