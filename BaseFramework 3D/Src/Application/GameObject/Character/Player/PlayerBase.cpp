#include "PlayerBase.h"
#include"../../Camera/CameraBase.h"
#include"../../../Info/KeyInfo/KeyInfo.h"
#include"../../../Scene/GameScene/GameScene.h"
#include"Action/NextFloorAction/NextFloorAction.h"

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
	KeyInfo::Instance().SetKeyValid(m_keyConfig.interact);


	m_pos = {};


	m_spCharaModel = std::make_shared<KdModelWork>();
	m_spCharaModel->SetModelData("Asset/Models/Character/Player/Player.gltf");

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();


	//アニメータの準備
	m_spAnimetor = std::make_shared<KdAnimator>();
	m_spAnimetor->SetAnimation(m_spCharaModel->GetAnimation("Walk"), true);



	if (!m_spNextFloorAction)
	{
		m_spNextFloorAction = std::make_shared<NextFloorAction>();
		m_spNextFloorAction->Init();
		m_spNextFloorAction->SetActionKey(m_keyConfig.interact);
	}

}

void PlayerBase::Update()
{
	//////////////////////////////////////////////////////////////
	//移動
	Move();



	//次の階に行くアクションをするかどうか

	if (m_spNextFloorAction)
	{
		m_spNextFloorAction->Update(m_nextFloorActionFlg);
	}




	//ジャンプ&重力処理
	JumpAndGravity();

	AngeleUpdate();

	m_spAnimetor->AdvanceTime(m_spCharaModel->WorkNodes());
	m_spCharaModel->CalcNodeMatrices();

	//座標行列を作る
	Math::Matrix tMat = Math::Matrix::CreateTranslation(m_pos);

	//回転行列
	Math::Matrix rMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angle));

	//行列の合成(S * R * T)
	m_mWorld = rMat * tMat;
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
	data["interact"] = m_keyConfig.interact;

	std::ofstream ofs(_filePath);
	ofs << data.dump(4);
}

void PlayerBase::Move()
{
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


		m_moveVec *= m_status.moveSpeed.nowSpeed;
		m_pos += m_moveVec;

		//////////////////////////////////////////////////////////////


	}
	else
	{
		//動いてないならストップを入れる
		m_moveMode = MoveMode::MoveStop;
	}
	//////////////////////////////////////////////////////////////
}

void PlayerBase::MoveNowSpeedDecision()
{
	switch (m_moveMode)
	{
	case PlayerBase::MoveWalk:
		m_status.moveSpeed.nowSpeed = m_status.moveSpeed.baseSpeed + m_status.moveSpeed.walkMovePowe;
		break;
	case PlayerBase::MoveRun:
		m_status.moveSpeed.nowSpeed = m_status.moveSpeed.baseSpeed + m_status.moveSpeed.runMovePowe;
		break;
	default:
		break;
	}
}

void PlayerBase::JumpAndGravity()
{
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
			m_jumpPower = 0.5;
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

