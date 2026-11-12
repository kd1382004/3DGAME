#include "ComboStepValidator.h"

std::vector<ComboStepIssue> ComboStepValidator::Validate(const std::vector<ComboStep>& _steps)
{
	std::vector<ComboStepIssue> comboStepIssue;

	CheckComboIndex(_steps, comboStepIssue);

	CheckTiming(_steps, comboStepIssue);

	return  comboStepIssue;
}

void ComboStepValidator::CheckComboIndex(const std::vector<ComboStep>& _steps, std::vector<ComboStepIssue>& _outIssues)
{
	//何段あるかどうか
	const int num = static_cast<int>(_steps.size());

	// users[番号] = その番号を使っている段の添字のリスト
	std::vector<std::vector<int>> users(num);

	// 範囲外を見る ＋ 範囲内なら誰が使っているかを記録
	for (int i = 0; i < num; ++i)
	{
		const int index = _steps[i].comboIndex;

		if (index < 0 || index >= num)
		{
			ComboStepIssue issue;
			issue.isError = true;
			issue.stepIndex = i;
			issue.message = std::to_string(index) + U8(" は範囲外です");
			_outIssues.push_back(issue);
			continue;
		}

		users[index].push_back(i);
	}

	// 欠番と重複を見る（ここの n はコンボ番号）
	for (int n = 0; n < num; ++n)
	{
		// 欠番：
		if (users[n].empty())
		{
			ComboStepIssue issue;
			issue.isError = true;
			issue.stepIndex = -1;
			issue.message = std::to_string(n) + U8(" 番が欠番です");
			_outIssues.push_back(issue);
		}
		// 重複
		else if (users[n].size() >= 2)
		{
			for (int stepIdx : users[n])
			{
				ComboStepIssue issue;
				issue.isError = true;
				issue.stepIndex = stepIdx;   // 配列の添字
				issue.message = std::to_string(n) + U8(" 番が重複しています");
				_outIssues.push_back(issue);
			}
		}
	}
}

void ComboStepValidator::CheckTiming(const std::vector<ComboStep>& _steps, std::vector<ComboStepIssue>& _outIssues)
{
	//何段あるかどうか
	const int num = static_cast<int>(_steps.size());
	if (num == 0) { return; }

	// 最後の段 = comboIndex が一番大きい段（配列の末尾ではない）
	int lastComboIndex = _steps[0].comboIndex;
	for (const ComboStep& step : _steps)
	{
		lastComboIndex = std::max(lastComboIndex, step.comboIndex);
	}

	for (int i = 0; i < num; i++)
	{
		const ComboStep& step = _steps[i];

		// メッセージの頭に付ける「1番 Attack2: 」
		const std::string head = std::to_string(step.comboIndex) + U8("番 ") + step.animeName + ": ";

		//問題を1件足す
		auto add = [&](bool _isError, const std::string& _msg)
			{
				ComboStepIssue issue;
				issue.isError = _isError;
				issue.stepIndex = i;
				issue.message = head + _msg;
				_outIssues.push_back(issue);
			};

		// 進行度が 0〜1 の外なら警告
		auto checkRange = [&](float _value, const char* _name)
			{
				if (_value < 0.0f || _value > 1.0f)
				{
					add(false, std::string(_name) + U8(" が 0〜1 の外です（") + std::to_string(_value) + U8("）"));
				}
			};

		const bool isLastStep = (step.comboIndex == lastComboIndex);
		const bool hasNext = (step.acceptStart <= 1.0f);   // 9.9 なら「次なし」


		//==============================
		// エラー
		//==============================

				// 速度
		if (step.speed <= 0.0f)
		{
			add(true, U8("再生速度が 0 以下です（アニメーションが進みません）"));
		}

		// 当たり判定
		if (step.hitStart > step.hitEnd)
		{
			add(true, U8("当たり判定の開始が終了より後です（判定が出ません）"));
		}

		// 軌跡
		if (step.trailStart > step.trailEnd)
		{
			add(true, U8("軌跡の開始が終了より後です（軌跡が出ません）"));
		}

		// 受付と切替の順番（次の段がある場合だけ）
		if (!isLastStep && hasNext && step.acceptStart > step.cancelPoint)
		{
			add(true, U8("受付開始が切替点より後です（予約しても次の段に進めません）"));
		}


		//==============================
		// 警告
		//==============================

		// 最後の段なのに次がある
		if (isLastStep && hasNext)
		{
			add(false, U8("最後の段なのに受付開始が設定されています（「最後の段」にチェックを入れてください）"));
		}

		// 最後の段ではないのに次がない
		if (!isLastStep && !hasNext)
		{
			add(false, U8("最後の段ではないのに次の段に進めません（「最後の段」のチェックを外してください）"));
		}

		// 範囲（受付・切替は次の段があるときだけ調べる）
		if (hasNext)
		{
			checkRange(step.acceptStart, "acceptStart");
			checkRange(step.cancelPoint, "cancelPoint");
		}

		checkRange(step.evasionCancel, "evasionCancel");
		checkRange(step.hitStart, "hitStart");
		checkRange(step.hitEnd, "hitEnd");
		checkRange(step.trailStart, "trailStart");
		checkRange(step.trailEnd, "trailEnd");

	}
}
