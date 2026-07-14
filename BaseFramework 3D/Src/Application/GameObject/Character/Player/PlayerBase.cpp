#include "PlayerBase.h"
#include"../../Camera/CameraBase.h"
#include"../../../Info/KeyInfo/KeyInfo.h"

void PlayerBase::Init()
{
	LoadKeyConfig("aa");
}

void PlayerBase::Update()
{
	float			_moveSpd = 0.1f;
	Math::Vector3	_nowPos = GetPos();

	Math::Vector3 _moveVec = Math::Vector3::Zero;

	if (KeyInfo::Instance().GetValidKeyPush(m_keyConfig.moveRight))
	{
		_moveVec.x += 1.0f;
	}

	if (KeyInfo::Instance().GetValidKeyPush(m_keyConfig.moveLeft))
	{
		_moveVec.x += -1.0f;
	}

	if (KeyInfo::Instance().GetValidKeyPush(m_keyConfig.moveForward))
	{
		_moveVec.z += 1.0f;
	}

	if (KeyInfo::Instance().GetValidKeyPush(m_keyConfig.moveBackward))
	{
		_moveVec.z += -1.0f;
	}




	//カメラに合わせる
	std::shared_ptr<CameraBase>camera = m_wpCamera.lock();
	if (camera)
	{
		_moveVec = Math::Vector3::TransformNormal(_moveVec, camera->GetRotationYMatrix());
	}

	_moveVec.Normalize();
	_moveVec *= _moveSpd;
	_nowPos += _moveVec;

	static	bool _spaceFlg = false;

	if (KeyInfo::Instance().GetValidKeyPush(m_keyConfig.jump,true,true))
	{
		if (!_spaceFlg)
		{
			_spaceFlg = true;
			m_Gravity = -0.2;
		}
	}
	else
	{
		_spaceFlg = false;
	}

	//キャラクターには常に重力がかかる
	m_Gravity += 0.01f;
	_nowPos.y -= m_Gravity;

	// キャラクターのワールド行列を創る処理
	m_mWorld = Math::Matrix::CreateTranslation(_nowPos);
}

void PlayerBase::LoadKeyConfig(std::string _filePath)
{
	ActionKeyConfig cfg;

	std::ifstream ifs(_filePath);
	
	if (!ifs.is_open())
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] キー設定ファイルが見つかりません : %s"),_filePath.c_str());
		return;
	}


	nlohmann::json data = nlohmann::json::parse(ifs, nullptr, false);

	if (data.is_discarded())
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] キー設定 JSON の読み込みに失敗しました : %s"), _filePath.c_str());
		return;
	}

	// 安全に読み込む（存在チェック付き）
	auto getInt = [&](const char* key, int defaultValue = 0)
		{
			if (data.contains(key) && data[key].is_number())
				return data[key].get<int>();
			return defaultValue;
		};

	cfg.moveForward = getInt("moveForward");
	cfg.moveBackward = getInt("moveBackward");
	cfg.moveRight = getInt("moveRight");
	cfg.moveLeft = getInt("moveLeft");
	cfg.jump = getInt("jump");


	m_keyConfig = cfg;
}

void PlayerBase::LoadCharaStatus(std::string _filePath)
{
	Status status;

	m_charaStatusFilePath = _filePath;

	std::ifstream ifs(_filePath);
	if (!ifs.is_open())
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] ステータスファイルが見つかりません : %s"), _filePath.c_str());
		return;
	}

	nlohmann::json data = nlohmann::json::parse(ifs, nullptr, false);

	if (data.is_discarded())
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] ステータスファイル JSON の読み込みに失敗しました : %s"), _filePath.c_str());
		return;
	}

	auto getFloat = [&](const nlohmann::json& j, const char* key, float def = 0.0f)
		{
			if (j.contains(key) && j[key].is_number())
				return j[key].get<float>();
			return def;
		};

	// HP
	status.HP.maxHP = getFloat(data["HP"], "maxHP");
	status.HP.nowHP = getFloat(data["HP"], "nowHP");

	// 攻撃力
	status.attck.baseAttckPowe = getFloat(data["attck"], "baseAttckPowe");
	status.attck.nowAttck = getFloat(data["attck"], "nowAttck");

	// 防御力
	status.defense.baseDefensePowe = getFloat(data["defense"], "baseDefensePowe");
	status.defense.nowDefense = getFloat(data["defense"], "nowDefense");

	// 移動速度
	status.moveSpeed.baseSpeed = getFloat(data["moveSpeed"], "baseSpeed");
	status.moveSpeed.walkMovePowe = getFloat(data["moveSpeed"], "walkMovePowe");
	status.moveSpeed.runMovePowe = getFloat(data["moveSpeed"], "runMovePowe");
	status.moveSpeed.nowSpeed = getFloat(data["moveSpeed"], "nowSpeed");

	m_status = status;
}
