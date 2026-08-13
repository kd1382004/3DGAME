#include "WeaponBase.h"

#include"../Character/CharacterBase.h"


#include"../../Scene/SceneManager.h"
void WeaponBase::Init()
{
	LoadWeaponStatus(m_WeaponStatusFilePath);
	SaveWeaponStatus(m_WeaponStatusFilePath);
}

void WeaponBase::Update()
{
	m_mWorld = m_localMat * m_weponParentMat;
}

void WeaponBase::DrawLit()
{
}

void WeaponBase::CreateWeaponHitCollider()
{
}

void WeaponBase::SetAttackFlg(bool _flg)
{
	m_attackFlg = _flg; 

	if (m_attackFlg)
	{
		m_hitCharactersList.clear();
	}

}

Math::Vector3 WeaponBase::GetCloseAttackHitCharacter()
{
	Math::Vector3 ClosePos;

	float nearestDist = FLT_MAX;


	
	for (auto& wpGameObj : m_attackHitCharacterList)
	{
		auto spGameObj = wpGameObj.lock();
		if (!spGameObj) continue;

		float dist = (spGameObj->GetPos() - GetPos()).Length();

		dist = abs(dist);


		if (dist < nearestDist)
		{
			nearestDist = dist;
			ClosePos = spGameObj->GetPos();
		}


	}


	return ClosePos;
}

void WeaponBase::LoadWeaponStatus(const std::string& filePath)
{
	WeaponStatus ws;

	std::ifstream ifs(filePath);
	if (!ifs.is_open())
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] 武器ステータスファイルが見つかりません : %s\n"), filePath.c_str());
		return;
	}

	nlohmann::json data = nlohmann::json::parse(ifs, nullptr, false);

	if (data.is_discarded())
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] 武器ステータスファイルの読み込みに失敗しました : %s\n"), filePath.c_str());
		return;
	}

	auto getFloat = [&](const nlohmann::json& j, const char* key, float def = 0.0f)
		{
			if (j.contains(key) && j[key].is_number())
				return j[key].get<float>();
			return def;
		};

	// --- 基本ステータス ---
	ws.attackPower = getFloat(data, "attackPower");
	ws.attackSpeed = getFloat(data, "attackSpeed", 1.0f);
	ws.critRate = getFloat(data, "critRate");
	ws.critDamage = getFloat(data, "critDamage");

	// --- 武器の性質 ---
	ws.range = getFloat(data, "range", 1.0f);
	ws.weight = getFloat(data, "weight", 1.0f);
	ws.knockback = getFloat(data, "knockback");
	ws.poiseBreak = getFloat(data, "poiseBreak");
	ws.stunPower = getFloat(data, "stunPower");

	// --- モーション補正 ---
	ws.startup = getFloat(data, "startup");
	ws.recovery = getFloat(data, "recovery");
	ws.cancelWindow = getFloat(data, "cancelWindow");

	m_baseWeaponStatus = ws;
}

void WeaponBase::SaveWeaponStatus(const std::string& filePath)
{
	nlohmann::json data;

	// --- 基本ステータス ---
	data["attackPower"] = m_baseWeaponStatus.attackPower;
	data["attackSpeed"] = m_baseWeaponStatus.attackSpeed;
	data["critRate"] = m_baseWeaponStatus.critRate;
	data["critDamage"] = m_baseWeaponStatus.critDamage;

	// --- 武器の性質 ---
	data["range"] = m_baseWeaponStatus.range;
	data["weight"] = m_baseWeaponStatus.weight;
	data["knockback"] = m_baseWeaponStatus.knockback;
	data["poiseBreak"] = m_baseWeaponStatus.poiseBreak;
	data["stunPower"] = m_baseWeaponStatus.stunPower;

	// --- モーション補正 ---
	data["startup"] = m_baseWeaponStatus.startup;
	data["recovery"] = m_baseWeaponStatus.recovery;
	data["cancelWindow"] = m_baseWeaponStatus.cancelWindow;

	std::ofstream ofs(filePath);
	ofs << data.dump(4);
}