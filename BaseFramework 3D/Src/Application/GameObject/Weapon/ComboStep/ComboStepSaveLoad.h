#pragma once
#include "ComboStep.h"

// ファイル1つ分の中身
struct ComboFileData
{
	std::string            m_charaModelPath;   // プレビュー用のキャラモデル（エディター専用）
	std::string            m_weaponModelPath;   // プレビュー用の武器モデル（エディター専用）
	std::vector<ComboStep> m_steps;
};

class ComboStepSaveLoad
{
public:
	// _path のファイルを読んで outData に入れる。失敗したら false（outData は変わらない）
	// _outData ... 読み込んだデータを入れる先
	static bool Load(const std::string& _path, ComboFileData& _outData);

	// _data を path に書き出す。失敗したら false
	// _path ... 書き出すファイルのパス
	static bool Save(const std::string& _path, const ComboFileData& _data);

private:
	//インスタンスは作らない
	ComboStepSaveLoad() {}

	// 段1つ分の変換
	// _j ... 段1つ分のjson
	static ComboStep      StepFromJson(const nlohmann::json& _j);
	//段1つ分をjsonにする
	// _step ... 段のデータ
	//戻り値 ... 段1つ分のjson
	static nlohmann::json StepToJson(const ComboStep& _step);
};
