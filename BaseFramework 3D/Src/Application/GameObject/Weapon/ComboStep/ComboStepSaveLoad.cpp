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
		result.charaModelPath = data["charaModelPath"].get<std::string>();
	}

	// 武器モデルのパス（なければ空のまま）
	if (data.contains("weaponModelPath") && data["weaponModelPath"].is_string())
	{
		result.weaponModelPath = data["weaponModelPath"].get<std::string>();
	}

	// 段
	for (const nlohmann::json& stepJson : data["steps"])
	{
		// 段ではないものは飛ばす
		if (!stepJson.is_object()) { continue; }

		result.steps.push_back(StepFromJson(stepJson));
	}

	// 番号順に並べておく
	std::stable_sort(result.steps.begin(), result.steps.end(),
		[](const ComboStep& _a, const ComboStep& _b) { return _a.comboIndex < _b.comboIndex; });

	// 全部うまくいったら入れ替える
	_outData = std::move(result);
	return true;
}

bool ComboStepSaveLoad::Save(const std::string& _path, const ComboFileData& _data)
{
	// 1. comboIndex の順に並べたコピーを作る
	std::vector<ComboStep> sorted = _data.steps;
	std::stable_sort(sorted.begin(), sorted.end(),
		[](const ComboStep& _a, const ComboStep& _b) { return _a.comboIndex < _b.comboIndex; });

	// 2. 組み立てる
	nlohmann::json stepsJson = nlohmann::json::array();
	for (const ComboStep& step : sorted)
	{
		stepsJson.push_back(StepToJson(step));
	}

	nlohmann::json data;
	data["version"] = 1;
	data["charaModelPath"] = _data.charaModelPath;
	data["weaponModelPath"] = _data.weaponModelPath;
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
		step.animeName = _j["animeName"].get<std::string>();
	}
	if (_j.contains("comboIndex") && _j["comboIndex"].is_number_integer())
	{
		step.comboIndex = _j["comboIndex"].get<int>();
	}

	getFloat("speed", step.speed);
	getFloat("acceptStart", step.acceptStart);
	getFloat("cancelPoint", step.cancelPoint);
	getFloat("evasionCancel", step.evasionCancel);
	getFloat("hitStart", step.hitStart);
	getFloat("hitEnd", step.hitEnd);
	getFloat("trailStart", step.trailStart);
	getFloat("trailEnd", step.trailEnd);
	getFloat("damageRate", step.damageRate);
	getFloat("knockbackRate", step.knockbackRate);
	getFloat("hitStopTime", step.hitStopTime);
	getFloat("stepForward", step.stepForward);
	getFloat("staminaCost", step.staminaCost);

	return step;
}

nlohmann::json ComboStepSaveLoad::StepToJson(const ComboStep& _step)
{

	nlohmann::json j;

	j["animeName"] = _step.animeName;
	j["comboIndex"] = _step.comboIndex;
	j["speed"] = _step.speed;
	j["acceptStart"] = _step.acceptStart;
	j["cancelPoint"] = _step.cancelPoint;
	j["evasionCancel"] = _step.evasionCancel;
	j["hitStart"] = _step.hitStart;
	j["hitEnd"] = _step.hitEnd;
	j["trailStart"] = _step.trailStart;
	j["trailEnd"] = _step.trailEnd;
	j["damageRate"] = _step.damageRate;
	j["knockbackRate"] = _step.knockbackRate;
	j["hitStopTime"] = _step.hitStopTime;
	j["stepForward"] = _step.stepForward;
	j["staminaCost"] = _step.staminaCost;

	return j;
}
