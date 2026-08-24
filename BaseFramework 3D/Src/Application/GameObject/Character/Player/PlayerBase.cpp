#include "PlayerBase.h"
#include"../../Camera/CameraBase.h"
#include"../../../Info/KeyInfo/KeyInfo.h"
#include"../../../Scene/GameScene/GameScene.h"
#include"Action/NextFloorAction/NextFloorAction.h"

//UI
#include"../../UI/UIManager.h"
#include"../../UI/HPBar/HPBar.h"
#include"../../UI/UIMap/UIMapManager.h"
#include"../../UI/UIMap/UIMap_Player/UIMap_Player.h"
#include"../../UI/StaminaGage/StaminaGage.h"
#include"../../UI/SkillGage/SkillGage.h"

//武器
#include"../../Weapon/WeaponBase.h"

//インベントリ
#include"PlayerInventory/PlayerInventory.h"

//バフ
#include"PlayerBuffManager/PlayerBuffManager.h"


void PlayerBase::Init()
{
	m_keyConfigFilePath = "Asset/Data/KeyConfig/KeyConfig.json";
	m_charaStatusFilePath = "Asset/Data/ObjeData/Character/Praeyr/Status/Status.json";

	m_statusEditorName = "player";

	LoadKeyConfig(m_keyConfigFilePath);

	LoadCharaStatus(m_charaStatusFilePath);

	SaveKeyConfig(m_keyConfigFilePath);

	SaveCharaStatus(m_charaStatusFilePath);

	KeyInfo::Instance().SetKeyValid(m_keyConfig.moveRight);
	KeyInfo::Instance().SetKeyValid(m_keyConfig.moveLeft);
	KeyInfo::Instance().SetKeyValid(m_keyConfig.moveForward);
	KeyInfo::Instance().SetKeyValid(m_keyConfig.moveBackward);
	KeyInfo::Instance().SetKeyValid(m_keyConfig.jump);
	KeyInfo::Instance().SetKeyValid(m_keyConfig.dash);
	KeyInfo::Instance().SetKeyValid(m_keyConfig.evasion);
	KeyInfo::Instance().SetKeyValid(m_keyConfig.interact);
	KeyInfo::Instance().SetKeyValid(m_keyConfig.attack);


	m_pos = {};

	if (!m_spCharaModel)
	{
		m_spCharaModel = std::make_shared<KdModelWork>();
		m_spCharaModel->SetModelData("Asset/Models/Character/Player/Rogue.gltf");

		//アニメータの準備
		m_spAnimetor = std::make_shared<KdAnimator>();
		m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_playerAnimeName.IdleAnime), true);

	}

	if (!m_spNextFloorAction)
	{
		m_spNextFloorAction = std::make_shared<NextFloorAction>();
		m_spNextFloorAction->Init();
		m_spNextFloorAction->SetActionKey(m_keyConfig.interact);
	}

	if (!m_spPlayerInventory)
	{
		m_spPlayerInventory = std::make_shared<PlayerInventory>();
		m_spPlayerInventory->Init();
	}


	if (!m_pCollider)
	{
		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("Player", m_spCharaModel, KdCollider::TypeBump | KdCollider::TypeDamage);
	}

	if (!m_spPlayerBuffManager)
	{
		m_spPlayerBuffManager = std::make_shared<PlayerBuffManager>();
	}


	/*if (!m_pDebugWire)
	{
		m_pDebugWire = std::make_unique<KdDebugWireFrame>();
	}*/
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

	m_IsDetectedByEnemyNum = 0;
}

void PlayerBase::Update()
{
	if (m_isDead)
	{
		//デバック用プレイヤー死亡後Result移行
		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			std::shared_ptr<GameScene>spGameScene = m_wpGameScene.lock();
			if (spGameScene)
			{
				spGameScene->ChangeResultScene();
			}
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
		if (m_isDead)
		{
			//次の階に行くアクション
			m_spNextFloorAction->Update(m_nextFloorActionFlg);
		}

	}


	WeaponUpdate();

	AngeleUpdate();



	CharacterBase::Update();

	//座標行列を作る
	Math::Matrix tMat = Math::Matrix::CreateTranslation(m_pos);

	//回転行列
	Math::Matrix rMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angle));

	//行列の合成(S * R * T)
	m_mWorld = rMat * tMat;


	PlayerAnimeModeUpdate();
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


	if (m_IsDetectedByEnemyNum == 0)
	{
		m_IsDetectedByEnemy = false;
	}
}


void PlayerBase::SetGameScene(const std::shared_ptr<GameScene>& _GameScene)
{
	m_wpGameScene = _GameScene;

	if (m_spNextFloorAction)
	{
		m_spNextFloorAction->SetGameScene(_GameScene);
	}

}

void PlayerBase::SetNextFloorGaugeUI(const std::shared_ptr<NextFloorGaugeUI>& _NextFloorGaugeUI)
{
	if (m_spNextFloorAction)
	{
		m_spNextFloorAction->SetNextFloorGaugeUI(_NextFloorGaugeUI);
	}
}

void PlayerBase::SetPos(const Math::Vector3& pos)
{
	m_pos = pos;
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
		spHPBar->Set2DPos({ 0,-260 });
		spHPBar->SetDrawFlg(true);
		spHPBar->SetSiz(2);
		m_wpHPBar = spHPBar;
		_spUIManager->AddUIObj(spHPBar);

		m_wpUIMap_Player = _spUIManager->GetUIMapManager()->GetUIMap_Player();


		std::shared_ptr<StaminaGage>spStaminaGage = std::make_shared<StaminaGage>();
		spStaminaGage->Init();
		spStaminaGage->Set2DPos({ 0,-290 });
		spStaminaGage->SetSiz(2);
		m_wpStaminaGage = spStaminaGage;
		_spUIManager->AddUIObj(spStaminaGage);

		std::shared_ptr<SkillGage>spSkillGage = std::make_shared<SkillGage>();
		spSkillGage->Init();
		spSkillGage->Set2DPos({ 500,-330 });
		m_wpSkillGage = spSkillGage;
		_spUIManager->AddUIObj(spSkillGage);

	}



}

void PlayerBase::OnAttackHit(float _damage, float _knockbackDistance, const Math::Vector3& _knockbackDir, float _hitStunTime, bool _isCritical, float _ignoreRate)
{
	if (m_evasionFlg)
	{
		//DeltaTime::Instance().SetTimeScale(0.5);
		//DeltaTime::Instance().SetSlowTimer(0.1);

		////回避成功演出入れる

		return;
	}

	CharacterBase::OnAttackHit(_damage, _knockbackDistance, _knockbackDir, _hitStunTime, _isCritical, _ignoreRate);

}

void PlayerBase::HPHeal(int _Heal)
{
	m_status.HP.nowHP += _Heal;
	if (m_status.HP.nowHP >= m_status.HP.maxHP)
	{
		m_status.HP.nowHP = m_status.HP.maxHP;
	}


	std::shared_ptr<HPBar>spHPBar = m_wpHPBar.lock();
	if (spHPBar)
	{
		float percent = m_status.HP.nowHP / m_status.HP.maxHP;
		spHPBar->SetHPBarTexPercent(percent);
	}
}

void PlayerBase::AddMaxHP(int _Bosst, bool flg)
{

	m_status.HP.maxHP += _Bosst;

	if (flg)
	{
		m_status.HP.nowHP += _Bosst;
	}

	std::shared_ptr<HPBar>spHPBar = m_wpHPBar.lock();
	if (spHPBar)
	{
		float percent = m_status.HP.nowHP / m_status.HP.maxHP;
		spHPBar->SetHPBarTexPercent(percent);
	}
}

void PlayerBase::SetDead()
{
	m_nowPlayerAnimeMode = PlayerAnimeMode::DeathAnime;
	m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_playerAnimeName.DeathAnime), false);
	m_isDead = true;
}

void PlayerBase::WeaponUpdate()
{
	//回避中はできない
	if (m_evasionAnimeFlg) { return; }
	//やられたらできない
	if (m_isDead) { return; }

	//攻撃力の更新
	m_status.attck.nowAttck = m_status.attck.baseAttckPowe + m_status.attck.addAttack;

	if (KeyInfo::Instance().GetValidKeyPush(m_keyConfig.attack, true) && m_normalAttack)
	{
		m_normalAttack = false;
		m_normalAttackWaitNow = 0;

		m_nowPlayerAnimeMode = PlayerBase::SwordAttackAnime;
		std::shared_ptr<WeaponBase > spWeapon = m_wpWepon.lock();
		if (spWeapon)
		{
			spWeapon->SetAttackFlg(true);
			spWeapon->SetCharacterAttackPower(m_status.attck.nowAttck);
			// キャラの向きを敵の方向に補正
			const Math::Vector3 targetPos = spWeapon->GetCloseAttackHitCharacter();
			if (targetPos != Math::Vector3::Zero)
			{
				Math::Vector3 dir = targetPos - GetPos();
				dir.y = 0.0f; // XZ平面上の方向ベクトル
				if (dir.LengthSquared() > 0.0001f)
				{
					dir.Normalize();
					m_moveVec = dir;

					// XZ平面上の絶対向き(Y軸回転角度)を計算
					float angle = DirectX::XMConvertToDegrees(std::atan2(dir.x, dir.z));
					if (angle < 0.0f) { angle += 360.0f; }
					m_angle = angle;
				}
			}
		}
	}

	if (m_nowPlayerAnimeMode != PlayerBase::SwordAttackAnime)
	{
		m_normalAttackWaitNow += 1 * DeltaTime::Instance().GetGameDeltaTime();

		if (m_normalAttackWaitNow > m_normalAttackWaitMax)
		{
			m_normalAttackWaitNow = m_normalAttackWaitMax;
			m_normalAttack = true;
		}


		std::shared_ptr<SkillGage>spSkillGage = m_wpSkillGage.lock();
		if (spSkillGage)
		{
			spSkillGage->SetGaugePercent(m_normalAttackWaitNow / m_normalAttackWaitMax);
		}
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
	auto getInt = [&](const char* key, int defaultValue = -999)
		{
			if (data.contains(key) && data[key].is_number())
				return data[key].get<int>();
			return defaultValue;
		};


	int ans = getInt("moveForward");
	if (ans != -999)
	{
		cfg.moveForward = ans;
	}

	ans = getInt("moveBackward");
	if (ans != -999)
	{
		cfg.moveBackward = ans;
	}

	ans = getInt("moveRight");
	if (ans != -999)
	{
		cfg.moveRight = ans;
	}

	ans = getInt("moveLeft");
	if (ans != -999)
	{
		cfg.moveLeft = ans;
	}

	ans = getInt("jump");
	if (ans != -999)
	{
		cfg.jump = ans;
	}

	ans = getInt("dash");
	if (ans != -999)
	{
		cfg.dash = ans;
	}

	ans = getInt("evasion");
	if (ans != -999)
	{
		cfg.evasion = ans;
	}

	ans = getInt("attack");
	if (ans != -999)
	{
		cfg.attack = ans;
	}

	ans = getInt("interact");
	if (ans != -999)
	{
		cfg.interact = ans;
	}

	m_keyConfig = cfg;
}

void PlayerBase::SaveKeyConfig(std::string _filePath)
{
	nlohmann::json data;

	data["moveForward"] = m_keyConfig.moveForward;
	data["moveBackward"] = m_keyConfig.moveBackward;
	data["moveRight"] = m_keyConfig.moveRight;
	data["moveLeft"] = m_keyConfig.moveLeft;
	data["jump"] = m_keyConfig.jump;
	data["dash"] = m_keyConfig.dash;
	data["evasion"] = m_keyConfig.evasion;
	data["attack"] = m_keyConfig.attack;
	data["interact"] = m_keyConfig.interact;

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

	if (KeyInfo::Instance().GetValidKeyPush(m_keyConfig.moveRight))
	{
		m_moveVec.x += 1.0f;
	}

	if (KeyInfo::Instance().GetValidKeyPush(m_keyConfig.moveLeft))
	{
		m_moveVec.x += -1.0f;
	}

	if (KeyInfo::Instance().GetValidKeyPush(m_keyConfig.moveForward))
	{
		m_moveVec.z += 1.0f;
	}

	if (KeyInfo::Instance().GetValidKeyPush(m_keyConfig.moveBackward))
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
		std::shared_ptr<CameraBase>camera = m_wpCamera.lock();
		if (camera)
		{
			m_moveVec = Math::Vector3::TransformNormal(m_moveVec, camera->GetRotationYMatrix());
		}
		//////////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////////
		m_moveVec.Normalize();

		//移動状態から移動速度を求める
		MoveNowSpeedDecision();

		m_moveVec *= m_status.moveSpeed.nowSpeed * DeltaTime::Instance().GetGameDeltaTime();
		m_pos += m_moveVec;

		//////////////////////////////////////////////////////////////


	}
	else
	{
		//動いてないならストップを入れる
		m_moveMode = MoveMode::MoveStop;
		m_nowPlayerAnimeMode = PlayerBase::IdleAnime;
	}
	//////////////////////////////////////////////////////////////
}

void PlayerBase::MoveNowSpeedDecision()
{
	//移動モードの切り替え
	if (KeyInfo::Instance().GetValidKeyPush(m_keyConfig.dash))
	{

		if (m_IsDetectedByEnemy)
		{
			if (ConsumeStamina(m_dashStaminaDrainPerSec * DeltaTime::Instance().GetGameDeltaTime()))
			{
				m_moveMode = MoveRun;
			}
			else
			{
				m_moveMode = MoveWalk;
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


	switch (m_moveMode)
	{
	case PlayerBase::MoveWalk:
		m_status.moveSpeed.nowSpeed = m_status.moveSpeed.baseSpeed + m_status.moveSpeed.walkMovePowe;
		m_nowPlayerAnimeMode = PlayerBase::WalkAnime;
		break;
	case PlayerBase::MoveRun:
		m_status.moveSpeed.nowSpeed = m_status.moveSpeed.baseSpeed + m_status.moveSpeed.runMovePowe;
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
		m_pos.y -= m_Gravity;
		m_Gravity += m_gravityPower;
		return;
	}

	if (m_groundHit)
	{
		m_jumpFlg = true;
	}

	m_jumpFlg = true;
	//////////////////////////////////////////////////////////////
	//ジャンプ処理
	if (KeyInfo::Instance().GetValidKeyPush(m_keyConfig.jump, true, true))
	{
		if (m_jumpFlg)
		{
			m_jumpPower = 1;
			m_Gravity = -m_jumpPower;
			m_jumpFlg = false;
		}

	}
	//////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////
	// 重力処理
	//キャラクターには常に重力がかかる
	m_pos.y -= m_Gravity;
	m_Gravity += m_gravityPower;

	//////////////////////////////////////////////////////////////
}

void PlayerBase::PlayerAnimeModeUpdate()
{


	if (!m_spAnimetor) { return; }
	if (!m_spCharaModel) { return; }

	switch (m_oldPlayerAnimeMode)
	{
	case PlayerBase::PickUpAnime:
	case PlayerBase::SwordAttackAnime:
	case PlayerBase::PunchAttackAnime:
	case PlayerBase::RollAnime:

		if (!m_spAnimetor->IsAnimationEnd())
		{
			m_nowPlayerAnimeMode = m_oldPlayerAnimeMode;
		}
		else
		{

			if (m_evasionAnimeFlg)
			{
				m_evasionAnimeFlg = false;
			}

			std::shared_ptr<WeaponBase > spWeapon = m_wpWepon.lock();
			if (spWeapon)
			{
				spWeapon->SetAttackFlg(false);
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
			m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_playerAnimeName.IdleAnime), true);
			break;
		case PlayerBase::WalkAnime:
			m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_playerAnimeName.WalkAnime), true);
			break;
		case PlayerBase::RunAnime:
			m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_playerAnimeName.RunAnime), true);
			break;
		case PlayerBase::PickUpAnime:
			m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_playerAnimeName.PickUpAnime), false);
			break;
		case PlayerBase::SwordAttackAnime:
			m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_playerAnimeName.SwordAttackAnime), false);
			break;
		case PlayerBase::PunchAttackAnime:
			m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_playerAnimeName.PunchAttackAnime), false);
			break;
		case PlayerBase::RollAnime:
			m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_playerAnimeName.EvasionAnime), false);
			break;
		case PlayerBase::DeathAnime:
			m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation(m_playerAnimeName.DeathAnime), false);
			break;
		default:
			break;
		}
	}



	m_spAnimetor->AdvanceTime(m_spCharaModel->WorkNodes(), 100);
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

	m_weponParentMatrix = rMat * handNode->m_worldTransform * modelWorld;

	//武器の親行列を設定
	std::shared_ptr<WeaponBase > spWeapon = m_wpWepon.lock();
	if (spWeapon)
	{
		spWeapon->SetParentMatrix(m_weponParentMatrix);
		spWeapon->SetParentRotation(rot);
	}

}

bool PlayerBase::ConsumeStamina(float _amount)
{
	//今のスタミナが減少量未満なら減らせない
	if (m_staminaNow < _amount) { return false; }


	m_staminaNow = std::max(0.0f, m_staminaNow - _amount);
	return true;
}

void PlayerBase::StaminaManager()
{
	m_staminaNow += m_staminaRegenPerSec * DeltaTime::Instance().GetGameDeltaTime();

	if (m_staminaNow < 0.0f) { m_staminaNow = 0.0f; }

	if (m_staminaNow > m_staminaMax) { m_staminaNow = m_staminaMax; }

	std::shared_ptr<StaminaGage>spStaminaGage = m_wpStaminaGage.lock();
	if (spStaminaGage)
	{
		spStaminaGage->SetStaminaBarTexPercent(m_staminaNow / m_staminaMax);
	}

}

void PlayerBase::EvasionUpdate()
{

	//やられたら回避できない
	if (m_isDead) { return; }

	if (KeyInfo::Instance().GetValidKeyPush(m_keyConfig.evasion, true))
	{

		if (!m_evasionAnimeFlg && ConsumeStamina(m_evasionCost))
		{
			m_evasionAnimeFlg = true;
			m_evasionFlg = true;
			m_nowPlayerAnimeMode = PlayerBase::RollAnime;

			std::shared_ptr<CameraBase>camera = m_wpCamera.lock();
			if (camera)
			{
				m_moveVec = { 0,0,1 };
				m_moveVec = Math::Vector3::TransformNormal(m_moveVec, camera->GetRotationYMatrix());
				m_moveVec.y = 0;
				m_moveVec.Normalize();
			}
		}
	}

	if (m_evasionAnimeFlg)
	{
		float t = m_spAnimetor->GetAnimeProgress();

		// 回避の速度カーブ
		float evasionSpeed = 0.0f;

		if (t < m_kEvasionAccelEnd)
		{
			evasionSpeed = 0.0f + (m_evasionSpeed - 0.0) * t / m_kEvasionAccelEnd;
		}
		else if (t < m_kEvasionMaxSpeedEnd)
		{
			evasionSpeed = m_evasionSpeed; // 最大速度
		}
		else
		{
			m_evasionFlg = false;
			evasionSpeed = m_evasionSpeed + (0.0 - m_evasionSpeed) * (t - m_kEvasionMaxSpeedEnd) / (1.0 - m_kEvasionMaxSpeedEnd);
		}

		float dt = DeltaTime::Instance().GetGameDeltaTime();
		Math::Vector3 nextPos = m_pos + m_moveVec * evasionSpeed * dt;

		if (!RaycastFromTo(nextPos))
		{
			m_pos = nextPos;
		}
	}


}
