#include "Sword.h"
#include <cmath>
#include <algorithm>
#include"../../Character/CharacterBase.h"

#include"../../../Scene/SceneManager.h"
void Sword::Init()
{
	m_WeaponStatusFilePath = "Asset/Data/ObjeData/Weapon/Sword/BaseWeaponStatus.json";
	m_comboFileName = "Sword.json";   // フォルダーは持ち主（PlayerBase）が決める
	WeaponBase::Init();

	if (!m_spWeaponModel)
	{
		m_spWeaponModel = std::make_shared<KdModelWork>();
		m_spWeaponModel->SetModelData("Asset/Models/Weapon/Sword/Warrior_Sword.gltf");


		Math::Matrix swordtip = m_spWeaponModel->FindNode("SwordTip")->m_localTransform;
		Math::Matrix guard = m_spWeaponModel->FindNode("guard")->m_localTransform;


		m_weaponLength = (swordtip - guard).Translation().Length();
		tipLocalPos = swordtip.Translation();
		baseLocalPos = guard.Translation();

		// 切っ先の頂点
		const float tipEnd = tipLocalPos.y;
		m_hitBoxLocalOffset = { 0.0f, (baseLocalPos.y + tipEnd) * 0.5f, 0.0f };
		m_hitBoxExtents = { 0.26f, (tipEnd - baseLocalPos.y) * 0.5f, 0.07f };
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

	if (!m_spDaggerSE)
	{
		m_spDaggerSE = KdAudioManager::Instance().Play("Asset/Sound/SE/Wepon/Dagger/Dagger.wav");
		if (m_spDaggerSE)
		{
			m_spDaggerSE->Stop();
		}
	}
	if (!m_spChargeSE)
	{
		m_spChargeSE = KdAudioManager::Instance().Play("Asset/Sound/SE/Wepon/Dagger/Charge.wav");
		if (m_spChargeSE)
		{
			m_spChargeSE->Stop();
		}
	}
	if (!m_spHitSE)
	{
		m_spHitSE = KdAudioManager::Instance().Play("Asset/Sound/SE/Wepon/Dagger/Hit.wav");
		if (m_spHitSE)
		{
			m_spHitSE->Stop();
		}
	}

	m_animePercentPolyStart = 0.33;
	m_animePercentPolyEnd = 0.5;
}

void Sword::Update()
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

	// コンボの段の当たり判定の範囲（hitStart〜hitEnd）の外なら、判定しない
	if (!IsInComboHitWindow())
	{
		m_isFirstFrame = true;   // 範囲に入ったフレームから掃引をやり直す
		if (m_tPoly && m_tPolyFlg)
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
	// 前フレームの判定箱と今の判定箱を両方覆う OBB を、今の剣のローカル空間で作る
	Math::Vector3 minPos = m_hitBoxLocalOffset - m_hitBoxExtents;
	Math::Vector3 maxPos = m_hitBoxLocalOffset + m_hitBoxExtents;

	const Math::Matrix prevWorld = m_localMat * m_prevWeponParentMat;
	const Math::Matrix prevToCurr = prevWorld * m_mWorld.Invert();

	for (int i = 0; i < 8; ++i)
	{
		const Math::Vector3 corner(
			m_hitBoxLocalOffset.x + ((i & 1) ? m_hitBoxExtents.x : -m_hitBoxExtents.x),
			m_hitBoxLocalOffset.y + ((i & 2) ? m_hitBoxExtents.y : -m_hitBoxExtents.y),
			m_hitBoxLocalOffset.z + ((i & 4) ? m_hitBoxExtents.z : -m_hitBoxExtents.z));

		const Math::Vector3 p = Math::Vector3::Transform(corner, prevToCurr);
		minPos = Math::Vector3::Min(minPos, p);
		maxPos = Math::Vector3::Max(maxPos, p);
	}

	DirectX::BoundingOrientedBox localOBB(
		(minPos + maxPos) * 0.5f,
		(maxPos - minPos) * 0.5f,
		DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

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
			m_defenseIgnore = 1.0f - std::pow(0.85f, m_weaponsStrengtheningInfo[WeaponsStrengthening_DefenseIgnore].Lv - 1);


			m_hitCharactersList.push_back(spGameObj);
			float damage = m_characterAttackPower * (m_baseWeaponStatus.attackPower + (m_ChargeLV * 0.2)) + (m_weaponsStrengtheningInfo[WeaponsStrengthening_Attck].Lv * 0.2) * 10;
			damage *= GetComboDamageRate();   // コンボの段のダメージ倍率
			Math::Vector3 dir = currTipPos - m_prevTipPos;
			if (dir.LengthSquared() < 0.0001f) dir = Math::Vector3::Forward;

			spGameObj->OnAttackHit(
				damage,
				m_baseWeaponStatus.knockback * GetComboKnockbackRate(),   // コンボの段のノックバック倍率
				dir,
				m_baseWeaponStatus.startup,
				false,
				m_defenseIgnore
			);

			float hitStopTime = GetComboHitStopTime(0.03f);   // コンボの段のヒットストップ
			DeltaTime::Instance().HitStop(hitStopTime);

			if (m_spHitSE)
			{
				m_spHitSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
				m_spHitSE->Play();
			}


			//スタミナ回復
			
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
	if (m_tPoly && m_tPolyFlg)
	{
		Math::Matrix mat = Math::Matrix::CreateTranslation(currTipPos);
		m_tPoly->AddPoint(mat);
	}
}

void Sword::DrawLit()
{
	if (!m_spWeaponModel) { return; }
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spWeaponModel, m_mWorld);
	if (m_attackFlg && m_tPolyFlg)
	{
		if (m_tPoly)
		{
			KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_tPoly);
		}
	}

}

void Sword::ChargAttackPlay()
{
	if (m_ChargeLV > 0 && m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].Lv > 0)
	{
		m_chargeAttackMaxdamage = m_characterAttackPower + (m_baseWeaponStatus.attackPower * (m_ChargeLV * 0.2)) * (1 + m_weaponsStrengtheningInfo[WeaponsStrengthening_Attck].Lv * 0.2);

	}


	//レベル2を止める
	auto spEffect2 = m_wpChargeLV2Effect.lock();
	if (spEffect2 && spEffect2->IsPlaying())
	{
		spEffect2->StopEffect();
	}

}

void Sword::SetNowChargeTime(float _time)
{
	m_chargeTime = _time;
	m_chargeTimeMaxFlg = false;

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
		m_chargeTimeMaxFlg = true;
	}


	if (m_spChargeSE)
	{
		// 1. チャージ速度強化(Lv)による速度倍率に応じた基本ピッチ (log2)
		float speedRate = (m_addCargeTime > 0.0f) ? (GetAddCargeTime() / m_addCargeTime) : 1.0f;
		float basePitch = (speedRate > 0.0f) ? std::log2(speedRate) : 0.0f;

		// 2. チャージ進行度 (0.0 ～ 1.0) に応じたピッチ上昇量 (最大 +0.4f)
		float progress = (m_chargeTimeMax > 0.0f) ? (m_chargeTime / m_chargeTimeMax) : 0.0f;
		progress = std::clamp(progress, 0.0f, 1.0f);
		float progressPitch = progress * 0.4f;

		// 総合ピッチ (-1.0f ～ 1.0f の範囲に制限)
		float finalPitch = std::clamp(basePitch + progressPitch, -1.0f, 1.0f);
		m_spChargeSE->SetPitch(finalPitch);

		if (!m_spChargeSE->IsPlaying())
		{
			m_spChargeSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
			m_spChargeSE->Play();
		}
	}
}

void Sword::SetAttackFlg(bool _flg)
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


	if (m_spDaggerSE && _flg)
	{
		m_spDaggerSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
		m_spDaggerSE->Play();
	}


	if (m_spChargeSE)
	{
		m_spChargeSE->Stop();
	}
}

void Sword::StopWeaponSE()
{
	if (m_spChargeSE)
	{
		m_spChargeSE->Stop();
	}

	if (m_spDaggerSE)
	{
		m_spDaggerSE->Stop();
	}

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

const std::vector<ComboStep>& Sword::GetComboList()
{
	constexpr int kBaseComboCount = 3;
	const int unlocked = std::max(0, kBaseComboCount + GetWeaponsStrengtheningLv(WeaponsStrengthening_Characteristic));
	const size_t count = std::min(m_comboList.size(), static_cast<size_t>(unlocked));

	m_nowLvComboList.assign(m_comboList.begin(), m_comboList.begin() + count);
	return m_nowLvComboList;
}

bool Sword::IsAlreadyHit(const std::shared_ptr<CharacterBase>& _chara)
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
