#include "SmashAttack.h"
#include"../../../../Player/PlayerBase.h"

void SmashAttack::Update()
{



	// 回転行列（判定とワイヤーで共通）
	const float radY = DirectX::XMConvertToRadians(m_angleY);
	Math::Matrix rotMat = Math::Matrix::CreateRotationY(radY);

	// ローカル座標でのオフセット
	Math::Vector3 localOffset = { 0.0f, m_boxSiz.y,m_boxSiz.z};

	// オフセットをキャラの向きに合わせて回転させ、位置に足す
	Math::Vector3 center = m_pos + Math::Vector3::TransformNormal(localOffset, rotMat);

	// OBB（回転対応ボックス）の作成
	DirectX::BoundingOrientedBox obb;
	obb.Center = center;
	obb.Extents = m_boxSiz;
	obb.Orientation = Math::Quaternion::CreateFromRotationMatrix(rotMat);

	KdCollider::BoxInfo boxInfo(KdCollider::Type::TypeDamage, obb);

	// デバッグ表示（判定と同じ center・rotMat を使う）
	if (m_pDebugWire)
	{
		Math::Matrix boxMat = rotMat * Math::Matrix::CreateTranslation(center);
		m_pDebugWire->AddDebugBox(boxMat, m_boxSiz, Math::Vector3::Zero, true, kRedColor);
	}


	if (!m_hitFlg) { return; }

	std::shared_ptr<PlayerBase> spPlayerBase = m_wpPlayerBase.lock();
	if (spPlayerBase)
	{
		if (spPlayerBase->Intersects(boxInfo, nullptr))
		{
			m_hitFlg = false;

			float damage = m_attckPower * m_attckMagnification;
			spPlayerBase->OnAttackHit(damage, m_knockbackDistance, m_knockbackDir, m_hitStunTime, false, 0);
		}
	}


}

void SmashAttack::SetAttackStatus()
{
	//攻撃力倍率
	m_attckMagnification = 1.3;

	//ふっとばし距離	
	m_knockbackDistance = 3;

	//スタン時間
	m_hitStunTime = 0.2;

	m_boxSiz = { 1,2,1 };
}
