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


		Math::Matrix swordtip = m_spWeaponModel->FindNode("SwordTip")->m_localTransform;
		Math::Matrix guard = m_spWeaponModel->FindNode("guard")->m_localTransform;


		m_weaponLength = (swordtip - guard).Translation().Length();
		tipLocalPos = swordtip.Translation();
		baseLocalPos = guard.Translation();
	}

	if (!m_pDebugWire)
	{
		m_pDebugWire = std::make_unique<KdDebugWireFrame>();
	}

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
			float damage = m_characterAttackPower * (m_baseWeaponStatus.attackPower + (m_ChargeLV * 0.2)) + (m_weaponsStrengtheningInfo[WeaponsStrengthening_Attck].Lv * 0.2) * 10;
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
	if (m_ChargeLV > 0 && m_weaponsStrengtheningInfo[WeaponsStrengthening_ShockWave].Lv > 0)
	{
		m_chargeAttackMaxdamage = m_characterAttackPower + (m_baseWeaponStatus.attackPower * (m_ChargeLV * 0.2)) * (1 + m_weaponsStrengtheningInfo[WeaponsStrengthening_Attck].Lv * 0.2);

		std::shared_ptr<Dagger_ChargeAttack>spDagger_ChargeAttack;

		for (int i = 0; i < m_weaponsStrengtheningInfo[WeaponsStrengthening_ShockWave].Lv; i++)
		{
			spDagger_ChargeAttack = std::make_shared<Dagger_ChargeAttack>();
			spDagger_ChargeAttack->Init();

			float maxDistanceM = m_maxDistanceM * (m_ChargeLV * 0.3) + (m_weaponsStrengtheningInfo[WeaponsStrengthening_ShockWave_maxDistanceM].Lv * 0.3) * 10;
			float chargeAttackSpeed = m_chargeAttackSpeed * (m_ChargeLV * 0.3) + (m_weaponsStrengtheningInfo[WeaponsStrengthening_ShockWave_speed].Lv * 0.3);

			int hitNum = m_hitNum + (m_weaponsStrengtheningInfo[WeaponsStrengthening_ShockWave_hitNum].Lv * 1);



			float spread = 30.0f; // 角度の広がり
			float t;
			if (m_weaponsStrengtheningInfo[WeaponsStrengthening_ShockWave].Lv - 1 == 0)
			{
				t = 0;
				spread = 0;
			}
			else
			{
				t = (float)i / (m_weaponsStrengtheningInfo[WeaponsStrengthening_ShockWave].Lv - 1); // 0～1
			}

			float ang = m_attackAngle - spread * 0.5f + spread * t;

			spDagger_ChargeAttack->SetShockwaveStatus(hitNum, maxDistanceM, chargeAttackSpeed, m_chargeAttackMaxdamage, m_mWorld.Translation(), ang);
			spDagger_ChargeAttack->SetAttackHitCharacterList(m_attackHitCharacterList);
			spDagger_ChargeAttack->SetMapObjList(m_objList);
			SceneManager::Instance().AddObject(spDagger_ChargeAttack);
		}


	}


	//レベル2を止める
	auto spEffect2 = m_wpChargeLV2Effect.lock();
	if (spEffect2 && spEffect2->IsPlaying())
	{
		spEffect2->StopEffect();
	}

}

void Dagger::SetNowChargeTime(float _time)
{
	m_chargeTime = _time;


	if (m_chargeTime < m_chargeTimeMax / 3.0f)
	{
		//chargeレベル0
		auto spEffect = m_wpChargeLV0Effect.lock();
		if (!spEffect || !spEffect->IsPlaying())
		{
			// 処理範囲内に戻ったら再再生
			m_wpChargeLV0Effect = KdEffekseerManager::GetInstance().Play(
				"ChargeAttack/chargeLV0.efkefc", m_mWorld.Translation(), 1.0f, 1.0f, true
			);
		}

		if (spEffect && spEffect->IsPlaying())
		{
			spEffect->SetPos(m_mWorld.Translation());
		}

		m_ChargeLV = 0;
	}
	else if (m_chargeTime < m_chargeTimeMax / 3.0f * 2)
	{
		//レベル0を止める
		auto spEffect0 = m_wpChargeLV0Effect.lock();
		if (spEffect0 && spEffect0->IsPlaying())
		{
			spEffect0->StopEffect();
		}


		//chargeレベル1
		auto spEffect = m_wpChargeLV1Effect.lock();
		if (!spEffect || !spEffect->IsPlaying())
		{
			// 処理範囲内に戻ったら再再生
			m_wpChargeLV1Effect = KdEffekseerManager::GetInstance().Play(
				"ChargeAttack/chargeLV1.efkefc", m_mWorld.Translation(), 1.0f, 1.0f, true
			);
		}

		if (spEffect && spEffect->IsPlaying())
		{
			spEffect->SetPos(m_mWorld.Translation());
		}

		m_ChargeLV = 1;
	}
	else if (m_chargeTime < m_chargeTimeMax)
	{
		//レベル1を止める
		auto spEffect1 = m_wpChargeLV1Effect.lock();
		if (spEffect1 && spEffect1->IsPlaying())
		{
			spEffect1->StopEffect();
		}

		//chargeレベル2
		auto spEffect = m_wpChargeLV2Effect.lock();
		if (!spEffect || !spEffect->IsPlaying())
		{
			// 処理範囲内に戻ったら再再生
			m_wpChargeLV2Effect = KdEffekseerManager::GetInstance().Play(
				"ChargeAttack/chargeLV2.efkefc", m_mWorld.Translation(), 1.0f, 1.0f, true
			);
		}

		if (spEffect && spEffect->IsPlaying())
		{
			spEffect->SetPos(m_mWorld.Translation());
		}

		m_ChargeLV = 2;
	}
	else
	{
		//chargeレベル2
		auto spEffect = m_wpChargeLV2Effect.lock();
		if (!spEffect || !spEffect->IsPlaying())
		{
			// 処理範囲内に戻ったら再再生
			m_wpChargeLV2Effect = KdEffekseerManager::GetInstance().Play(
				"ChargeAttack/chargeLV2.efkefc", m_mWorld.Translation(), 1.0f, 1.0f, true
			);
		}

		if (spEffect && spEffect->IsPlaying())
		{
			spEffect->SetPos(m_mWorld.Translation());
		}


		//chargeレベルMAX
		m_chargeTime = m_chargeTimeMax;

		m_ChargeLV = 2;
	}
}

void Dagger::SetAttackFlg(bool _flg)
{
	WeaponBase::SetAttackFlg(_flg);

	//レベル0を止める
	auto spEffect0 = m_wpChargeLV0Effect.lock();
	if (spEffect0 && spEffect0->IsPlaying())
	{
		spEffect0->StopEffect();
	}

	//レベル1を止める
	auto spEffect1 = m_wpChargeLV1Effect.lock();
	if (spEffect1 && spEffect1->IsPlaying())
	{
		spEffect1->StopEffect();
	}

	//レベル2を止める
	auto spEffect2 = m_wpChargeLV2Effect.lock();
	if (spEffect2 && spEffect2->IsPlaying())
	{
		spEffect2->StopEffect();
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
