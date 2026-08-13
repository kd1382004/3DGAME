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
}

void Dagger::Update()
{
	WeaponBase::Update();

	if (!m_attackFlg) { return; }

	float r = tipLocalPos.y / 3.0;

	Math::Matrix finalMat = m_weponParentMat;

	Math::Vector3 worldPos;

	//武器の先端を取る
	finalMat = m_weponParentMat;
	worldPos = Math::Vector3::Transform(tipLocalPos, finalMat);

	DirectX::BoundingSphere sphere1;
	sphere1.Radius = r;
	sphere1.Center = GetPos();
	KdCollider::SphereInfo spher1Info(KdCollider::TypeBump, sphere1);

	DirectX::BoundingSphere sphere2;
	sphere2.Radius = r;
	sphere2.Center = Math::Vector3::Transform(tipLocalPos / 2, finalMat);
	KdCollider::SphereInfo spher2Info(KdCollider::TypeBump, sphere2);

	DirectX::BoundingSphere sphere3;
	sphere3.Radius = r;
	sphere3.Center = Math::Vector3::Transform(tipLocalPos, finalMat);
	KdCollider::SphereInfo spher3Info(KdCollider::TypeBump, sphere3);

	for (auto& wpGameObj : m_attackHitCharacterList)
	{
		auto spGameObj = wpGameObj.lock();
		if (!spGameObj) continue;

		// 実際に当たったキャラ
		std::shared_ptr<CharacterBase> hitCharacter = spGameObj;

		if (Hit(hitCharacter)) { continue; }

		if (hitCharacter->Intersects(spher1Info, nullptr))
		{
			//Hit後処理
			// 初回ヒット
			m_hitCharactersList.push_back(hitCharacter);

			// Hit後処理
			float damage = m_characterAttackPower * m_baseWeaponStatus.attackPower;
			spGameObj->OnAttackHit(
				damage,
				0,
				Math::Vector3::Zero,
				m_baseWeaponStatus.startup,
				false,
				m_baseWeaponStatus.poiseBreak
			);
		}


		if (hitCharacter->Intersects(spher2Info, nullptr))
		{
			if (Hit(hitCharacter)) { continue; }

			//Hit後処理
			// 初回ヒット
			m_hitCharactersList.push_back(hitCharacter);

			// Hit後処理
			float damage = m_characterAttackPower * m_baseWeaponStatus.attackPower;
			spGameObj->OnAttackHit(
				damage,
				0,
				Math::Vector3::Zero,
				m_baseWeaponStatus.startup,
				false,
				m_baseWeaponStatus.poiseBreak
			);
		}

		if (hitCharacter->Intersects(spher3Info, nullptr))
		{
			if (Hit(hitCharacter)) { continue; }

			//Hit後処理
			// 初回ヒット
			m_hitCharactersList.push_back(hitCharacter);

			// Hit後処理
			float damage = m_characterAttackPower * m_baseWeaponStatus.attackPower;
			spGameObj->OnAttackHit(
				damage,
				0,
				Math::Vector3::Zero,
				m_baseWeaponStatus.startup,
				false,
				m_baseWeaponStatus.poiseBreak
			);
		}
	}
}

void Dagger::DrawLit()
{
	if (!m_spWeaponModel) { return; }
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spWeaponModel, m_mWorld);

}

bool Dagger::Hit(std::shared_ptr<CharacterBase> _chara)
{
	// 既にヒットしているかチェック
	bool alreadyHit = false;

	for (auto& wp : m_hitCharactersList)
	{
		if (auto sp = wp.lock())
		{
			if (sp == _chara)
			{
				alreadyHit = true;
				break;
			}
		}
	}


	return alreadyHit;
}
