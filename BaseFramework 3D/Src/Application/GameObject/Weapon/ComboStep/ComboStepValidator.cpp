//---- 自分のヘッダー ----
#include "ComboStepValidator.h"		//コンボデータのチェック

std::vector<ComboStepIssue> ComboStepValidator::Validate(
	const std::vector<ComboStep>& _steps,
	const std::unordered_set<std::string>* _animeNames)
{
	std::vector<ComboStepIssue> comboStepIssue;

	CheckComboIndex(_steps, comboStepIssue);

	CheckTiming(_steps, comboStepIssue);

	CheckAnimeName(_steps, _animeNames, comboStepIssue);

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
		const int index = _steps[i].m_comboIndex;

		if (index < 0 || index >= num)
		{
			ComboStepIssue issue;
			issue.m_isError = true;
			issue.m_stepIndex = i;
			issue.m_message = std::to_string(index) + U8(" は範囲外です");
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
			issue.m_isError = true;
			issue.m_stepIndex = -1;
			issue.m_message = std::to_string(n) + U8(" 番が欠番です");
			_outIssues.push_back(issue);
		}
		// 重複
		else if (users[n].size() >= 2)
		{
			for (int stepIdx : users[n])
			{
				ComboStepIssue issue;
				issue.m_isError = true;
				issue.m_stepIndex = stepIdx;   // 配列の添字
				issue.m_message = std::to_string(n) + U8(" 番が重複しています");
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
	int lastComboIndex = _steps[0].m_comboIndex;
	for (const ComboStep& step : _steps)
	{
		lastComboIndex = std::max(lastComboIndex, step.m_comboIndex);
	}

	for (int i = 0; i < num; i++)
	{
		const ComboStep& step = _steps[i];

		// メッセージの頭に付ける「1番 Attack2: 」
		const std::string head = std::to_string(step.m_comboIndex) + U8("番 ") + step.m_animeName + ": ";

		//問題を1件足す
		auto add = [&](bool _isError, const std::string& _msg)
			{
				ComboStepIssue issue;
				issue.m_isError = _isError;
				issue.m_stepIndex = i;
				issue.m_message = head + _msg;
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

		const bool isLastStep = (step.m_comboIndex == lastComboIndex);
		const bool hasNext = (step.m_acceptStart <= 1.0f);   // 9.9 なら「次なし」


		//==============================
		// エラー
		//==============================

				// 速度
		if (step.m_speed <= 0.0f)
		{
			add(true, U8("再生速度が 0 以下です（アニメーションが進みません）"));
		}

		// 当たり判定
		if (step.m_hitStart > step.m_hitEnd)
		{
			add(true, U8("当たり判定の開始が終了より後です（判定が出ません）"));
		}

		// 軌跡
		if (step.m_trailStart > step.m_trailEnd)
		{
			add(true, U8("軌跡の開始が終了より後です（軌跡が出ません）"));
		}

		// 受付と切替の順番（次の段がある場合だけ）
		if (!isLastStep && hasNext && step.m_acceptStart > step.m_cancelPoint)
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
			checkRange(step.m_acceptStart, "acceptStart");
			checkRange(step.m_cancelPoint, "cancelPoint");
		}

		checkRange(step.m_evasionCancel, "evasionCancel");
		checkRange(step.m_hitStart, "hitStart");
		checkRange(step.m_hitEnd, "hitEnd");
		checkRange(step.m_trailStart, "trailStart");
		checkRange(step.m_trailEnd, "trailEnd");

	}
}

void ComboStepValidator::CheckAnimeName(const std::vector<ComboStep>& _steps,
	const std::unordered_set<std::string>* _animeNames, std::vector<ComboStepIssue>& _outIssues)
{
	for (int i = 0; i < static_cast<int>(_steps.size()); ++i)
	{
		const ComboStep& step = _steps[i];

		// 問題を1件足す
		auto add = [&](const std::string& _msg)
			{
				ComboStepIssue issue;
				issue.m_isError = true;
				issue.m_stepIndex = i;
				issue.m_message = std::to_string(step.m_comboIndex) + U8("番: ") + _msg;
				_outIssues.push_back(issue);
			};

		// 名前が空（一覧がなくても調べる）
		if (step.m_animeName.empty())
		{
			add(U8("アニメーション名が空です"));
			continue;
		}

		// モデルにない（一覧が渡されたときだけ調べる）
		if (_animeNames && _animeNames->count(step.m_animeName) == 0)
		{
			add(U8("モデルにアニメーション ") + step.m_animeName + U8(" がありません"));
		}
	}
}
