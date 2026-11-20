#include "PlayerBase.h"
#include"../../Camera/CameraBase.h"
#include"../../../Info/KeyInfo/KeyInfo.h"
#include"../../../Scene/GameScene/GameScene.h"
#include"Action/NextFloorAction/NextFloorAction.h"

#include"../../../Scene/SceneManager.h"


//UI
#include"../../UI/UIManager.h"
#include"../../UI/HPBar/HPBar.h"
#include"../../UI/UIMap/UIMapManager.h"
#include"../../UI/UIMap/UIMap_Player/UIMap_Player.h"
#include"../../UI/StaminaGage/StaminaGage.h"
#include"../../UI/SkillGage/SkillGage.h"
#include"../../UI/ItemGetUIController/ItemGetUIController.h"


//武器
#include"../../Weapon/WeaponBase.h"

//インベントリ
#include"PlayerInventory/PlayerInventory.h"


//バフ
#include"PlayerBuffManager/PlayerBuffManager.h"

//エフェクト
#include"../../Effect/EffectManager.h"
#include"../../Effect/DamageOverlay/DamageOverlay.h"

//LV
#include"PlayreLV/PlayreLV.h"

#include"../../Accessory/AccessoryManager.h"
#include"../../Accessory/Flashlight/Flashlight.h"

#include"PlayerAccessory/PlayerAccessory.h"

void PlayerBase::Init()
{
	m_keyConfigFilePath = "Asset/Data/KeyConfig/KeyConfig.json";
	m_charaDefaultStatusFilePath = "Asset/Data/ObjeData/Character/Praeyr/Status/Status.json";

	m_statusEditorName = "player";

	LoadKeyConfig(m_keyConfigFilePath);
	SaveKeyConfig(m_keyConfigFilePath);


	KeyInfo::Instance().SetKeyValid(m_keyConfig.m_moveRight);
	KeyInfo::Instance().SetKeyValid(m_keyConfig.m_moveLeft);
	KeyInfo::Instance().SetKeyValid(m_keyConfig.m_moveForward);
	KeyInfo::Instance().SetKeyValid(m_keyConfig.m_moveBackward);
	KeyInfo::Instance().SetKeyValid(m_keyConfig.m_jump);
	KeyInfo::Instance().SetKeyValid(m_keyConfig.m_dash);
	KeyInfo::Instance().SetKeyValid(m_keyConfig.m_evasion);
	KeyInfo::Instance().SetKeyValid(m_keyConfig.m_interact);
	KeyInfo::Instance().SetKeyValid(m_keyConfig.m_attack);


	m_pos = {};

	if (!m_spCharaModel)
	{
		m_spCharaModel = std::make_shared<KdModelWork>();
		m_spCharaModel->SetModelData("Asset/Models/Character/Player/Rogue.gltf");

		//アニメータの準備
		m_spAnimator = std::make_shared<KdAnimator>();
		m_spAnimator->SetAnimation(m_spCharaModel->GetAnimation(m_playerAnimeName.m_idleAnime), true);

	}

	if (!m_spNextFloorAction)
	{
		m_spNextFloorAction = std::make_shared<NextFloorAction>();
		m_spNextFloorAction->Init();
		m_spNextFloorAction->SetActionKey(m_keyConfig.m_interact);
	}

	if (!m_spPlayerInventory)
	{
		m_spPlayerInventory = std::make_shared<PlayerInventory>();
		m_spPlayerInventory->Init();
	}


	if (!m_pCollider)
	{
		m_pCollider = std::make_unique<KdCollider>();

		// 歩きのポーズでノード行列を計算してから読む（基本姿勢だと腕を広げた形のため）
		m_spAnimator->AdvanceTime(m_spCharaModel->WorkNodes());
		m_spCharaModel->CalcNodeMatrices();

		// エンプティの位置 = 球の中心、エンプティのスケール = 半径
		const UINT bodyType = KdCollider::Type::TypeBump | KdCollider::Type::TypeDamage;
		m_pCollider->RegisterCollisionShape("Player", m_spCharaModel, bodyType);
	}

	if (!m_spPlayerBuffManager)
	{
		m_spPlayerBuffManager = std::make_shared<PlayerBuffManager>();
	}

	if (!m_spPlayerLV)
	{
		m_spPlayerLV = std::make_shared<PlayreLV>();
	}

	if (!m_pDebugWire)
	{
		m_pDebugWire = std::make_unique<KdDebugWireFrame>();
	}

	if (!m_spPlayerAccessory)
	{
		m_spPlayerAccessory = std::make_unique<PlayerAccessory>();
		m_spPlayerAccessory->Init();
	}

	if (!m_spWalkSE)
	{
		m_spWalkSE = KdAudioManager::Instance().Play("Asset/Sound/SE/Player/Walk.wav", false);
		m_spWalkSE->Stop();
	}
	if (!m_spDashSE)
	{
		m_spDashSE = KdAudioManager::Instance().Play("Asset/Sound/SE/Player/Dash.wav", false);
		m_spDashSE->Stop();
	}
	if (!m_spDamageSE)
	{
		m_spDamageSE = KdAudioManager::Instance().Play("Asset/Sound/SE/Player/Damage.wav", false);
		m_spDamageSE->Stop();
	}
	if (!m_spRollSE)
	{
		m_spRollSE = KdAudioManager::Instance().Play("Asset/Sound/SE/Player/Roll.wav", false);
		m_spRollSE->Stop();
	}
	if (!m_spJumpSE)
	{
		m_spJumpSE = KdAudioManager::Instance().Play("Asset/Sound/SE/Player/Jump.wav", false);
		m_spJumpSE->Stop();
	}
}

void PlayerBase::PreUpdate()
{
	m_oldPlayerAnimeMode = m_nowPlayerAnimeMode;

	if (m_hitStunFlg)
	{
		m_hitStunTimer -= DeltaTime::Instance().GetRealDeltaTime();
		if (m_hitStunTimer <= 0)
		{
			m_hitStunFlg = false;
		}
	}

	m_isDetectedByEnemyNum = 0;

	//デバック
	if (GetAsyncKeyState('2') & 0x8000)
	{
		SetDead();
	}

}

void PlayerBase::Update()
{
	if (m_isDead && m_isDeadAnime)
	{
		std::shared_ptr<GameScene>spGameScene = m_wpGameScene.lock();
		if (spGameScene && !m_isGameSceneOut)
		{
			spGameScene->PlayerDefeat();
			m_isGameSceneOut = true;
		}
	}



	//バフのアップデート
	if (m_spPlayerBuffManager)
	{
		m_spPlayerBuffManager->Update();
	}


	//////////////////////////////////////////////////////////////]

	//移動
	Move();

	//ジャンプ&重力処理
	JumpAndGravity();

	//回避
	EvasionUpdate();

	if (m_spNextFloorAction)
	{	//やられたらできない
		if (!m_isDead)
		{
			//次の階に行くアクション
			m_spNextFloorAction->Update(m_nextFloorActionFlg);
		}

	}


	WeaponUpdate();

	AngleUpdate();



	CharacterBase::Update();

	//座標行列を作る
	Math::Matrix tMat = Math::Matrix::CreateTranslation(m_pos);

	//回転行列
	Math::Matrix rMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angle));

	//行列の合成(S * R * T)
	m_mWorld = rMat * tMat;


	PlayerAnimeModeUpdate();


	if (m_spPlayerAccessory)
	{
		m_spPlayerAccessory->Update(this);
	}
}

void PlayerBase::PostUpdate()
{


	CharacterBase::PostUpdate();

	std::shared_ptr<UIMap_Player>spUIMap_Player = m_wpUIMap_Player.lock();
	if (spUIMap_Player)
	{
		spUIMap_Player->SetPlayer3DPos(m_pos);

		float miniMapAngle = -(m_angle);
		spUIMap_Player->SetAngle(miniMapAngle);
	}


	// スタミナ管理
	StaminaManager();


	if (m_isDetectedByEnemyNum == 0)
	{
		m_isDetectedByEnemy = false;
	}


	{
		auto spEffect = m_wpHealEffect.lock();
		if (spEffect && spEffect->IsPlaying())
		{
			spEffect->SetPos(m_pos);
		}
	}


	if (m_isAttackBoostActive)
	{
		Math::Vector3 pos = m_pos + Math::Vector3{ 0, 0.5f, 0 };

		auto spEffect = m_wpAttackBoostEffect.lock();
		if (!spEffect || !spEffect->IsPlaying())
		{
			// 1周終わったら自分で再生し直す（毎回新しいオブジェクトを受け取る）
			m_wpAttackBoostEffect = KdEffekseerManager::GetInstance().Play(
				"AttackBoost/AttackBoost.efkefc", pos, 1.0f, 1.0f, false);
			spEffect = m_wpAttackBoostEffect.lock();
		}

		if (spEffect) { spEffect->SetPos(pos); }
	}

	if (m_isDashActive && m_groundHit)
	{
		Math::Vector3 pos = m_pos;

		auto spEffect = m_wpDashEffect.lock();
		if (!spEffect || !spEffect->IsPlaying())
		{
			// 1周終わったら自分で再生し直す（毎回新しいオブジェクトを受け取る）
			m_wpDashEffect = KdEffekseerManager::GetInstance().Play(
				"Dash/Dash.efkefc", pos, 1.0f, 1.0f, false);
			spEffect = m_wpDashEffect.lock();
		}

		if (spEffect) { spEffect->SetPos(pos); }
	}
	else
	{
		auto spEffect = m_wpDashEffect.lock();
		if (spEffect && spEffect->IsPlaying())
		{
			spEffect->StopEffect();
		}
	}
}

void PlayerBase::GenerateDepthMapFromLight()
{}

const std::vector<Inventory>& PlayerBase::GetPotionsInventoryList()
{
	if (m_spPlayerInventory)
	{
		return m_spPlayerInventory->GetPotionsInventoryList();
	}

	return std::vector<Inventory>();
}

const std::vector<Inventory>& PlayerBase::GetWeaponsStrengtheningInventoryList()
{
	if (m_spPlayerInventory)
	{
		return m_spPlayerInventory->GetWeaponsStrengtheningInventoryList();
	}

	return std::vector<Inventory>();
}

Math::Vector3 PlayerBase::GetBonePosition(std::string _boneName)
{
	if (!m_spCharaModel) return Math::Vector3::Zero;


	//
	auto* node = m_spCharaModel->FindNode(_boneName);
	Math::Vector3 pos = node->m_worldTransform.Translation();
	return pos;
}

Math::Matrix PlayerBase::GetBoneWorldMatrix(const std::string& _boneName)
{
	if (!m_spCharaModel) { return m_mWorld; }

	auto* node = m_spCharaModel->FindNode(_boneName);
	if (!node) { return m_mWorld; }

	// ボーンの行列（モデル基準） × キャラのワールド行列
	return node->m_worldTransform * m_mWorld;
}

void PlayerBase::SetPotionsInventoryList(const std::vector<Inventory>& _potions)
{
	if (m_spPlayerInventory)
	{
		m_spPlayerInventory->SetPotionsInventoryList(_potions);
	}
}

void PlayerBase::SetWeaponsStrengtheningInventoryList(const std::vector<Inventory>& _weapons)
{
	if (m_spPlayerInventory)
	{
		m_spPlayerInventory->SetWeaponsStrengtheningInventoryList(_weapons);
	}
}

void PlayerBase::SetGameScene(const std::shared_ptr<GameScene>& _spGameScene)
{
	m_wpGameScene = _spGameScene;

	if (m_spNextFloorAction)
	{
		m_spNextFloorAction->SetGameScene(_spGameScene);
	}

}

void PlayerBase::SetNextFloorGaugeUI(const std::shared_ptr<NextFloorGaugeUI>& _spNextFloorGaugeUI)
{
	if (m_spNextFloorAction)
	{
		m_spNextFloorAction->SetNextFloorGaugeUI(_spNextFloorGaugeUI);
	}
}

void PlayerBase::SetWeapon(const std::shared_ptr<WeaponBase>& _spWeapon)
{
	m_wpWeapon = _spWeapon;

	// コンボファイルのパスは「持ち主のフォルダー + 武器のファイル名」で決めて、武器に読ませる
	m_comboFilePath = m_comboFolder + _spWeapon->GetComboFileName();
	_spWeapon->LoadCombo(m_comboFilePath);

	m_attackCombo = _spWeapon->GetComboList();
}

void PlayerBase::SetPos(const Math::Vector3& _pos)
{
	m_pos = _pos;
	m_mWorld.Translation(m_pos);

	std::shared_ptr<UIMap_Player>spUIMap_Player = m_wpUIMap_Player.lock();
	if (spUIMap_Player)
	{
		spUIMap_Player->SetPlayer3DPos(m_pos);

		float miniMapAngle = -(m_angle);
		spUIMap_Player->SetAngle(miniMapAngle);
	}
}

void PlayerBase::AddUIList(std::shared_ptr<UIManager> _spUIManager)
{
	if (_spUIManager)
	{
		std::shared_ptr<HPBar>spHPBar = std::make_shared<HPBar>();
		spHPBar->Init();
		spHPBar->SetSiz(2);
		spHPBar->SetMaxHP(m_status.m_HP.m_maxHP);
		spHPBar->Set2DPos({ 0,-260 });
		spHPBar->SetDrawFlg(true);
		spHPBar->SetLV(m_status.m_LV);
		float percent = m_status.m_HP.m_nowHP / m_status.m_HP.m_maxHP;
		spHPBar->SetHPBarTexPercentNODelayOffset(percent);
		spHPBar->SetDrawMemoryFlg(true);
		m_wpHPBar = spHPBar;
		_spUIManager->AddUIObj(spHPBar);

		m_wpUIMap_Player = _spUIManager->GetUIMapManager()->GetUIMap_Player();


		std::shared_ptr<StaminaGage>spStaminaGauge = std::make_shared<StaminaGage>();
		spStaminaGauge->Init();
		spStaminaGauge->SetSiz(2);
		spStaminaGauge->Set2DPos({ 0,-290 });
		spStaminaGauge->SetMaxStamina(m_status.m_stamina.m_staminaMax);
		m_wpStaminaGauge = spStaminaGauge;
		_spUIManager->AddUIObj(spStaminaGauge);

		std::shared_ptr<ItemGetUIController>spItemGetUIController = std::make_shared<ItemGetUIController>();
		spItemGetUIController->Init();
		_spUIManager->AddUIObj(spItemGetUIController);
		m_spPlayerInventory->SetItemGetUIController(spItemGetUIController);
	}



}

void PlayerBase::SetEffectManager(std::shared_ptr<EffectManager> _spEffectManager)
{

	if (!_spEffectManager) { return; }

	m_wpEffectManager = _spEffectManager;
	if (!m_spDamageOverlay)
	{
		m_spDamageOverlay = std::make_shared<DamageOverlay>();
		m_spDamageOverlay->Init();

		_spEffectManager->AddEffectList(m_spDamageOverlay);
	}


}

void PlayerBase::AddMaxHP(int _boost, bool _flg)
{

	m_status.m_HP.m_maxHP += _boost;

	if (_flg)
	{
		m_status.m_HP.m_nowHP += _boost;
	}

	std::shared_ptr<HPBar>spHPBar = m_wpHPBar.lock();
	if (spHPBar)
	{
		float percent = m_status.m_HP.m_nowHP / m_status.m_HP.m_maxHP;
		spHPBar->SetMaxHP(m_status.m_HP.m_maxHP);
		spHPBar->SetHPBarTexPercent(percent);
	}
}

void PlayerBase::SetDead()
{
	m_nowPlayerAnimeMode = PlayerAnimeMode::DeathAnime;
	m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_playerAnimeName.m_deathAnime), 0.2f, false);

	// 倒れた音（2回呼ばれても1回だけ鳴らす）
	if (!m_isDead)
	{
		std::shared_ptr<KdSoundInstance> spSE = KdAudioManager::Instance().Play("Asset/Sound/SE/Player/Dead.wav", false);
		if (spSE) { spSE->SetVolume(VOLInfo::Instance().GetMSSEVol()); }
	}

	m_isDead = true;
}

void PlayerBase::SetLV(int _LV)
{
	m_status.m_LV = _LV;

	m_status.m_attack.m_baseAttackPower += 5;
	m_status.m_defense.m_baseDefensePower += 5;
	m_status.m_defense.m_nowDefense = m_status.m_defense.m_baseDefensePower;
	AddMaxHP(10, true);

	std::shared_ptr<HPBar>spHPBar = m_wpHPBar.lock();
	if (spHPBar)
	{
		spHPBar->SetLV(m_status.m_LV);
	}

	if (m_spPlayerLV)
	{
		m_spPlayerLV->CalcNextLevelExp(_LV);
	}
}

void PlayerBase::AddExp(int _gainedExp)
{
	if (!m_spPlayerLV) { return; }

	m_spPlayerLV->AddExp(_gainedExp, this);
}

void PlayerBase::SetAccessoryManager(std::shared_ptr<AccessoryManager> _spAccessoryManager)
{
	m_wpAccessoryManager = _spAccessoryManager;

	std::shared_ptr<Flashlight>spFlashlight = std::make_shared<Flashlight>();
	spFlashlight->Init();

	_spAccessoryManager->AddAccessoryList(spFlashlight);

	if (!m_spPlayerAccessory) { return; }
	m_spPlayerAccessory->SetFlashlight(spFlashlight);
}

void PlayerBase::SetPlayerWeaponInfo(int _info)
{
	{
		switch (_info)
		{
		case PlayerWeaponInfo::PlayerWeaponInfo_Dagger:
			m_playerAnimeName.m_attackAnime = m_playerAnimeName.m_daggerAttackAnime;
			break;
		case PlayerWeaponInfo::PlayerWeaponInfo_Sword:
			m_playerAnimeName.m_attackAnime = m_playerAnimeName.m_swordAttackAnime;
			break;

		default:
			break;
		}
		m_playerAnimeName.m_attackAnime;
	}
}

void PlayerBase::ClearAllBuffs()
{
	if (m_spPlayerBuffManager)
	{
		m_spPlayerBuffManager->ClearAllBuffs();
	}
}

void PlayerBase::ReloadCombo()
{
	std::shared_ptr<WeaponBase> spWeapon = m_wpWeapon.lock();
	if (!spWeapon) { return; }

	// 段の数や中身が変わるので、コンボの途中なら止めてから入れ替える
	CancelCombo();

	if (spWeapon->LoadCombo(m_comboFilePath))
	{
		m_attackCombo = spWeapon->GetComboList();
		KdDebugGUI::Instance().AddLog(U8("コンボを読み直しました : %s\n"), m_comboFilePath.c_str());
	}
}

void PlayerBase::OnAttackHit(float _damage, float _knockbackDistance, const Math::Vector3& _knockbackDir, float _hitStunTime, bool _isCritical, float _ignoreRate, const Math::Vector3* _hitPos)
{
	if (m_evasionFlg)
	{
		DeltaTime::Instance().SetTimeScale(0.1);
		DeltaTime::Instance().SetSlowTimer(1);

		//回避成功演出入れる
		std::shared_ptr<CameraBase>spCamera = m_wpCamera.lock();
		if (spCamera)
		{
			spCamera->SetEvasionCamera(1);
		}

		return;
	}

	CharacterBase::OnAttackHit(_damage, _knockbackDistance, _knockbackDir, _hitStunTime, _isCritical, _ignoreRate, _hitPos);

	if (m_spDamageOverlay)
	{
		m_spDamageOverlay->OnDamaged(_damage, m_status.m_HP.m_maxHP);
	}

	if (m_spDamageSE)
	{
		m_spDamageSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
		m_spDamageSE->Play();
	}

	KdEffekseerManager::GetInstance().Play("PlayerDamage/PlayerDamage.efkefc", m_pos, 3.0f, 1.0f, false);

	// 被弾で画面を揺らす
	CameraShake(0.2f, 0.15f);
}

void PlayerBase::HPHeal(int _heal)
{
	m_status.m_HP.m_nowHP += _heal;
	if (m_status.m_HP.m_nowHP >= m_status.m_HP.m_maxHP)
	{
		m_status.m_HP.m_nowHP = m_status.m_HP.m_maxHP;
	}


	std::shared_ptr<HPBar>spHPBar = m_wpHPBar.lock();
	if (spHPBar)
	{
		spHPBar->SetMaxHP(m_status.m_HP.m_maxHP);
		float percent = m_status.m_HP.m_nowHP / m_status.m_HP.m_maxHP;
		spHPBar->SetHPBarTexPercentNODelayOffset(percent);
	}


	if (_heal > 0)
	{
		m_wpHealEffect = KdEffekseerManager::GetInstance().Play("Heel/HealEffect.efkefc", m_pos, 1.0f, 1.0f, false);
	}
}

void PlayerBase::StopSound()
{
	if (m_spWalkSE) { m_spWalkSE->Stop(); }
	if (m_spDashSE) { m_spDashSE->Stop(); }
	if (m_spRollSE) { m_spRollSE->Stop(); }
	if (m_spJumpSE) { m_spJumpSE->Stop(); }
	if (m_spDamageSE) { m_spDamageSE->Stop(); }

	std::shared_ptr<WeaponBase> spWeapon = m_wpWeapon.lock();
	if (spWeapon)
	{
		spWeapon->StopWeaponSE();
	}

	// チャージ中に開いた場合はチャージ状態も解除
	m_chargeAttackFlg = false;
	m_chargeAttackCount = 0;
}

void PlayerBase::PlayAttackBoostEffect()
{
	m_isAttackBoostActive = true;

	auto spEffect = m_wpAttackBoostEffect.lock();
	if (spEffect && spEffect->IsPlaying()) { return; }   // 再生中ならそのまま

	m_wpAttackBoostEffect = KdEffekseerManager::GetInstance().Play(
		"AttackBoost/AttackBoost.efkefc", m_pos + Math::Vector3{ 0, 0.5f, 0 }, 1.0f, 1.0f, false);
}

void PlayerBase::StopAttackBoostEffect()
{
	m_isAttackBoostActive = false;

	if (auto effect = m_wpAttackBoostEffect.lock())
	{
		effect->StopEffect();   // 中で SetLoop(false) も呼ばれる
	}
	m_wpAttackBoostEffect.reset();
}

void PlayerBase::WeaponUpdate()
{


	//回避中はできない
	if (m_evasionAnimeFlg) { return; }
	//やられたらできない
	if (m_isDead) { return; }

	//攻撃力の更新
	m_status.m_attack.m_nowAttack = m_status.m_attack.m_baseAttackPower + m_status.m_attack.m_addAttack;


	ComboUpdate();

	if (KeyInfo::Instance().GetValidKeyPush(m_keyConfig.m_attack) && m_normalAttack)
	{
		m_nowPlayerAnimeMode = PlayerBase::SwordAttackAnime;
		m_normalAttackWaitNow = 0;
		m_spAnimator->SetAnimation(m_spCharaModel->GetAnimation(m_playerAnimeName.m_attackAnime), false);

		m_chargeAttackFlg = true;
		m_status.m_stamina.m_staminaRegenFlg = false;
		std::shared_ptr<WeaponBase > spWeapon = m_wpWeapon.lock();
		if (spWeapon)
		{
			float cost = spWeapon->GetChargeAttackMaxCost() * DeltaTime::Instance().GetGameDeltaTime();
			if (!spWeapon->GetChargeTimeMaxFlg())
			{
				if (ConsumeStamina(cost))
				{
					m_chargeAttackCount += DeltaTime::Instance().GetGameDeltaTime() * spWeapon->GetAddChargeTime();
				}
			}

			spWeapon->SetNowChargeTime(m_chargeAttackCount);
		}
	}
	else
	{
		m_chargeAttackFlg = false;
		if (m_chargeAttackCount != 0)
		{
			m_nowNormalAttack = true;
			m_normalAttack = false;
			m_chargeAttackCount = 0;
			std::shared_ptr<WeaponBase > spWeapon = m_wpWeapon.lock();
			if (spWeapon)
			{
				m_attackCombo = spWeapon->GetComboList();

				spWeapon->SetAttackFlg(true);
				StartComboStep(0);


				// キャラの向きを一番近い敵の方向に補正
				if (auto target = spWeapon->FindAimAssistTarget(m_pos))
				{
					Math::Vector3 dir = *target - GetPos();
					dir.y = 0.0f;
					if (dir.LengthSquared() > 0.0001f)
					{
						dir.Normalize();
						float angle = DirectX::XMConvertToDegrees(std::atan2(dir.x, dir.z));
						if (angle < 0.0f) { angle += 360.0f; }
						m_angle = angle;
						// m_moveVec = dir; // 移動にも反映したい場合だけ有効にする
					}
				}

				spWeapon->SetAngle(m_angle);
				spWeapon->ChargeAttackPlay();
			}
		}

	}


	//クールタイム回復
	//  ・溜め中でない
	//  ・攻撃アニメ再生中でない
	//  ・コンボの途中でない
	const bool isAttacking = m_chargeAttackFlg || m_nowNormalAttack || (m_comboStep >= 0);
	if (!isAttacking)
	{
		m_normalAttackWaitNow += 1 * DeltaTime::Instance().GetGameDeltaTime();

		if (m_normalAttackWaitNow > m_normalAttackWaitMax)
		{
			m_normalAttackWaitNow = m_normalAttackWaitMax;
			m_normalAttack = true;
		}
	}

	std::shared_ptr<SkillGage>spSkillGauge = m_wpSkillGauge.lock();
	if (spSkillGauge)
	{
		spSkillGauge->SetGaugePercent(m_normalAttackWaitNow / m_normalAttackWaitMax);
	}
}

void PlayerBase::LoadKeyConfig(std::string _filePath)
{
	ActionKeyConfig cfg;

	std::ifstream ifs(_filePath);

	if (!ifs.is_open())
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] キー設定ファイルが見つかりません : %s\n"), _filePath.c_str());
		return;
	}


	nlohmann::json data = nlohmann::json::parse(ifs, nullptr, false);

	if (data.is_discarded())
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] キー設定 JSON の読み込みに失敗しました : %s\n"), _filePath.c_str());
		return;
	}

	// 安全に読み込む（存在チェック付き）
	auto getInt = [&](const char* _key, int _defaultValue = -999)
		{
			if (data.contains(_key) && data[_key].is_number())
				return data[_key].get<int>();
			return _defaultValue;
		};


	int ans = getInt("moveForward");
	if (ans != -999)
	{
		cfg.m_moveForward = ans;
	}

	ans = getInt("moveBackward");
	if (ans != -999)
	{
		cfg.m_moveBackward = ans;
	}

	ans = getInt("moveRight");
	if (ans != -999)
	{
		cfg.m_moveRight = ans;
	}

	ans = getInt("moveLeft");
	if (ans != -999)
	{
		cfg.m_moveLeft = ans;
	}

	ans = getInt("jump");
	if (ans != -999)
	{
		cfg.m_jump = ans;
	}

	ans = getInt("dash");
	if (ans != -999)
	{
		cfg.m_dash = ans;
	}

	ans = getInt("evasion");
	if (ans != -999)
	{
		cfg.m_evasion = ans;
	}

	ans = getInt("attack");
	if (ans != -999)
	{
		cfg.m_attack = ans;
	}

	ans = getInt("interact");
	if (ans != -999)
	{
		cfg.m_interact = ans;
	}

	m_keyConfig = cfg;
}

void PlayerBase::SaveKeyConfig(std::string _filePath)
{
	nlohmann::json data;

	data["moveForward"] = m_keyConfig.m_moveForward;
	data["moveBackward"] = m_keyConfig.m_moveBackward;
	data["moveRight"] = m_keyConfig.m_moveRight;
	data["moveLeft"] = m_keyConfig.m_moveLeft;
	data["jump"] = m_keyConfig.m_jump;
	data["dash"] = m_keyConfig.m_dash;
	data["evasion"] = m_keyConfig.m_evasion;
	data["attack"] = m_keyConfig.m_attack;
	data["interact"] = m_keyConfig.m_interact;

	std::ofstream ofs(_filePath);
	ofs << data.dump(4);
}

void PlayerBase::Move()
{
	//スタンなら移動できない
	if (m_hitStunFlg) { return; }

	//ふっとばしなら移動できない
	if (m_isKnockbackFlg) { return; }

	//回避中は移動できない
	if (m_evasionAnimeFlg) { return; }

	//やられたら移動できない
	if (m_isDead) { return; }


	//////////////////////////////////////////////////////////////
	//どの方向に行きたいかベクトルを取る
	m_moveVec = Math::Vector3::Zero;

	if (KeyInfo::Instance().GetValidKeyPush(m_keyConfig.m_moveRight))
	{
		m_moveVec.x += 1.0f;
	}

	if (KeyInfo::Instance().GetValidKeyPush(m_keyConfig.m_moveLeft))
	{
		m_moveVec.x += -1.0f;
	}

	if (KeyInfo::Instance().GetValidKeyPush(m_keyConfig.m_moveForward))
	{
		m_moveVec.z += 1.0f;
	}

	if (KeyInfo::Instance().GetValidKeyPush(m_keyConfig.m_moveBackward))
	{
		m_moveVec.z += -1.0f;
	}

	//////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////
	//ベクトルの状態から動いてるかどうかを取る
	if (m_moveVec != Math::Vector3::Zero)
	{
		m_moveMode = MoveMode::MoveWalk;


		//////////////////////////////////////////////////////////////
		//動いてるならベクトルをカメラの向きに合わせる
		std::shared_ptr<CameraBase>spCamera = m_wpCamera.lock();
		if (spCamera)
		{
			m_moveVec = Math::Vector3::TransformNormal(m_moveVec, spCamera->GetRotationYMatrix());
		}
		//////////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////////
		m_moveVec.Normalize();

		//移動状態から移動速度を求める
		MoveNowSpeedDecision();

		m_moveVec *= m_status.m_moveSpeed.m_nowSpeed * DeltaTime::Instance().GetGameDeltaTime();
		m_pos += m_moveVec;


		if (m_moveMode == MoveMode::MoveWalk)
		{
			if (m_spWalkSE && !m_spWalkSE->IsPlaying() && m_groundHit)
			{
				m_spWalkSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
				m_spWalkSE->Play();
			}
			if (m_spDashSE)
			{
				m_spDashSE->Stop();
			}

		}

		if (m_moveMode == MoveMode::MoveRun)
		{
			if (m_spDashSE && !m_spDashSE->IsPlaying() && m_groundHit)
			{
				m_spDashSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
				m_spDashSE->Play();
			}
			if (m_spWalkSE)
			{
				m_spWalkSE->Stop();
			}

			m_isDashActive = true;
		}
		else
		{
			m_isDashActive = false;
		}


		if (!m_groundHit)
		{
			if (m_spDashSE)
			{
				m_spDashSE->Stop();
			}
			if (m_spWalkSE)
			{
				m_spWalkSE->Stop();
			}
		}


		//////////////////////////////////////////////////////////////

	}
	else
	{
		//動いてないならストップを入れる
		m_moveMode = MoveMode::MoveStop;
		m_nowPlayerAnimeMode = PlayerBase::IdleAnime;

		if (m_spWalkSE)
		{
			m_spWalkSE->Stop();
		}
		if (m_spDashSE)
		{
			m_spDashSE->Stop();
		}

		m_isDashActive = false;
	}
	//////////////////////////////////////////////////////////////
}

void PlayerBase::MoveNowSpeedDecision()
{
	if (m_chargeAttackFlg || m_nowPlayerAnimeMode == SwordAttackAnime)
	{
		m_moveMode = MoveStop;
	}
	else
	{
		//移動モードの切り替え
		if (KeyInfo::Instance().GetValidKeyPush(m_keyConfig.m_dash, true))
		{
			m_isDash = !m_isDash;
		}

		if (m_isDash)
		{
			if (m_isDetectedByEnemy)
			{
				if (ConsumeStamina(m_status.m_stamina.m_dashStaminaDrainPerSec * DeltaTime::Instance().GetGameDeltaTime()))
				{
					m_moveMode = MoveRun;
				}
				else
				{
					m_moveMode = MoveWalk;
					m_isDash = false;
				}

			}
			else
			{
				m_moveMode = MoveRun;
			}
		}
		else
		{
			m_moveMode = MoveWalk;
		}



	}


	switch (m_moveMode)
	{
	case PlayerBase::MoveWalk:
		m_status.m_moveSpeed.m_nowSpeed = m_status.m_moveSpeed.m_baseSpeed + m_status.m_moveSpeed.m_walkMovePower;
		m_nowPlayerAnimeMode = PlayerBase::WalkAnime;
		break;
	case PlayerBase::MoveRun:
		m_status.m_moveSpeed.m_nowSpeed = m_status.m_moveSpeed.m_baseSpeed + m_status.m_moveSpeed.m_runMovePower;
		m_nowPlayerAnimeMode = PlayerBase::RunAnime;
		break;
	case PlayerBase::MoveStop:
		m_status.m_moveSpeed.m_nowSpeed = 0.00000001;
		m_nowPlayerAnimeMode = PlayerBase::RunAnime;
		break;
	default:
		break;
	}
}

void PlayerBase::JumpAndGravity()
{



	//スタンならorふっとばし or 回避中 なら　移動できない
	if (m_hitStunFlg || m_isKnockbackFlg || m_evasionAnimeFlg || m_isDead)
	{
		ApplyGravity();
		return;
	}

	if (m_groundHit)
	{
		m_jumpFlg = true;
	}
	m_jumpFlg = true;
	//////////////////////////////////////////////////////////////
	//ジャンプ処理
	if (KeyInfo::Instance().GetValidKeyPush(m_keyConfig.m_jump, true, true))
	{
		if (m_jumpFlg)
		{
			m_jumpPower = 60;
			m_gravity = -m_jumpPower;
			m_jumpFlg = false;

			if (m_spJumpSE)
			{
				m_spJumpSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
				m_spJumpSE->Play();
			}
			if (m_spDashSE) { m_spDashSE->Stop(); }
			if (m_spWalkSE) { m_spWalkSE->Stop(); }
		}

	}
	//////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////
	// 重力処理
	//キャラクターには常に重力がかかる
	ApplyGravity();

	//////////////////////////////////////////////////////////////
}

void PlayerBase::PlayerAnimeModeUpdate()
{


	if (!m_spAnimator) { return; }
	if (!m_spCharaModel) { return; }

	switch (m_oldPlayerAnimeMode)
	{
	case PlayerBase::PickUpAnime:
	case PlayerBase::SwordAttackAnime:
	case PlayerBase::PunchAttackAnime:
	case PlayerBase::RollAnime:

		if (!m_spAnimator->IsAnimationEnd())
		{
			m_nowPlayerAnimeMode = m_oldPlayerAnimeMode;
		}
		else
		{

			if (m_evasionAnimeFlg)
			{
				m_evasionAnimeFlg = false;
			}

			std::shared_ptr<WeaponBase > spWeapon = m_wpWeapon.lock();
			if (spWeapon)
			{
				spWeapon->SetAttackFlg(false);
				m_nowNormalAttack = false;
			}
		}

		break;
	default:
		break;
	}

	if (m_nowPlayerAnimeMode != m_oldPlayerAnimeMode)
	{
		switch (m_nowPlayerAnimeMode)
		{
		case PlayerBase::IdleAnime:
			m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_playerAnimeName.m_idleAnime), 0.2f, true);
			break;
		case PlayerBase::WalkAnime:
			m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_playerAnimeName.m_walkAnime), 0.2f, true);
			break;
		case PlayerBase::RunAnime:
			m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_playerAnimeName.m_runAnime), 0.2f, true);
			break;
		case PlayerBase::PickUpAnime:
			m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_playerAnimeName.m_pickUpAnime), 0.2f, false);
			break;
		case PlayerBase::SwordAttackAnime:
			m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_playerAnimeName.m_attackAnime), 0.1f, false);
			break;
		case PlayerBase::PunchAttackAnime:
			m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_playerAnimeName.m_punchAttackAnime), 0.1f, false);
			break;
		case PlayerBase::RollAnime:
			m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_playerAnimeName.m_evasionAnime), 0.1f, false);
			break;
		case PlayerBase::DeathAnime:
			m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_playerAnimeName.m_deathAnime), 0.2f, false);
			break;
		default:
			break;
		}
	}



	m_spAnimator->AdvanceTime(m_spCharaModel->WorkNodes(), 100 * m_animeSpeedRate);
	m_spCharaModel->CalcNodeMatrices();


	//
	auto* handNode = m_spCharaModel->FindNode("Weapon.R");
	Math::Vector3 handPos = handNode->m_worldTransform.Translation();

	// モデルのワールド行列
	Math::Matrix modelWorld = GetMatrix();

	Math::Vector3 rot = { 0,0,70 };

	Math::Matrix rMat = Math::Matrix::CreateFromYawPitchRoll(
		DirectX::XMConvertToRadians(rot.y),
		DirectX::XMConvertToRadians(rot.x),
		DirectX::XMConvertToRadians(rot.z));

	m_weaponParentMatrix = rMat * handNode->m_worldTransform * modelWorld;

	//武器の親行列を設定
	std::shared_ptr<WeaponBase > spWeapon = m_wpWeapon.lock();
	if (spWeapon)
	{
		spWeapon->SetParentMatrix(m_weaponParentMatrix);
		spWeapon->SetParentRotation(rot);
		switch (m_nowPlayerAnimeMode)
		{
		case PlayerBase::SwordAttackAnime:
			spWeapon->SetAnimePercentNow(m_spAnimator->GetAnimeProgress());
		}

	}


	if (m_nowPlayerAnimeMode == PlayerBase::DeathAnime)
	{
		if (m_spAnimator->IsAnimationEnd())
		{
			m_isDeadAnime = true;
		}
	}

}

void PlayerBase::EvasionUpdate()
{

	//やられたら回避できない
	if (m_isDead) { return; }

	//攻撃中は無理
	if (m_chargeAttackFlg) { return; }

	// 攻撃中は、コンボのキャンセル可能な位置を過ぎるまで回避できない
	bool cancelByEvasion = false;
	if (m_nowNormalAttack)
	{
		if (m_comboStep < 0 || m_comboStep >= (int)m_attackCombo.size()) { return; }

		float progress = m_spAnimator->GetAnimeProgress();
		if (progress < m_attackCombo[m_comboStep].m_evasionCancel) { return; }

		cancelByEvasion = true;
	}

	if (m_evasionAnimeFlg)
	{
		float t = m_spAnimator->GetAnimeProgress();

		// 回避の速度カーブ
		float evasionSpeed = 0.0f;

		if (t < kEvasionAccelEnd)
		{
			evasionSpeed = 0.0f + (m_evasionSpeed - 0.0) * t / kEvasionAccelEnd;
		}
		else if (t < kEvasionMaxSpeedEnd)
		{
			evasionSpeed = m_evasionSpeed; // 最大速度
		}
		else
		{
			m_evasionFlg = false;
			evasionSpeed = m_evasionSpeed + (0.0 - m_evasionSpeed) * (t - kEvasionMaxSpeedEnd) / (1.0 - kEvasionMaxSpeedEnd);
		}

		float dt = DeltaTime::Instance().GetGameDeltaTime();
		Math::Vector3 nextPos = m_pos + m_moveVec * evasionSpeed * dt;

		if (!RaycastFromTo(nextPos))
		{
			m_pos = nextPos;
		}
	}

	if (KeyInfo::Instance().GetValidKeyPush(m_keyConfig.m_evasion, true))
	{

		if (!m_evasionAnimeFlg && ConsumeStamina(m_evasionCost))
		{
			if (cancelByEvasion)
			{
				CancelCombo();

				// 攻撃アニメが終わっていなくても、すぐに回避アニメへ切り替える
				m_spAnimator->CrossFade(m_spCharaModel->GetAnimation(m_playerAnimeName.m_evasionAnime), 0.1f, false);
				m_oldPlayerAnimeMode = PlayerBase::RollAnime;
			}

			m_evasionAnimeFlg = true;
			m_evasionFlg = true;
			m_nowPlayerAnimeMode = PlayerBase::RollAnime;

			// プレイヤーの向き（m_angle）基準で前方向に回避
			const float rad = DirectX::XMConvertToRadians(m_angle);
			m_moveVec = { std::sin(rad), 0.0f, std::cos(rad) };
			m_moveVec.Normalize();

			if (m_spRollSE)
			{
				m_spRollSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
				m_spRollSE->Play();
			}
		}
	}
}

void PlayerBase::StartComboStep(int _step)
{
	if (_step < 0 || _step >= (int)m_attackCombo.size()) { return; }

	const ComboStep& c = m_attackCombo[_step];
	m_comboStep = _step;
	m_comboReserved = false;
	m_stepForwardDone = 0.0f;

	m_nowPlayerAnimeMode = SwordAttackAnime;
	m_oldPlayerAnimeMode = SwordAttackAnime; // モード切替の処理でアニメを上書きされないように
	m_spAnimator->SetAnimation(m_spCharaModel->GetAnimation(c.m_animeName), false);
	m_animeSpeedRate = c.m_speed;
	m_nowNormalAttack = true;

	if (auto spWeapon = m_wpWeapon.lock())
	{
		spWeapon->SetComboStep(c);            // ダメージ・ノックバック・ヒットストップ・判定と軌跡のタイミング
		spWeapon->SetCharacterAttackPower(m_status.m_attack.m_nowAttack);
		AimToNearestEnemy(spWeapon);          // 段ごとに一番近い敵の方を向き直す
		spWeapon->SetAngle(m_angle);
		spWeapon->SetAttackFlg(true);         // 当たった敵のリストもここで空になる


		// キャラの向きを一番近い敵の方向に補正
		if (auto target = spWeapon->FindAimAssistTarget(m_pos))
		{
			Math::Vector3 dir = *target - GetPos();
			dir.y = 0.0f;
			if (dir.LengthSquared() > 0.0001f)
			{
				dir.Normalize();
				float angle = DirectX::XMConvertToDegrees(std::atan2(dir.x, dir.z));
				if (angle < 0.0f) { angle += 360.0f; }
				m_angle = angle;
			}
		}
	}
}

void PlayerBase::ComboUpdate()
{
	if (m_comboStep < 0 || m_comboStep >= (int)m_attackCombo.size()) { return; }

	const ComboStep& c = m_attackCombo[m_comboStep];
	float progress = m_spAnimator->GetAnimeProgress();

	// 踏み込み：段の最初から当たり判定が始まるまでに、stepForward だけ前へ進む
	if (c.m_stepForward > 0.0f)
	{
		const float endRate = std::max(c.m_hitStart, 0.05f);
		const float target = c.m_stepForward * std::clamp(progress / endRate, 0.0f, 1.0f);
		const float delta = target - m_stepForwardDone;
		if (delta > 0.0f)
		{
			// 向いている方向（m_angle は atan2(x, z) で作っているので、前は (sin, 0, cos)）
			const float rad = DirectX::XMConvertToRadians(m_angle);
			const Math::Vector3 forward = { std::sin(rad), 0.0f, std::cos(rad) };
			const Math::Vector3 nextPos = m_pos + forward * delta;

			// 壁の向こうへは進まない（回避と同じ判定）
			if (!RaycastFromTo(nextPos))
			{
				m_pos = nextPos;
			}
			m_stepForwardDone = target;
		}
	}

	// 入力の予約（押した瞬間だけ反応させる）
	if (progress >= c.m_acceptStart &&
		KeyInfo::Instance().GetValidKeyPush(m_keyConfig.m_attack, true))
	{
		m_comboReserved = true;
	}

	// キャンセルポイントを過ぎていて予約があれば、次の段へ
	if (m_comboReserved && progress >= c.m_cancelPoint &&
		m_comboStep + 1 < (int)m_attackCombo.size())
	{
		const ComboStep& next = m_attackCombo[m_comboStep + 1];

		if (ConsumeStamina(next.m_staminaCost))
		{
			StartComboStep(m_comboStep + 1);
			return;
		}

		// スタミナが足りない → 予約を取り消して、今の段で終わる
		m_comboReserved = false;
	}

	// 予約がないまま最後まで再生したら終了
	if (m_spAnimator->IsAnimationEnd())
	{
		m_comboStep = -1;
		m_animeSpeedRate = 1.0f;
		if (auto spWeapon = m_wpWeapon.lock())
		{
			spWeapon->ClearComboStep();   // 武器の値をコンボ以外の攻撃用に戻す
		}
		// 攻撃フラグを下ろす処理と待機アニメへの切り替えは、
		// 今の PlayerAnimeModeUpdate() の終了処理がそのままやってくれる
	}
}

void PlayerBase::AimToNearestEnemy(const std::shared_ptr<WeaponBase>& _spWeapon)
{

}

void PlayerBase::CancelCombo()
{
	m_comboStep = -1;
	m_comboReserved = false;
	m_animeSpeedRate = 1.0f;
	m_nowNormalAttack = false;

	if (auto spWeapon = m_wpWeapon.lock())
	{
		spWeapon->SetAttackFlg(false);   // 当たり判定と軌跡を止める
		spWeapon->ClearComboStep();      // 武器の値をコンボ以外の攻撃用に戻す
	}
}
