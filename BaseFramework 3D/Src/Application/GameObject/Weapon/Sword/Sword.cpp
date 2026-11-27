//---- 自分のヘッダー ----
#include "Sword.h"									//剣

//---- シーン ----
#include"../../../Scene/SceneManager.h"				//シーンの管理

//---- キャラクター ----
#include"../../Character/CharacterBase.h"			//キャラクターの基底クラス

//---- 武器 ----
#include"Sword_FinalSlash/Sword_FinalSlash.h"		//剣の必殺技

//=============================================
// 基本の処理
//=============================================

void Sword::Init()
{
	m_weaponStatusFilePath = "Asset/Data/ObjeData/Weapon/Sword/BaseWeaponStatus.json";
	m_comboFileName = "Sword.json";   // フォルダーは持ち主（PlayerBase）が決める
	WeaponBase::Init();

	if (!m_spWeaponModel)
	{
		m_spWeaponModel = std::make_shared<KdModelWork>();
		m_spWeaponModel->SetModelData("Asset/Models/Weapon/Sword/Warrior_Sword.gltf");


		Math::Matrix swordtip = m_spWeaponModel->FindNode("SwordTip")->m_localTransform;
		Math::Matrix guard = m_spWeaponModel->FindNode("guard")->m_localTransform;


		m_weaponLength = (swordtip - guard).Translation().Length();
		m_tipLocalPos = swordtip.Translation();
		m_baseLocalPos = guard.Translation();

		// 切っ先の頂点
		const float tipEnd = m_tipLocalPos.y;
		m_hitBoxLocalOffset = { 0.0f, (m_baseLocalPos.y + tipEnd) * 0.5f, 0.0f };
		m_hitBoxExtents = { 0.26f, (tipEnd - m_baseLocalPos.y) * 0.5f, 0.07f };
	}

	if (!m_pDebugWire)
	{
		m_pDebugWire = std::make_unique<KdDebugWireFrame>();
	}

	//軌跡(剣先から鍔まで)
	InitTrail("Asset/Textures/Weapon/Daerre/jet.png", 20);

	m_chargeTimeMax = 3;

	if (!m_spDaggerSE)
	{
		m_spDaggerSE = KdAudioManager::Instance().Play("Asset/Sound/SE/Wepon/Sword/Sword.wav");
		if (m_spDaggerSE)
		{
			m_spDaggerSE->Stop();
		}
	}
	if (!m_spChargeSE)
	{
		m_spChargeSE = KdAudioManager::Instance().Play("Asset/Sound/SE/Wepon/Sword/Charge.wav");
		if (m_spChargeSE)
		{
			m_spChargeSE->Stop();
		}
	}
	if (!m_spHitSE)
	{
		m_spHitSE = KdAudioManager::Instance().Play("Asset/Sound/SE/Wepon/Sword/Hit.wav");
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
	Math::Vector3 currTipPos = Math::Vector3::Transform(m_tipLocalPos, m_weaponParentMat);
	Math::Vector3 currBasePos = Math::Vector3::Transform(m_baseLocalPos, m_weaponParentMat);
	if (!m_attackFlg)
	{
		m_isFirstFrame = true;
		AddTrailPoint(currTipPos, currBasePos);
		return;
	}

	//最後段(終ノ一閃)
	int finalSlashLv = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic1].m_lv;
	if (finalSlashLv > 0 && !m_nowLvComboList.empty() && !m_comboList.empty() &&
		m_comboStepNow.m_comboIndex == m_comboList.back().m_comboIndex)
	{
		if (m_animePercentNow >= m_comboStepNow.m_hitEnd && !m_finalSlashDoneFlg)
		{
			// ここに終ノ一閃の処理（1回だけ通る）
			m_finalSlashDoneFlg = true;
			SpawnFinalSlash(finalSlashLv);
		}
	}

	// コンボの段の当たり判定の範囲（hitStart〜hitEnd）の外なら、判定しない
	if (!IsInComboHitWindow())
	{
		m_isFirstFrame = true;   // 範囲に入ったフレームから掃引をやり直す
		if (m_tPolyFlg) { AddTrailPoint(currTipPos, currBasePos); }
		return;
	}


	if (m_isFirstFrame)
	{
		m_prevTipPos = currTipPos;
		m_prevBasePos = currBasePos;
		m_prevWeaponParentMat = m_weaponParentMat; // 初回フレームの行列保持
		m_isFirstFrame = false;
	}
	// ----------------------------------------------------
	// m_hitBoxExtents に連動したローカル OBB の自動計算
	// ----------------------------------------------------
	// 前フレームの判定箱と今の判定箱を両方覆う OBB を、今の剣のローカル空間で作る
	Math::Vector3 minPos = m_hitBoxLocalOffset - m_hitBoxExtents;
	Math::Vector3 maxPos = m_hitBoxLocalOffset + m_hitBoxExtents;

	const Math::Matrix prevWorld = m_localMat * m_prevWeaponParentMat;
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
			m_defenseIgnore = 1.0f - std::pow(0.85f, m_weaponsStrengtheningInfo[WeaponsStrengthening_DefenseIgnore].m_lv - 1);


			m_hitCharactersList.push_back(spGameObj);
			//攻撃の強化：1段階ごとに +20%(強化なしで1.0倍、Lv5で2.0倍)
			const float strengthenRate = 1.0f + m_weaponsStrengtheningInfo[WeaponsStrengthening_Attack].m_lv * 0.2f;
			float damage = m_characterAttackPower * (m_baseWeaponStatus.m_attackPower + (m_chargeLV * 0.2f)) * strengthenRate;
			damage *= GetComboDamageRate();   // コンボの段のダメージ倍率
			Math::Vector3 dir = currTipPos - m_prevTipPos;
			if (dir.LengthSquared() < 0.0001f) dir = Math::Vector3::Forward;

			// 当てる前に生きていたか（とどめの判定用）
			const bool wasAlive = !spGameObj->GetIsDead();


			if (m_spHitSE)
			{
				m_spHitSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
				m_spHitSE->Play();
			}

			// ---- エフェクト・ダメージ表示の位置を求める ----
			// 刃の根元→先端へレイを飛ばして、敵の表面に当たった点を使う
			Math::Vector3 effectPos = currTipPos;	// 当たらなかったときは刃の先端
			{
				Math::Vector3 bladeDir = currTipPos - currBasePos;
				const float bladeLen = bladeDir.Length();
				if (bladeLen > 0.0001f)
				{
					bladeDir /= bladeLen;

					KdCollider::RayInfo ray;
					ray.m_pos = currBasePos;
					ray.m_dir = bladeDir;
					ray.m_range = bladeLen;
					ray.m_type = KdCollider::TypeDamage;

					std::list<KdCollider::CollisionResult> rayResults;
					if (spGameObj->Intersects(ray, &rayResults) && !rayResults.empty())
					{
						effectPos = rayResults.front().m_hitPos;
					}
				}
			}

			KdEffekseerManager::GetInstance().Play("Hit/Weapon/Sword/Hit_Sword.efkefc", effectPos, 3.0f, 1.0f, false);

			spGameObj->OnAttackHit(
				damage,
				m_baseWeaponStatus.m_knockback * GetComboKnockbackRate(),   // コンボの段のノックバック倍率
				dir,
				m_baseWeaponStatus.m_startup,
				false,
				m_defenseIgnore,
				&effectPos	// 当たった位置
			);

			// ヒットストップ（段・チャージ・最後の段・とどめで強弱をつける）
			const bool isKill = wasAlive && spGameObj->GetIsDead();
			float hitStopTime = CalcHitStopTime(m_chargeLV, isKill);
			DeltaTime::Instance().HitStop(hitStopTime);

			// 画面を揺らす（ヒットストップが長い段・チャージ攻撃ほど強く）
			if (std::shared_ptr<CharacterBase> spParent = m_wpParentChara.lock())
			{
				const float shakeTime = 0.08f + hitStopTime * 2.0f;
				const float shakePower = hitStopTime * 1.6f + m_chargeLV * 0.03f;
				spParent->CameraShake(shakeTime, shakePower);
			}

			//スタミナ回復
			int lv = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic3].m_lv;
			if (lv > 0)
			{
				auto spParentChara = m_wpParentChara.lock();
				if (spParentChara)
				{
					constexpr float kHealBase = 1.0f;     // Lv1のときの回復量
					constexpr float kHealPerLv = 2.0f;    // Lvが1上がるごとに増える量

					int healAmount = (int)(kHealBase + (lv - 1) * kHealPerLv);
					spParentChara->StaminaHeal(healAmount);
				}
			}

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
	m_prevWeaponParentMat = m_weaponParentMat;
	// トレイルポイント追加
	if (m_tPolyFlg) { AddTrailPoint(currTipPos, currBasePos); }
}

void Sword::DrawLit()
{
	if (!m_spWeaponModel) { return; }
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spWeaponModel, m_mWorld);


}

//=============================================
// 取得
//=============================================

const std::vector<ComboStep>& Sword::GetComboList()
{
	constexpr int kBaseComboCount = 3;
	const int unlocked = std::max(0, kBaseComboCount + GetWeaponsStrengtheningLv(WeaponsStrengthening_Characteristic));
	const size_t count = std::min(m_comboList.size(), static_cast<size_t>(unlocked));

	m_nowLvComboList.assign(m_comboList.begin(), m_comboList.begin() + count);

	int lv = m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic2].m_lv;
	if (lv > 0)
	{
		for (size_t i = 0; i < m_nowLvComboList.size(); i++)
		{
			auto& step = m_nowLvComboList[i];

			// 元データが足りないときは最後の段を使う
			const auto& base = m_comboList[std::min(i, m_comboList.size() - 1)];

			step.m_damageRate = base.m_damageRate;
			step.m_speed = base.m_speed;
			if (lv > 0)
			{
				float fi = (float)i;
				step.m_damageRate += (0.1f + 0.1f * lv) * fi;
				step.m_speed += (0.1f + 0.01f * lv) * fi;

				if (step.m_speed >= base.m_speed * 1.5)
				{
					step.m_speed = base.m_speed * 1.5;
				}
			}
		}
	}
	m_finalSlashDoneFlg = false;
	return m_nowLvComboList;
}

//=============================================
// 設定
//=============================================

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

		m_chargeLV = 0;
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

		m_chargeLV = 1;
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

		m_chargeLV = 2;
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

		m_chargeLV = 2;
		m_chargeTimeMaxFlg = true;
	}


	if (m_spChargeSE)
	{
		// 1. チャージ速度強化(Lv)による速度倍率に応じた基本ピッチ (log2)
		float speedRate = (m_addChargeTime > 0.0f) ? (GetAddChargeTime() / m_addChargeTime) : 1.0f;
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

//=============================================
// その他の処理
//=============================================

void Sword::ChargeAttackPlay()
{
	if (m_chargeLV > 0 && m_weaponsStrengtheningInfo[WeaponsStrengthening_Characteristic].m_lv > 0)
	{
		//チャージ攻撃：(武器倍率 + チャージ段階×0.2) × 強化倍率(1段階ごとに +20%)
		m_chargeAttackMaxDamage = m_characterAttackPower * (m_baseWeaponStatus.m_attackPower + m_chargeLV * 0.2f) * (1.0f + m_weaponsStrengtheningInfo[WeaponsStrengthening_Attack].m_lv * 0.2f);

	}


	//レベル2を止める
	auto spEffect2 = m_wpChargeLV2Effect.lock();
	if (spEffect2 && spEffect2->IsPlaying())
	{
		spEffect2->StopEffect();
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

//=============================================
// 取得
//=============================================

bool Sword::IsAlreadyHit(const std::shared_ptr<CharacterBase>& _spChara)
{
	for (auto& wp : m_hitCharactersList)
	{
		if (auto sp = wp.lock())
		{
			if (sp == _spChara) return true;
		}
	}
	return false;
}

//=============================================
// その他の処理
//=============================================

void Sword::SpawnFinalSlash(int _lv)
{
	//---------- コンボの段数で変わる値 ----------
	// 今のコンボの段数（最後の段で出すので、出した段の番号 + 1）
	const int comboCount = m_comboStepNow.m_comboIndex + 1;

	// 基本の 3段からいくつ伸びているか（3段:0 〜 6段:3）
	constexpr int kBaseComboCount = 3;
	const int comboBonus = std::max(0, comboCount - kBaseComboCount);

	//---------- Lv と段数で変わる値 ----------
	// ダメージ倍率：Lv で +0.5、段数が1つ伸びるごとに +0.3
	const float damageRate = 1.5f + 0.5f * (_lv - 1) + 0.3f * comboBonus;

	// 判定の大きさ：Lv で +0.15、段数が1つ伸びるごとに +0.1
	const float sizeRate = 1.3f + 0.15f * (_lv - 1) + 0.1f * comboBonus;

	// 溜めの短さ：Lv で -0.05、段数が1つ伸びるごとに -0.02（下限 0.15秒）
	const float chargeTime = std::max(0.15f, 0.35f - 0.05f * (_lv - 1) - 0.02f * comboBonus);

	//---------- 判定の大きさ：今の剣の箱を大きくする ----------
	Math::Vector3 extents = m_hitBoxExtents * sizeRate;
	extents.z = std::max(extents.z, 0.3f);	// 薄すぎると当たらないので厚みを足す

	//---------- 位置：プレイヤーの正面に固定 ----------
	const float frontDist = 1.5f;	// プレイヤーから前に出す距離（m）
	const float height = 1.0f;		// 地面からの高さ（m）

	Math::Vector3 playerPos = m_mWorld.Translation();	// 持ち主がいなかったときの保険
	if (auto spParent = m_wpParentChara.lock())
	{
		playerPos = spParent->GetPos();
	}

	// 向いている方向（m_attackAngle は atan2(x, z) で作っているので、前は (sin, 0, cos)）
	const float rad = DirectX::XMConvertToRadians(m_attackAngle);
	const Math::Vector3 forward = { std::sin(rad), 0.0f, std::cos(rad) };

	const Math::Vector3 center = playerPos + forward * frontDist + Math::Vector3(0.0f, height, 0.0f);

	//---------- 向き：横一文字（刃を寝かせて、プレイヤーの向きに合わせる） ----------
	// 箱のローカルの Y（刃の長さ方向）を Z 軸で 90 度回して横にし、プレイヤーの向きに回す
	const Math::Matrix slashMat =
		Math::Matrix::CreateRotationZ(DirectX::XM_PIDIV2) *
		Math::Matrix::CreateRotationY(rad) *
		Math::Matrix::CreateTranslation(center);

	// 箱は中心から置くので、ローカルの中心のずれ（m_hitBoxLocalOffset）は使わない
	DirectX::BoundingOrientedBox localOBB(Math::Vector3::Zero, extents,
		DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	DirectX::BoundingOrientedBox worldOBB;
	localOBB.Transform(worldOBB, slashMat);

	//---------- ダメージ：通常の当たりと同じ式 × 倍率 ----------
	//攻撃の強化：1段階ごとに +20%
	float damage = m_characterAttackPower * m_baseWeaponStatus.m_attackPower
		* (1.0f + m_weaponsStrengtheningInfo[WeaponsStrengthening_Attack].m_lv * 0.2f);
	damage *= damageRate;

	// ノックバックの向き：プレイヤーの正面
	const Math::Vector3 dir = forward;

	m_defenseIgnore = 1.0f - std::pow(0.85f, m_weaponsStrengtheningInfo[WeaponsStrengthening_DefenseIgnore].m_lv - 1);

	//---------- 作ってシーンに追加 ----------
	auto spSlash = std::make_shared<Sword_FinalSlash>();
	spSlash->Init();
	spSlash->SetSlashStatus(worldOBB, damage, m_baseWeaponStatus.m_knockback * 2.0f, dir, m_defenseIgnore, chargeTime);
	spSlash->SetAttackHitCharacterList(m_attackHitCharacterList);
	if (std::shared_ptr<CharacterBase> spParent = m_wpParentChara.lock())
	{
		spSlash->SetCamera(spParent->GetCamera());	// 弾けたときに画面を揺らす用
	}
	SceneManager::Instance().AddObject(spSlash);
}
