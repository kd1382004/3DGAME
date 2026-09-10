#include "Dagger.h"
#include"../../Character/CharacterBase.h"

void Dagger::Init()
{
	m_WeaponStatusFilePath = "Asset/Data/ObjeData/Weapon/Dagger/BaseWeaponStatus.json";
	WeaponBase::Init();

	if (!m_spWeaponModel)
	{
		m_spWeaponModel = std::make_shared<KdModelWork>();
		m_spWeaponModel->SetModelData("Asset/Models/Weapon/Dagger/Rogue_Dagger.gltf");
	}

	if (!m_pDebugWire)
	{
		m_pDebugWire = std::make_unique<KdDebugWireFrame>();
	}

	m_localPos = Math::Vector3(0.0f, 1.2f, 0.0f);


	if (!m_tPoly)
	{
		m_tPoly = std::make_shared<KdTrailPolygon>();
		m_tPoly->SetMaterial("Asset/Textures/jet.png");

		//トレイルポリゴンをビルボード(面をカメラに向ける)化
		m_tPoly->SetPattern(KdTrailPolygon::Trail_Pattern::eBillboard);
	}
}

void Dagger::Update()
{
	WeaponBase::Update();

	// 現フレームの先端・基部ワールド座標
	Math::Vector3 currTipPos = Math::Vector3::Transform(tipLocalPos, m_weponParentMat);
	Math::Vector3 currBasePos = Math::Vector3::Transform(baseLocalPos, m_weponParentMat);

	if (!m_attackFlg)
	{ 
		m_isFirstFrame = true;

		if (m_tPoly)
		{
			//トレイルポイント
			if (m_tPoly)
			{
				Math::Matrix mat = Math::Matrix::CreateTranslation(currTipPos);

				m_tPoly->AddPoint(mat);
			}
		}

		return; 
	}



	if (m_isFirstFrame)
	{
		m_prevTipPos = currTipPos;
		m_prevBasePos = currBasePos;
		m_isFirstFrame = false;
	}

	// --- 速度に応じた動的サブステッピング分割数の計算 ---
	float moveDist = (currTipPos - m_prevTipPos).Length();

	// 半径の距離ごとに1分割（隙間が絶対できないように設定）
	int steps = static_cast<int>(std::ceil(moveDist / m_hitSphereRadius));
	if (steps < 1) { steps = 1; }
	const int stepsMax = 20;
	if (steps > stepsMax) { steps = stepsMax; } // 安全のための上限値

	std::vector<KdCollider::SphereInfo> sphereList;


	// フレームを線形補間しながら判定球を生成
	for (int i = 0; i <= steps; ++i)
	{
		float t = static_cast<float>(i) / static_cast<float>(steps);
		Math::Vector3 tipPos = Math::Vector3::Lerp(m_prevTipPos, currTipPos, t);
		Math::Vector3 basePos = Math::Vector3::Lerp(m_prevBasePos, currBasePos, t);
		Math::Vector3 midPos = (tipPos + basePos) * 0.5f;

		// 先端・基部・中点に判定球を配置
		sphereList.push_back(KdCollider::SphereInfo(KdCollider::TypeDamage, tipPos, m_hitSphereRadius));
		sphereList.push_back(KdCollider::SphereInfo(KdCollider::TypeDamage, basePos, m_hitSphereRadius));
		sphereList.push_back(KdCollider::SphereInfo(KdCollider::TypeDamage, midPos, m_hitSphereRadius));
	}


	// --- 当たり判定処理 ---
	for (auto& wpGameObj : m_attackHitCharacterList)
	{
		auto spGameObj = wpGameObj.lock();
		if (!spGameObj) continue;
		if (IsAlreadyHit(spGameObj)) continue;
		bool isHit = false;
		std::list<KdCollider::CollisionResult> results;
		for (const auto& sphere : sphereList)
		{
			if (spGameObj->Intersects(sphere, &results))
			{
				isHit = true;
				break; // 1つでも当たっていれば確定
			}
		}


		if (isHit)
		{
			m_hitCharactersList.push_back(spGameObj);
			float damage = m_characterAttackPower * m_baseWeaponStatus.attackPower;
			Math::Vector3 dir = currTipPos - m_prevTipPos;
			if (dir.LengthSquared() < 0.0001f) dir = Math::Vector3::Forward;
			spGameObj->OnAttackHit(
				damage,
				m_baseWeaponStatus.knockback,
				dir,
				m_baseWeaponStatus.startup,
				false,
				m_baseWeaponStatus.poiseBreak
			);




			float m_hitStoptim = 0.03f;
			DeltaTime::Instance().HitStop(m_hitStoptim);
		}
	}

	// 座標更新
	m_prevTipPos = currTipPos;
	m_prevBasePos = currBasePos;


	// デバッグ描画
	if (m_pDebugWire)
	{
		for (const auto& sphere : sphereList)
		{
			m_pDebugWire->AddDebugSphere(sphere.m_sphere.Center, sphere.m_sphere.Radius, kRedColor);
		}
	}


	//トレイルポイント
	if (m_tPoly)
	{
		Math::Matrix mat = Math::Matrix::CreateTranslation(currTipPos);

		m_tPoly->AddPoint(mat);
	}

}

void Dagger::DrawLit()
{
	if (!m_spWeaponModel) { return; }
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spWeaponModel, m_mWorld);
	if (m_attackFlg)
	{
		if (m_tPoly)
		{
			KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_tPoly);
		}
	}

}
bool Dagger::IsAlreadyHit(const std::shared_ptr<CharacterBase>& _chara)
{
	for (auto& wp : m_hitCharactersList)
	{
		if (auto sp = wp.lock())
		{
			if (sp == _chara) return true;
		}
	}
	return false;
}
