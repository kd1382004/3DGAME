#pragma once
#include"ComboStep.h"

// 問題1件分
struct ComboStepIssue
{
	bool        isError = true;    // true = エラー / false = 警告
	int         stepIndex = -1;    // 問題のある段の添字（m_steps の何番目か）。全体の問題なら -1
	std::string message;           // 画面に出す説明
};

class ComboStepValidator
{
public:
	// 全部の検査をまとめて行い、問題の一覧を返す（問題なしなら空）
	static std::vector<ComboStepIssue> Validate(const std::vector<ComboStep>& _steps);

private:
	//comboIndex の連番・重複
	static void CheckComboIndex(const std::vector<ComboStep>& _steps, std::vector<ComboStepIssue>& _outIssues);

	//進行度の範囲や順番など
	static void CheckTiming(const std::vector<ComboStep>& _steps, std::vector<ComboStepIssue>& _outIssues);

	ComboStepValidator() {}
};
