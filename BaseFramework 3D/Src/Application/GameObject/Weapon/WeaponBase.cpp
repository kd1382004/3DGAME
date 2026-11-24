//---- 自分のヘッダー ----
#include "WeaponBase.h"							//武器の基底クラス

//---- シーン ----
#include"../../Scene/SceneManager.h"			//シーンの管理

//---- キャラクター ----
#include"../Character/CharacterBase.h"			//キャラクターの基底クラス

//---- プレイヤー ----
#include"../Character/Player/PlayerBase.h"		//プレイヤー

//---- 武器 ----
#include"ComboStep/ComboStepSaveLoad.h"			//コンボの保存・読み込み
#include"ComboStep/ComboStepValidator.h"		//コンボデータのチェック

//=============================================
// 基本の処理
//=============================================

void WeaponBase::Init()
{
	LoadWeaponStatus(m_weaponStatusFilePath);
}

void WeaponBase::Update()
{
	m_mWorld = m_localMat * m_weaponParentMat;

	const bool prevPolyFlg = m_tPolyFlg;

	if (m_animePercentNow >= m_animePercentPolyStart)
	{
		m_tPolyFlg = true;
	}
	if (m_animePercentNow >= m_animePercentPolyEnd)
	{
		m_tPolyFlg = false;
	}

	// 軌跡が始まった・途切れたら、それまでの点を全部消す
	//（前の軌跡の最後の点と、次の軌跡の最初の点がつながらないように）
	if (prevPolyFlg != m_tPolyFlg && m_spTPoly)
	{
		m_spTPoly->ClearPoints();
	}
}

void WeaponBase::DrawLit()
{

}

void WeaponBase::DrawBright()
{
	if (m_attackFlg && m_tPolyFlg)
	{
		if (m_spTPoly)
		{
			KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_spTPoly);
		}
	}
}

//=============================================
// 取得
//=============================================

float WeaponBase::GetAttack()
{
	//武器倍率 × 強化倍率(1段階ごとに +20%)
	return m_baseWeaponStatus.m_attackPower * (1.0f + m_weaponsStrengtheningInfo[WeaponsStrengthening_Attack].m_lv * 0.2f);
}

//=============================================
// 設定
//=============================================

void WeaponBase::SetAttackFlg(bool _flg)
{
	m_attackFlg = _flg;

	if (m_attackFlg)
	{
		m_hitCharactersList.clear();
	}

}

void WeaponBase::AddAttackHitCharacterList(std::shared_ptr<CharacterBase> _spCharacter)
{
	//あたり判定リストにいれる
	m_attackHitCharacterList.push_back(_spCharacter);
}

void WeaponBase::AddObjList(std::shared_ptr<MapBase> _spObj)
{
	m_objList.push_back(_spObj);
}

//=============================================
// その他の処理
//=============================================

void WeaponBase::CreateWeaponHitCollider()
{

}

// origin: 判定の基準位置（通常はキャラの位置）
std::optional<Math::Vector3> WeaponBase::FindAimAssistTarget(const Math::Vector3& _origin) const
{
	std::optional<Math::Vector3> result;
	float nearestDistSq = m_aimAssistRadius * m_aimAssistRadius;

	for (const auto& wpObj : m_attackHitCharacterList)
	{
		auto spObj = wpObj.lock();
		if (!spObj || spObj->GetIsDead()) { continue; }

		Math::Vector3 diff = spObj->GetPos() - _origin;
		diff.y = 0.0f; // XZ平面で判定
		const float distSq = diff.LengthSquared();

		if (distSq < nearestDistSq)
		{
			nearestDistSq = distSq;
			result = spObj->GetPos();
		}
	}
	return result;
}

bool WeaponBase::LoadCombo(const std::string& _path)
{
	ComboFileData data;
	if (!ComboStepSaveLoad::Load(_path, data))
	{
		// 失敗の理由は Load の中でログに出ている。今のコンボはそのまま
		return false;
	}

	// 読み込んだ直後に検査して、問題があればログに出す（ゲームは止めない）
	for (const ComboStepIssue& issue : ComboStepValidator::Validate(data.m_steps))
	{
		KdDebugGUI::Instance().AddLog(U8("[%s] %s : %s\n"),
			issue.m_isError ? "Error" : "Warning", _path.c_str(), issue.m_message.c_str());
	}

	m_comboList = std::move(data.m_steps);
	return true;
}

void WeaponBase::LoadWeaponStatus(const std::string& _filePath)
{
	WeaponStatus ws;

	std::ifstream ifs(_filePath);
	if (!ifs.is_open())
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] 武器ステータスファイルが見つかりません : %s\n"), _filePath.c_str());
		return;
	}

	nlohmann::json data = nlohmann::json::parse(ifs, nullptr, false);

	if (data.is_discarded())
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] 武器ステータスファイルの読み込みに失敗しました : %s\n"), _filePath.c_str());
		return;
	}

	auto getFloat = [&](const nlohmann::json& _j, const char* _key, float _def = 0.0f)
		{
			if (_j.contains(_key) && _j[_key].is_number())
				return _j[_key].get<float>();
			return _def;
		};

	// --- 基本ステータス ---
	ws.m_attackPower = getFloat(data, "attackPower");
	ws.m_attackSpeed = getFloat(data, "attackSpeed", 1.0f);
	ws.m_critRate = getFloat(data, "critRate");
	ws.m_critDamage = getFloat(data, "critDamage");

	// --- 武器の性質 ---
	ws.m_range = getFloat(data, "range", 1.0f);
	ws.m_weight = getFloat(data, "weight", 1.0f);
	ws.m_knockback = getFloat(data, "knockback");
	ws.m_poiseBreak = getFloat(data, "poiseBreak");
	ws.m_stunPower = getFloat(data, "stunPower");

	// --- モーション補正 ---
	ws.m_startup = getFloat(data, "startup");
	ws.m_recovery = getFloat(data, "recovery");
	ws.m_cancelWindow = getFloat(data, "cancelWindow");

	m_baseWeaponStatus = ws;
}

void WeaponBase::SaveWeaponStatus(const std::string& _filePath)
{
	nlohmann::json data;

	// --- 基本ステータス ---
	data["attackPower"] = m_baseWeaponStatus.m_attackPower;
	data["attackSpeed"] = m_baseWeaponStatus.m_attackSpeed;
	data["critRate"] = m_baseWeaponStatus.m_critRate;
	data["critDamage"] = m_baseWeaponStatus.m_critDamage;

	// --- 武器の性質 ---
	data["range"] = m_baseWeaponStatus.m_range;
	data["weight"] = m_baseWeaponStatus.m_weight;
	data["knockback"] = m_baseWeaponStatus.m_knockback;
	data["poiseBreak"] = m_baseWeaponStatus.m_poiseBreak;
	data["stunPower"] = m_baseWeaponStatus.m_stunPower;

	// --- モーション補正 ---
	data["startup"] = m_baseWeaponStatus.m_startup;
	data["recovery"] = m_baseWeaponStatus.m_recovery;
	data["cancelWindow"] = m_baseWeaponStatus.m_cancelWindow;

	std::ofstream ofs(_filePath);
	ofs << data.dump(4);
}
