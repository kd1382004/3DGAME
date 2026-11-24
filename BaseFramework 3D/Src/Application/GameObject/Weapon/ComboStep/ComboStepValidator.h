#pragma once
#include"ComboStep.h"

// 問題1件分
struct ComboStepIssue
{
	bool        m_isError = true;    // true = エラー / false = 警告
	int         m_stepIndex = -1;    // 問題のある段の添字（m_steps の何番目か）。全体の問題なら -1
	std::string m_message;           // 画面に出す説明
};

//コンボデータのチェック
class ComboStepValidator
{
public:

	//---- その他の処理 ----

	// 全部の検査をまとめて行い、問題の一覧を返す（問題なしなら空）
	// _animeNames を渡すと、アニメーション名がモデルにあるかも検査する（nullptr なら飛ばす）
	// _steps ... 検査するコンボ
	static std::vector<ComboStepIssue> Validate(
		const std::vector<ComboStep>& _steps,
		const std::unordered_set<std::string>* _animeNames = nullptr);

private:

	//---- コンストラクタ・デストラクタ ----

	//インスタンスは作らない
	ComboStepValidator() {}

	//---- その他の処理 ----

	//comboIndex の連番・重複
	// _steps ... 検査するコンボ
	// _outIssues ... 問題を足す先
	static void CheckComboIndex(const std::vector<ComboStep>& _steps, std::vector<ComboStepIssue>& _outIssues);

	//進行度の範囲や順番など
	// _steps ... 検査するコンボ
	// _outIssues ... 問題を足す先
	static void CheckTiming(const std::vector<ComboStep>& _steps, std::vector<ComboStepIssue>& _outIssues);

	//アニメーション名（空・モデルにない）
	// _steps ... 検査するコンボ
	// _animeNames ... モデルにあるアニメーション名(nullptrなら飛ばす)
	// _outIssues ... 問題を足す先
	static void CheckAnimeName(const std::vector<ComboStep>& _steps,
		const std::unordered_set<std::string>* _animeNames, std::vector<ComboStepIssue>& _outIssues);
};
