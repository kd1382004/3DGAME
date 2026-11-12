#pragma once
#include "ComboStep.h"

// ファイル1つ分の中身
struct ComboFileData
{
	std::string            charaModelPath;   // プレビュー用のキャラモデル（エディター専用）
	std::string            weaponModelPath;   // プレビュー用の武器モデル（エディター専用）
	std::vector<ComboStep> steps;
};

class ComboStepSaveLoad
{
public:
	// path のファイルを読んで outData に入れる。失敗したら false（outData は変わらない）
	static bool Load(const std::string& _path, ComboFileData& _outData);

	// data を path に書き出す。失敗したら false
	static bool Save(const std::string& _path, const ComboFileData& _data);

private:
	ComboStepSaveLoad() {}

	// 段1つ分の変換
	static ComboStep      StepFromJson(const nlohmann::json& _j);
	static nlohmann::json StepToJson(const ComboStep& _step);
};
