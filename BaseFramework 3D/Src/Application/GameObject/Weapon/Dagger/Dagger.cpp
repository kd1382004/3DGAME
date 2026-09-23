#include "Dagger.h"
#include"../../Character/CharacterBase.h"
#include"ChargeAttack/Dagger_ChargeAttack.h"

#include"../../../Scene/SceneManager.h"
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
		m_tPoly->SetMaterial("Asset/Textures/Weapon/Daerre/jet.png");

		//トレイルポリゴンをビルボード(面をカメラに向ける)化
		m_tPoly->SetPattern(KdTrailPolygon::Trail_Pattern::eBillboard);
	}

	m_chargeTimeMax = 3;
}

void Dagger::Update()
{
	WeaponBase::Update();
	Math::Vector3 currTipPos = Math::Vector3::Transform(tipLocalPos, m_weponParentMat);
	Math::Vector3 currBasePos = Math::Vector3::Transform(baseLocalPos, m_weponParentMat);
	if (!m_attackFlg)
	{
		m_isFirstFrame = true;
		if (m_tPoly)
		{
			Math::Matrix mat = Math::Matrix::CreateTranslation(currTipPos);
			m_tPoly->AddPoint(mat);
		}
		return;
	}
	if (m_isFirstFrame)
	{
		m_prevTipPos = currTipPos;
		m_prevBasePos = currBasePos;
		m_prevWeponParentMat = m_weponParentMat; // 初回フレームの行列保持
		m_isFirstFrame = false;
	}
	// ----------------------------------------------------
	// m_hitBoxExtents に連動したローカル OBB の自動計算
	// ----------------------------------------------------
	float moveDist = (currTipPos - m_prevTipPos).Length(); // 1フレームでの移動量
	float extentX = m_hitBoxExtents.x;   // 横幅の半分
	float extentY = m_hitBoxExtents.y;   // 高さ(刃の長さ)の半分
	float extentZ = m_hitBoxExtents.z;   // 厚みの半分

	// 移動量(moveDist)の半分を進行方向に伸ばしてすり抜けを防止
	float extendedExtentY = extentY + moveDist * 0.5f;
	float extendedCenterY = m_hitBoxLocalOffset.y - moveDist * 0.5f;
	DirectX::BoundingOrientedBox localOBB(
		Math::Vector3(m_hitBoxLocalOffset.x, extendedCenterY, m_hitBoxLocalOffset.z),
		Math::Vector3(extentX, extendedExtentY, extentZ),
		DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)
	);
	// ★ワールド行列をかけて進行方向に傾いた OBB に変換！
	DirectX::BoundingOrientedBox sweptOBB;
	localOBB.Transform(sweptOBB, m_mWorld);
	// OBB (isOriented = true) として BoxInfo を作成
	KdCollider::BoxInfo box(KdCollider::TypeDamage, sweptOBB);

	// ----------------------------------------------------
	// 当たり判定処理
	// ----------------------------------------------------
	for (auto& wpGameObj : m_attackHitCharacterList)
	{
		auto spGameObj = wpGameObj.lock();
		if (!spGameObj) { continue; }
		if (IsAlreadyHit(spGameObj)) { continue; }

		std::list<KdCollider::CollisionResult> results;
		if (spGameObj->Intersects(box, &results))
		{
			m_hitCharactersList.push_back(spGameObj);
			float damage = m_characterAttackPower * (m_baseWeaponStatus.attackPower * (1 + m_chargeTime / m_chargeTimeMax));
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

			float hitStopTime = 0.03f;
			DeltaTime::Instance().HitStop(hitStopTime);
		}
	}

	// ----------------------------------------------------
	// デバッグワイヤー描画（ポリゴンの赤枠を表示）
	// ----------------------------------------------------
	if (m_pDebugWire)
	{
		// OBB の回転(Quaternion)と位置(Center)から行列を作成して描画
		Math::Matrix obbMat = Math::Matrix::CreateFromQuaternion(sweptOBB.Orientation) * Math::Matrix::CreateTranslation(sweptOBB.Center);
		m_pDebugWire->AddDebugBox(obbMat, sweptOBB.Extents, Math::Vector3::Zero, true, kRedColor);
	}

	// 座標および行列の更新
	m_prevTipPos = currTipPos;
	m_prevBasePos = currBasePos;
	m_prevWeponParentMat = m_weponParentMat;
	// トレイルポイント追加
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

void Dagger::ChargAttackPlay()
{
	std::shared_ptr<Dagger_ChargeAttack>spDagger_ChargeAttack = std::make_shared<Dagger_ChargeAttack>();
	spDagger_ChargeAttack->Init();
	spDagger_ChargeAttack->SetShockwaveStatus(1, 100, 1, 100, m_mWorld.Translation(), m_attackAngle);
	spDagger_ChargeAttack->SetAttackHitCharacterList(m_attackHitCharacterList);
	spDagger_ChargeAttack->SetMapObjList(m_objList);
	SceneManager::Instance().AddObject(spDagger_ChargeAttack);

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
