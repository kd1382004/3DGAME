#include "ComboStepSaveLoad.h"

bool ComboStepSaveLoad::Load(const std::string& _path, ComboFileData& _outData)
{
	std::ifstream file(_path);
	if (!file)
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] コンボファイルが開けません : %s\n"), _path.c_str());
		return false;
	}

	nlohmann::json data = nlohmann::json::parse(file, nullptr, false);
	if (data.is_discarded())
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] コンボファイルのJSONが壊れています : %s\n"), _path.c_str());
		return false;
	}

	// 形の確認：{ "steps": [ ... ] } になっているか
	if (!data.is_object() || !data.contains("steps") || !data["steps"].is_array())
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] コンボファイルに steps 配列がありません : %s\n"), _path.c_str());
		return false;
	}

	// 一時的な入れ物に詰める（_outData を壊さない為に）
	ComboFileData result;

	// モデルのパス（なければ空のまま。古いファイルにはない）
	if (data.contains("charaModelPath") && data["charaModelPath"].is_string())
	{
		result.m_charaModelPath = data["charaModelPath"].get<std::string>();
	}

	// 武器モデルのパス（なければ空のまま）
	if (data.contains("weaponModelPath") && data["weaponModelPath"].is_string())
	{
		result.m_weaponModelPath = data["weaponModelPath"].get<std::string>();
	}

	// 段
	for (const nlohmann::json& stepJson : data["steps"])
	{
		// 段ではないものは飛ばす
		if (!stepJson.is_object()) { continue; }

		result.m_steps.push_back(StepFromJson(stepJson));
	}

	// 番号順に並べておく
	std::stable_sort(result.m_steps.begin(), result.m_steps.end(),
		[](const ComboStep& _a, const ComboStep& _b) { return _a.m_comboIndex < _b.m_comboIndex; });

	// 全部うまくいったら入れ替える
	_outData = std::move(result);
	return true;
}

bool ComboStepSaveLoad::Save(const std::string& _path, const ComboFileData& _data)
{
	// 1. comboIndex の順に並べたコピーを作る
	std::vector<ComboStep> sorted = _data.m_steps;
	std::stable_sort(sorted.begin(), sorted.end(),
		[](const ComboStep& _a, const ComboStep& _b) { return _a.m_comboIndex < _b.m_comboIndex; });

	// 2. 組み立てる
	nlohmann::json stepsJson = nlohmann::json::array();
	for (const ComboStep& step : sorted)
	{
		stepsJson.push_back(StepToJson(step));
	}

	nlohmann::json data;
	data["version"] = 1;
	data["charaModelPath"] = _data.m_charaModelPath;
	data["weaponModelPath"] = _data.m_weaponModelPath;
	data["steps"] = std::move(stepsJson);

	// 3. 保存先のフォルダーがなければ作る（例：新しい持ち主の Goblin/ など）
	const std::filesystem::path path = _path;
	std::error_code ec;
	if (path.has_parent_path())
	{
		std::filesystem::create_directories(path.parent_path(), ec);
		if (ec)
		{
			KdDebugGUI::Instance().AddLog(U8("[Error] フォルダーが作れません : %s\n"), _path.c_str());
			return false;
		}
	}

	// 4. いったん .tmp に書き出す（途中で落ちても本物のファイルは無事）
	const std::string tmpPath = _path + ".tmp";
	{
		std::ofstream file(tmpPath);
		if (!file)
		{
			KdDebugGUI::Instance().AddLog(U8("[Error] コンボファイルに書き込めません : %s\n"), tmpPath.c_str());
			return false;
		}

		file << data.dump(1, '\t');

		if (!file)
		{
			KdDebugGUI::Instance().AddLog(U8("[Error] コンボファイルの書き込みに失敗しました : %s\n"), tmpPath.c_str());
			return false;
		}
	}

	// 5. .tmp を本物の名前に置き換える
	std::filesystem::rename(tmpPath, _path, ec);
	if (ec)
	{
		// 置き換え先が既にあって失敗した場合は、消してからもう一度
		std::filesystem::remove(_path, ec);
		std::filesystem::rename(tmpPath, _path, ec);
		if (ec)
		{
			KdDebugGUI::Instance().AddLog(U8("[Error] コンボファイルの置き換えに失敗しました : %s\n"), _path.c_str());
			return false;
		}
	}

	KdDebugGUI::Instance().AddLog(U8("コンボを保存しました : %s\n"), _path.c_str());
	return true;
}

ComboStep ComboStepSaveLoad::StepFromJson(const nlohmann::json& _j)
{
	ComboStep step;   // ここに入っている値が初期値になる

	auto getFloat = [&](const char* _key, float& _dst)
		{
			if (_j.contains(_key) && _j[_key].is_number()) { _dst = _j[_key].get<float>(); }
		};

	if (_j.contains("animeName") && _j["animeName"].is_string())
	{
		step.m_animeName = _j["animeName"].get<std::string>();
	}
	if (_j.contains("comboIndex") && _j["comboIndex"].is_number_integer())
	{
		step.m_comboIndex = _j["comboIndex"].get<int>();
	}

	getFloat("speed", step.m_speed);
	getFloat("acceptStart", step.m_acceptStart);
	getFloat("cancelPoint", step.m_cancelPoint);
	getFloat("evasionCancel", step.m_evasionCancel);
	getFloat("hitStart", step.m_hitStart);
	getFloat("hitEnd", step.m_hitEnd);
	getFloat("trailStart", step.m_trailStart);
	getFloat("trailEnd", step.m_trailEnd);
	getFloat("damageRate", step.m_damageRate);
	getFloat("knockbackRate", step.m_knockbackRate);
	getFloat("hitStopTime", step.m_hitStopTime);
	getFloat("stepForward", step.m_stepForward);
	getFloat("staminaCost", step.m_staminaCost);

	return step;
}

nlohmann::json ComboStepSaveLoad::StepToJson(const ComboStep& _step)
{

	nlohmann::json j;

	j["animeName"] = _step.m_animeName;
	j["comboIndex"] = _step.m_comboIndex;
	j["speed"] = _step.m_speed;
	j["acceptStart"] = _step.m_acceptStart;
	j["cancelPoint"] = _step.m_cancelPoint;
	j["evasionCancel"] = _step.m_evasionCancel;
	j["hitStart"] = _step.m_hitStart;
	j["hitEnd"] = _step.m_hitEnd;
	j["trailStart"] = _step.m_trailStart;
	j["trailEnd"] = _step.m_trailEnd;
	j["damageRate"] = _step.m_damageRate;
	j["knockbackRate"] = _step.m_knockbackRate;
	j["hitStopTime"] = _step.m_hitStopTime;
	j["stepForward"] = _step.m_stepForward;
	j["staminaCost"] = _step.m_staminaCost;

	return j;
}
