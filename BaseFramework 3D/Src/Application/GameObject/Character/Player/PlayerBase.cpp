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

//武器
#include"../../Weapon/WeaponBase.h"


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


	if (!m_pCollider)
	{
		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("Player", m_spCharaModel, KdCollider::TypeBump | KdCollider::TypeDamage);
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
}

void PlayerBase::Update()
{
	//////////////////////////////////////////////////////////////]

	//移動
	Move();

	//ジャンプ&重力処理
	JumpAndGravity();

	if (m_spNextFloorAction)
	{
		//次の階に行くアクション
		m_spNextFloorAction->Update(m_nextFloorActionFlg);
	}



	AngeleUpdate();


	if (GetAsyncKeyState(m_keyConfig.attack))
	{
		m_nowPlayerAnimeMode = PlayerBase::SwordAttackAnime;
	}


	static float m_slowTimer = 0;

	if (GetAsyncKeyState(VK_RBUTTON))
	{
		m_nowPlayerAnimeMode = PlayerBase::PunchAttackAnime;
		DeltaTime::Instance().SetTimeScale(0.1f); // 10%速度にする（重いスロー）
		m_slowTimer = 2;
	}

	if (m_slowTimer > 0.0f)
	{
		m_slowTimer -= DeltaTime::Instance().GetRealDeltaTime();
		if (m_slowTimer <= 0.0f)
		{
			DeltaTime::Instance().SetTimeScale(1.0f); // 通常速度に戻す
		}
	}

	WeaponUpdate();


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
		spHPBar->Set2DPos({ 0,-300 });
		spHPBar->SetDrawFlg(true);
		spHPBar->SetSiz(2);
		m_wpHPBar = spHPBar;
		_spUIManager->AddUIObj(spHPBar);

		m_wpUIMap_Player = _spUIManager->GetUIMapManager()->GetUIMap_Player();


		std::shared_ptr<StaminaGage>spStaminaGage = std::make_shared<StaminaGage>();
		spStaminaGage->Init();
		spStaminaGage->Set2DPos({ 0,-330 });
		spStaminaGage->SetSiz(2);
		m_wpStaminaGage = spStaminaGage;
		_spUIManager->AddUIObj(spStaminaGage);
		
	}



}

void PlayerBase::WeaponUpdate()
{

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
	//スタンならorふっとばしなら　移動できない
	if (m_hitStunFlg || m_isKnockbackFlg)
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

		if (!m_spAnimetor->IsAnimationEnd())
		{
			m_nowPlayerAnimeMode = m_oldPlayerAnimeMode;
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
		spStaminaGage->SetStaminaBarTexPercent(m_staminaNow/ m_staminaMax);
	}

}
