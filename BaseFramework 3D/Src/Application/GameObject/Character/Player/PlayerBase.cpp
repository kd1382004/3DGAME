#include "PlayerBase.h"
#include"../../Camera/CameraBase.h"
#include"../../../Info/KeyInfo/KeyInfo.h"

void PlayerBase::Init()
{
	m_keyConfigFilePath = "Asset/Data/KeyConfig/KeyConfig.json";
	m_charaStatusFilePath = "Asset/Data/ObjeData/Character/Praeyr/Status/Status.json";
	
	LoadKeyConfig(m_keyConfigFilePath);

	LoadCharaStatus(m_charaStatusFilePath);

	SaveKeyConfig(m_keyConfigFilePath);

	KeyInfo::Instance().SetKeyValid(m_keyConfig.moveRight);
	KeyInfo::Instance().SetKeyValid(m_keyConfig.moveLeft);
	KeyInfo::Instance().SetKeyValid(m_keyConfig.moveForward);
	KeyInfo::Instance().SetKeyValid(m_keyConfig.moveBackward);
	KeyInfo::Instance().SetKeyValid(m_keyConfig.jump);
}

void PlayerBase::Update()
{
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
	_moveVec *= m_status.moveSpeed.nowSpeed + 0.1;
	_nowPos += _moveVec;

	static	bool _spaceFlg = false;

	if (KeyInfo::Instance().GetValidKeyPush(m_keyConfig.jump, true, true))
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
	m_Gravity += m_gravityPower;
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

	std::ofstream ofs(_filePath);
	ofs << data.dump(4); // 4 はインデント
}

