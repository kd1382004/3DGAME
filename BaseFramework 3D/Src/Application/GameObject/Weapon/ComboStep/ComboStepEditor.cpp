#include "ComboStepEditor.h"
#include"ComboStepSaveLoad.h"

void ComboStepEditor::Draw(bool* _open)
{
	// 表示する名前：未保存なら末尾に「 *」を付ける
	// ### より後ろが ID。ここを固定しておけば、名前が変わっても同じウィンドウとして扱われる
	const std::string title =
		std::string("ComboStepEditor") + (m_isDirty ? " *" : "") + "###ComboStepEditor";



	if (!ImGui::Begin(title.c_str(), _open, ImGuiWindowFlags_MenuBar))
	{
		ImGui::End();
		return;
	}

	//問題を確認
	m_issues = ComboStepValidator::Validate(m_steps);


	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu(U8("ファイル")))
		{
			if (ImGui::MenuItem(U8("新規"))) { NewFile(); }
			if (ImGui::MenuItem(U8("開く"))) { OpenFile(); }
			if (ImGui::MenuItem(U8("保存"), "Ctrl+S", false, m_isDirty)) { Save(); }
			if (ImGui::MenuItem(U8("名前を付けて保存"))) { SaveAs(); }
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu(U8("編集")))
		{
			if (ImGui::MenuItem(U8("元に戻す"), "Ctrl+Z", false, !m_undoStack.empty())) { Undo(); }
			if (ImGui::MenuItem(U8("やり直す"), "Ctrl+Y", false, !m_redoStack.empty())) { Redo(); }
			ImGui::Separator();
			if (ImGui::MenuItem(U8("連番を振り直す"))) { RenumberComboIndex(); }
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu(U8("表示")))
		{
			ImGui::MenuItem(U8("プレビュー"), nullptr, &m_showPreview);
			ImGui::MenuItem(U8("検査結果"), nullptr, &m_showValidation);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	// 下に空けるプレビューの高さ（「表示」メニューで消したら 0）
	const float previewHeight = m_showPreview ? 150.0f : 0.0f;

	// ボタン1行分の高さ
	const float buttonsHeight = ImGui::GetFrameHeightWithSpacing();

	ImGui::BeginGroup();

	// 一覧（ボタンの分だけ下を空ける）
	if (ImGui::BeginChild("List", ImVec2(250.0f, -(previewHeight + buttonsHeight)), true))
	{
		if (ImGui::BeginTable("Steps", 3, ImGuiTableFlags_RowBg))
		{
			// 列の設定（列の数と同じ回数呼ぶ）
			ImGui::TableSetupColumn(U8("段"), ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn(U8("アニメーション名"), ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn(U8("状態"), ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableHeadersRow();   // 見出しの行を描く

			// 行は段の数だけ
			for (int i = 0; i < static_cast<int>(m_steps.size()); ++i)
			{
				// --- この段に問題があるかを調べるだけ（ここでは描かない）---
				bool hasError = false;
				bool hasWarning = false;

				//問題のメッセージ一覧
				std::string tooltip;
				for (const ComboStepIssue& issue : m_issues)
				{
					if (issue.stepIndex != i) { continue; }

					if (issue.isError)
					{
						hasError = true;
					}
					else
					{
						hasWarning = true;
					}

					// 2件目からは改行
					if (!tooltip.empty()) { tooltip += "\n"; }
					tooltip += issue.message;
				}


				const ComboStep& step = m_steps[i];

				ImGui::TableNextRow();
				ImGui::PushID(i);

				// 1列目：段（ここを Selectable にして行ごと選べるようにする）
				ImGui::TableNextColumn();
				if (ImGui::Selectable(std::to_string(step.comboIndex).c_str(),
					m_selected == i,
					ImGuiSelectableFlags_SpanAllColumns))
				{
					m_selected = i;
				}

				//問題メッセージがあったら書く
				if (!tooltip.empty())
				{
					ImGui::SetItemTooltip("%s", tooltip.c_str());
				}

				// 2列目：アニメーション名
				ImGui::TableNextColumn();
				ImGui::TextUnformatted(step.animeName.c_str());

				// 3列目：状態
				ImGui::TableNextColumn();
				if (hasError)
				{
					ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), U8("エラー"));
				}
				else if (hasWarning)
				{
					ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), U8("警告"));
				}

				ImGui::PopID();
			}
			ImGui::EndTable();
		}
	}
	ImGui::EndChild();

	// 一覧の下のボタン
	const bool hasSelect = (m_selected >= 0 && m_selected < static_cast<int>(m_steps.size()));

	if (ImGui::Button(U8("追加"))) { AddStep(); }
	ImGui::SameLine();

	ImGui::BeginDisabled(!hasSelect);   // 段を選んでいないときは押せない
	if (ImGui::Button(U8("複製"))) { DuplicateStep(); }
	ImGui::SameLine();
	if (ImGui::Button(U8("削除"))) { DeleteStep(); }
	ImGui::SameLine();
	if (ImGui::ArrowButton("##up", ImGuiDir_Up)) { MoveStep(-1); }
	ImGui::SameLine();
	if (ImGui::ArrowButton("##down", ImGuiDir_Down)) { MoveStep(+1); }
	ImGui::EndDisabled();

	ImGui::EndGroup();

	ImGui::SameLine();

	// 右：段の詳細（幅 0 = 残り全部）
	if (ImGui::BeginChild("Detail", ImVec2(0.0f, -previewHeight), true))
	{

		if (ImGui::InputText(U8("モデル"), &m_CharaModelPath))
		{
			m_isDirty = true;
		}

		//==============================
		// 検査結果（「表示」メニューでオンのとき）
		//==============================
		if (m_showValidation)
		{
			ImGui::SeparatorText(U8("検査結果"));

			if (m_issues.empty())
			{
				ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), U8("問題なし"));
			}
			else
			{
				for (int k = 0; k < static_cast<int>(m_issues.size()); ++k)
				{
					const ComboStepIssue& issue = m_issues[k];
					const ImVec4 col = issue.isError ? ImVec4(1.0f, 0.3f, 0.3f, 1.0f)
						: ImVec4(1.0f, 0.8f, 0.2f, 1.0f);

					ImGui::PushID(k);
					ImGui::PushStyleColor(ImGuiCol_Text, col);

					// クリックしたらその段を選ぶ（段のない問題は選べない）
					if (ImGui::Selectable(issue.message.c_str(), false) && issue.stepIndex >= 0)
					{
						m_selected = issue.stepIndex;
					}

					ImGui::PopStyleColor();
					ImGui::PopID();
				}
			}
		}

		if (m_selected >= 0 && m_selected < static_cast<int>(m_steps.size()))
		{
			ComboStep& selecSteps = m_steps[m_selected];

			// 部品が変更を返したら「未保存」にする
			auto mark = [&](bool _changed) { if (_changed) { m_isDirty = true; } };

			// 最小・最大の外に出さない（直接入力でも）
			constexpr ImGuiSliderFlags clamp = ImGuiSliderFlags_AlwaysClamp;

			ImGui::PushID(m_selected);   // 段が変わったら別の部品として扱う
			ImGui::PushItemWidth(200.0f); // 部品の幅（ラベルが切れないように）

			//==============================
			ImGui::SeparatorText(U8("基本"));
			//==============================

			// アニメーション名
			mark(ImGui::InputText(U8("アニメーション名"), &selecSteps.animeName));

			// コンボ番号
			if (ImGui::InputInt(U8("コンボ番号"), &selecSteps.comboIndex))
			{
				selecSteps.comboIndex = std::max(0, selecSteps.comboIndex);
				m_isDirty = true;
			}

			// 再生速度
			mark(ImGui::DragFloat(U8("再生速度"), &selecSteps.speed,
				0.01f, 0.1f, 3.0f, U8("%.2f 倍"), clamp));

			//==============================
			ImGui::SeparatorText(U8("タイミング（進行度 0〜1）"));
			//==============================

			// 最後の段か（acceptStart が 1 より大きければ最後の段とみなす）
			bool isLast = selecSteps.acceptStart > 1.0f;
			if (ImGui::Checkbox(U8("最後の段（次の段なし）"), &isLast))
			{
				if (isLast)
				{
					selecSteps.acceptStart = 9.9f;
					selecSteps.cancelPoint = 9.9f;
				}
				else
				{
					selecSteps.acceptStart = 0.5f;
					selecSteps.cancelPoint = 0.8f;
				}
				m_isDirty = true;
			}

			// 最後の段のときは受付・切替を触れないようにする
			ImGui::BeginDisabled(isLast);
			if (isLast)
			{
				// 9.9 をスライダーに出すと右端に張り付くので、表示だけ
				ImGui::TextDisabled(U8("受付開始・切替点：なし"));
			}
			else
			{
				mark(ImGui::SliderFloat(U8("受付開始"), &selecSteps.acceptStart,
					0.0f, 1.0f, "%.2f", clamp));
				mark(ImGui::SliderFloat(U8("切替点"), &selecSteps.cancelPoint,
					0.0f, 1.0f, "%.2f", clamp));
			}
			ImGui::EndDisabled();

			// 回避キャンセル
			mark(ImGui::SliderFloat(U8("回避キャンセル"), &selecSteps.evasionCancel,
				0.0f, 1.0f, "%.2f", clamp));

			// 当たり判定の範囲
			mark(ImGui::DragFloatRange2(U8("当たり判定"),
				&selecSteps.hitStart, &selecSteps.hitEnd,
				0.005f, 0.0f, 1.0f, U8("開始 %.2f"), U8("終了 %.2f"), clamp));

			// 軌跡の範囲
			mark(ImGui::DragFloatRange2(U8("軌跡"),
				&selecSteps.trailStart, &selecSteps.trailEnd,
				0.005f, 0.0f, 1.0f, U8("開始 %.2f"), U8("終了 %.2f"), clamp));

			//==============================
			ImGui::SeparatorText(U8("威力・手触り"));
			//==============================

			mark(ImGui::DragFloat(U8("ダメージ倍率"), &selecSteps.damageRate,
				0.01f, 0.0f, 10.0f, U8("%.2f 倍"), clamp));

			mark(ImGui::DragFloat(U8("ノックバック倍率"), &selecSteps.knockbackRate,
				0.01f, 0.0f, 10.0f, U8("%.2f 倍"), clamp));

			mark(ImGui::DragFloat(U8("ヒットストップ"), &selecSteps.hitStopTime,
				0.001f, 0.0f, 0.5f, U8("%.3f 秒"), clamp));

			mark(ImGui::DragFloat(U8("踏み込み距離"), &selecSteps.stepForward,
				0.01f, 0.0f, 5.0f, "%.2f m", clamp));

			mark(ImGui::DragFloat(U8("消費スタミナ"), &selecSteps.staminaCost,
				0.5f, 0.0f, 100.0f, "%.1f", clamp));

			ImGui::PopItemWidth();
			ImGui::PopID();
		}
		else
		{
			ImGui::TextUnformatted(U8("左の一覧から段を選んでください"));
		}
	}
	ImGui::EndChild();

	// 下：プレビュー（「表示」でオンのときだけ）
	if (m_showPreview)
	{
		if (ImGui::BeginChild("Preview", ImVec2(0.0f, 0.0f), true))
		{
			ImGui::Text(U8("プレビュー"));
		}
		ImGui::EndChild();
	}


	ImGui::End();
}


ComboStepEditor::ComboStepEditor()
{
	m_isDirty = false;


	m_filePath = kTestPath;

	ComboFileData data;
	if (ComboStepSaveLoad::Load(m_filePath, data))
	{
		m_steps = data.steps;
		m_CharaModelPath = data.charaModelPath;
	}
	else
	{
		CreateDummyData();
	}
	m_isDirty = false;
}

void ComboStepEditor::NewFile()
{}

void ComboStepEditor::OpenFile()
{}

void ComboStepEditor::Save()
{
	ComboFileData data;
	data.charaModelPath = m_CharaModelPath;
	data.steps = m_steps;
	if (ComboStepSaveLoad::Save(m_filePath, data))
	{
		m_isDirty = false;
	}
}

void ComboStepEditor::SaveAs()
{}

void ComboStepEditor::Undo()
{}

void ComboStepEditor::Redo()
{}

void ComboStepEditor::RenumberComboIndex()
{
	for (int i = 0; i < m_steps.size(); i++)
	{
		m_steps[i].comboIndex = i;
	}

	m_isDirty = true;
}

void ComboStepEditor::AddStep()
{
	ComboStep addComboStep;
	addComboStep.animeName = "New";
	m_steps.push_back(addComboStep);

	m_selected = m_steps.size() - 1;
	RenumberComboIndex();
}

void ComboStepEditor::DuplicateStep()
{
	if (m_selected < 0 || m_selected >= m_steps.size())
	{
		return;
	}
	ComboStep addComboStep;
	addComboStep = m_steps[m_selected];

	auto itr = m_steps.begin() + m_selected + 1;
	m_steps.insert(itr, addComboStep);

	m_selected++;
	RenumberComboIndex();
}

void ComboStepEditor::DeleteStep()
{
	if (m_selected < 0 || m_selected >= m_steps.size())
	{
		return;
	}

	auto itr = m_steps.begin() + m_selected;
	m_steps.erase(itr);



	if (m_steps.empty())
	{
		m_selected = -1;
	}
	else
	{
		int listSiz = m_steps.size();
		m_selected = std::min(m_selected, listSiz - 1);
	}

	RenumberComboIndex();
}

void ComboStepEditor::MoveStep(int _dir)
{

	const int num = static_cast<int>(m_steps.size());

	// 選んでいる段が範囲外なら何もしない
	if (m_selected < 0 || m_selected >= num)
	{
		return;
	}

	// 入れ替える相手が範囲外なら何もしない
	int changeIndex = m_selected + _dir;
	if (changeIndex < 0 || changeIndex >= num)
	{
		return;
	}

	std::swap(m_steps[m_selected], m_steps[changeIndex]);
	m_selected = changeIndex;
	RenumberComboIndex();
}

void ComboStepEditor::CreateDummyData()
{
	//仮データ
	m_steps.clear();

	// 1段目：軽い横斬り
	{
		ComboStep step;
		step.animeName = "Attack1";
		step.comboIndex = 0;
		step.speed = 1.0f;
		step.acceptStart = 0.4f;
		step.cancelPoint = 0.7f;
		step.evasionCancel = 0.5f;
		step.hitStart = 0.3f;
		step.hitEnd = 0.5f;
		step.trailStart = 0.25f;
		step.trailEnd = 0.55f;
		step.damageRate = 1.0f;
		step.knockbackRate = 1.0f;
		step.hitStopTime = 0.03f;
		step.stepForward = 0.3f;
		step.staminaCost = 0.0f;   // 1段目は攻撃開始時に別で消費する想定
		m_steps.push_back(step);
	}

	// 2段目：返しの斬り
	{
		ComboStep step;
		step.animeName = "Attack2";
		step.comboIndex = 1;
		step.speed = 1.1f;
		step.acceptStart = 0.45f;
		step.cancelPoint = 0.75f;
		step.evasionCancel = 0.55f;
		step.hitStart = 0.35f;
		step.hitEnd = 0.55f;
		step.trailStart = 0.3f;
		step.trailEnd = 0.6f;
		step.damageRate = 1.2f;
		step.knockbackRate = 1.0f;
		step.hitStopTime = 0.04f;
		step.stepForward = 0.4f;
		step.staminaCost = 10.0f;
		m_steps.push_back(step);
	}

	// 3段目（最後）：大きく振り下ろす
	{
		ComboStep step;
		step.animeName = "Attack3";
		step.comboIndex = 2;
		step.speed = 0.9f;
		// 最後の段なので acceptStart / cancelPoint は 9.9 のまま
		step.evasionCancel = 0.7f;
		step.hitStart = 0.4f;
		step.hitEnd = 0.6f;
		step.trailStart = 0.35f;
		step.trailEnd = 0.65f;
		step.damageRate = 1.8f;
		step.knockbackRate = 2.0f;
		step.hitStopTime = 0.08f;
		step.stepForward = 0.8f;
		step.staminaCost = 15.0f;
		m_steps.push_back(step);
	}
}
