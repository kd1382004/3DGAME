#include "ComboStepEditor.h"
#include"ComboStepSaveLoad.h"

void ComboStepEditor::Draw(bool* _open)
{
	// 表示する名前：未保存なら末尾に「 *」を付ける
	// ### より後ろが ID。ここを固定しておけば、名前が変わっても同じウィンドウとして扱われる
	//  開いているファイル名も出す（新規なら「（未保存の新規）」）
	const std::string fileLabel = m_filePath.empty() ? std::string(U8("（未保存の新規）"))
		: std::filesystem::path(m_filePath).lexically_relative(kComboRoot).generic_string();
	const std::string title =
		std::string("ComboStepEditor - ") + fileLabel + (m_isDirty ? " *" : "") + "###ComboStepEditor";



	// 「初期配置に戻す」が押されたら、左側に並べ直す
	const ImVec2 display = ImGui::GetIO().DisplaySize;
	if (m_resetLayout)
	{
		ImGui::SetNextWindowPos(ImVec2(10.0f, 40.0f), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(std::max(display.x - 530.0f, 400.0f), std::max(display.y - 80.0f, 300.0f)), ImGuiCond_Always);
	}

	if (!ImGui::Begin(title.c_str(), _open, ImGuiWindowFlags_MenuBar | GetLockFlags()))
	{
		ImGui::End();
		m_resetLayout = false;
		return;
	}

	//問題を確認
	// （一覧が空＝モデル未設定なら、アニメーション名の検査は飛ばす）
	m_issues = ComboStepValidator::Validate(m_steps, m_animeList.empty() ? nullptr : &m_animeNames);

	//==============================
	// 元に戻す用：何も操作していないフレームで、今のデータを覚えておく
	//  ドラッグや文字入力が終わったフレームで、操作前のデータを履歴に積む
	//  （1回のドラッグが履歴1つ分になる）
	//==============================
	if (!ImGui::IsAnyItemActive())
	{
		if (m_editChanged)
		{
			PushUndoState(m_stableSteps);
			m_editChanged = false;
		}
		m_stableSteps = m_steps;
	}

	//==============================
	// ×で閉じようとしたとき、未保存なら確認を出す
	//==============================
	if (_open && !*_open && m_isDirty)
	{
		*_open = true;                // いったん閉じるのを取り消す
		m_requestClosePopup = true;
	}

	//==============================
	// ショートカット（このウィンドウが選ばれていて、文字入力中でないとき）
	//==============================
	{
		const ImGuiIO& io = ImGui::GetIO();
		if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !io.WantTextInput)
		{
			if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S, false)) { Save(); }
			if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Z, false)) { io.KeyShift ? Redo() : Undo(); }
			if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Y, false)) { Redo(); }
			if (!io.KeyCtrl && m_showPreview && ImGui::IsKeyPressed(ImGuiKey_Space, false)) { TogglePreviewPlay(); }
		}
	}

	// プレビューを進める（ゲームのヒットストップやスローの影響を受けないよう、ImGui の実時間を使う）
	if (m_showPreview)
	{
		// 手動モードの入力（文字入力中は反応させない）
		//（Space は再生・一時停止に使うので、手動の予約は J キー）
		m_previewKeyPushed = !ImGui::GetIO().WantTextInput && ImGui::IsKeyPressed(ImGuiKey_J, false);
		UpdatePreview(ImGui::GetIO().DeltaTime * m_previewPlaySpeed);
		UpdatePreviewObject();
	}


	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu(U8("ファイル")))
		{
			if (ImGui::MenuItem(U8("新規"))) { NewFile(); }
			if (ImGui::MenuItem(U8("開く"))) { OpenFile(); }
			if (ImGui::MenuItem(U8("保存"), "Ctrl+S", false, m_isDirty || m_filePath.empty())) { Save(); }
			if (ImGui::MenuItem(U8("名前を付けて保存"))) { SaveAs(); }
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu(U8("編集")))
		{
			if (ImGui::MenuItem(U8("元に戻す"), "Ctrl+Z", false, !m_undoStack.empty())) { Undo(); }
			if (ImGui::MenuItem(U8("やり直す"), "Ctrl+Y", false, !m_redoStack.empty())) { Redo(); }
			ImGui::Separator();
			if (ImGui::MenuItem(U8("連番を振り直す"))) { PushUndo(); RenumberComboIndex(); }
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu(U8("表示")))
		{
			ImGui::MenuItem(U8("プレビュー"), nullptr, &m_showPreview);
			ImGui::MenuItem(U8("検査結果"), nullptr, &m_showValidation);
			ImGui::Separator();
			ImGui::MenuItem(U8("ウィンドウの位置を固定"), nullptr, &m_lockWindows);
			if (ImGui::MenuItem(U8("ウィンドウを初期配置に戻す"))) { m_resetLayout = true; }
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	// メニューの外で、ポップアップを開いて描く（メニューの中で OpenPopup すると開かない）
	DrawFilePopups();

	// 下に空けるプレビューの高さ（「表示」メニューで消したら 0）
	const float previewHeight = m_showPreview ? 190.0f : 0.0f;

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
		// 入力を確定したとき（Enter・他をクリック）だけ読み直す
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			LoadPreviewModel();
		}
		ImGui::SameLine();
		ImGui::TextDisabled(U8("（アニメーション %d 個）"), static_cast<int>(m_animeList.size()));

		// 武器モデル（プレビューで手に持たせる）
		if (ImGui::InputText(U8("武器モデル"), &m_WeaponModelPath))
		{
			m_isDirty = true;
		}
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			LoadPreviewWeapon();
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
			auto mark = [&](bool _changed) { if (_changed) { MarkEdited(); } };

			// 最小・最大の外に出さない（直接入力でも）
			constexpr ImGuiSliderFlags clamp = ImGuiSliderFlags_AlwaysClamp;

			ImGui::PushID(m_selected);   // 段が変わったら別の部品として扱う
			ImGui::PushItemWidth(200.0f); // 部品の幅（ラベルが切れないように）

			//==============================
			ImGui::SeparatorText(U8("基本"));
			//==============================

			// アニメーション名
			DrawAnimeNameSelect(selecSteps);

			// コンボ番号
			if (ImGui::InputInt(U8("コンボ番号"), &selecSteps.comboIndex))
			{
				selecSteps.comboIndex = std::max(0, selecSteps.comboIndex);
				MarkEdited();
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
				MarkEdited();
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
			DrawPreview();
		}
		ImGui::EndChild();
	}


	// 「保存して閉じる」「保存せずに閉じる」が選ばれたら、ここで閉じる
	if (m_closeNow)
	{
		if (_open) { *_open = false; }
		m_closeNow = false;
	}

	ImGui::End();

	// モデルを映す専用ウィンドウ（エディター本体とは別のウィンドウ）
	if (m_showPreview && m_showPreviewModel)
	{
		DrawPreviewWindow();
	}

	// 並べ直しは1フレームだけ
	m_resetLayout = false;
}


ComboStepEditor::ComboStepEditor()
{
	m_isDirty = false;


	// 最初はプレイヤーの剣のコンボを開く。なければ仮データ（保存先は同じ場所）
	if (!LoadFromFile(kTestPath))
	{
		m_filePath = kTestPath;
		CreateDummyData();

		// モデルのパスが決まってから、アニメーション一覧を作る
		LoadPreviewModel();
		LoadPreviewWeapon();
	}

	m_isDirty = false;
}

void ComboStepEditor::NewFile()
{
	m_requestNewPopup = true;
}

void ComboStepEditor::OpenFile()
{
	RefreshFileList();
	m_requestOpenPopup = true;
}

void ComboStepEditor::Save()
{
	// 新規（まだファイルがない）なら、名前を付けて保存へ
	if (m_filePath.empty())
	{
		SaveAs();
		return;
	}
	RequestSave(m_filePath);
}

void ComboStepEditor::RequestSave(const std::string& _path)
{
	// 検査のエラーがあれば「それでも保存しますか？」を出す
	if (CountErrors() > 0)
	{
		m_pendingSavePath = _path;
		m_requestSaveErrorPopup = true;
		return;
	}
	SaveToFile(_path);
}

void ComboStepEditor::SaveAs()
{
	// 今のファイルの「持ち主/武器」を初期値にする
	if (!m_filePath.empty())
	{
		const std::filesystem::path path(m_filePath);
		m_saveAsOwner = path.parent_path().filename().string();
		m_saveAsWeapon = path.stem().string();
	}
	m_requestSaveAsPopup = true;
}

bool ComboStepEditor::LoadFromFile(const std::string& _path)
{
	ComboFileData data;
	if (!ComboStepSaveLoad::Load(_path, data))
	{
		return false;   // 失敗しても今のデータはそのまま
	}

	m_steps = data.steps;
	m_CharaModelPath = data.charaModelPath;
	m_WeaponModelPath = data.weaponModelPath;
	m_filePath = std::filesystem::path(_path).lexically_normal().generic_string();

	// 前のファイルの状態を持ち越さない
	m_selected = -1;
	m_undoStack.clear();
	m_redoStack.clear();
	m_preview = PreviewState();
	m_isDirty = false;

	LoadPreviewModel();
	LoadPreviewWeapon();
	return true;
}

bool ComboStepEditor::SaveToFile(const std::string& _path)
{
	ComboFileData data;
	data.charaModelPath = m_CharaModelPath;
	data.weaponModelPath = m_WeaponModelPath;
	data.steps = m_steps;

	if (!ComboStepSaveLoad::Save(_path, data))
	{
		return false;   // 失敗したら「*」は残したまま
	}

	m_filePath = std::filesystem::path(_path).lexically_normal().generic_string();
	m_isDirty = false;

	// ゲームに知らせる（プレイヤーが使っているファイルなら読み直される）
	if (m_onSaved) { m_onSaved(m_filePath); }
	return true;
}

void ComboStepEditor::RefreshFileList()
{
	m_fileList.clear();
	m_fileListSelected = -1;

	std::error_code ec;
	if (!std::filesystem::exists(kComboRoot, ec)) { return; }

	for (const auto& entry : std::filesystem::recursive_directory_iterator(kComboRoot, ec))
	{
		if (!entry.is_regular_file()) { continue; }
		if (entry.path().extension() != ".json") { continue; }

		m_fileList.push_back(entry.path().lexically_relative(kComboRoot).generic_string());
	}
	std::sort(m_fileList.begin(), m_fileList.end());
}

void ComboStepEditor::DrawFilePopups()
{
	//==============================
	// 開く
	//==============================
	if (m_requestOpenPopup)
	{
		ImGui::OpenPopup(U8("ファイルを開く###ComboOpen"));
		m_requestOpenPopup = false;
	}
	ImGui::SetNextWindowSize(ImVec2(420.0f, 360.0f), ImGuiCond_Appearing);
	if (ImGui::BeginPopupModal(U8("ファイルを開く###ComboOpen")))
	{
		ImGui::TextDisabled("%s/", kComboRoot.c_str());

		if (m_isDirty)
		{
			ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), U8("未保存の変更があります"));
		}

		bool doOpen = false;
		const float listH = -ImGui::GetFrameHeightWithSpacing();
		if (ImGui::BeginChild("Files", ImVec2(0.0f, listH), true))
		{
			if (m_fileList.empty())
			{
				ImGui::TextDisabled(U8("ファイルがありません"));
			}
			for (int i = 0; i < static_cast<int>(m_fileList.size()); ++i)
			{
				if (ImGui::Selectable(m_fileList[i].c_str(), m_fileListSelected == i, ImGuiSelectableFlags_AllowDoubleClick))
				{
					m_fileListSelected = i;
					// ダブルクリックで開く（未保存の変更があるときは、下のボタンで選んでもらう）
					if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && !m_isDirty) { doOpen = true; }
				}
			}
		}
		ImGui::EndChild();

		const bool hasSelect = (m_fileListSelected >= 0 && m_fileListSelected < static_cast<int>(m_fileList.size()));
		if (m_isDirty)
		{
			// 新規でまだファイルがないときは「保存して開く」は使えない
			ImGui::BeginDisabled(!hasSelect || m_filePath.empty());
			if (ImGui::Button(U8("保存して開く")))
			{
				if (SaveToFile(m_filePath)) { doOpen = true; }
			}
			ImGui::EndDisabled();
			ImGui::SameLine();
			ImGui::BeginDisabled(!hasSelect);
			if (ImGui::Button(U8("保存せずに開く"))) { doOpen = true; }
			ImGui::EndDisabled();
		}
		else
		{
			ImGui::BeginDisabled(!hasSelect);
			if (ImGui::Button(U8("開く"))) { doOpen = true; }
			ImGui::EndDisabled();
		}
		ImGui::SameLine();
		if (ImGui::Button(U8("キャンセル"))) { ImGui::CloseCurrentPopup(); }

		if (doOpen && hasSelect)
		{
			LoadFromFile(kComboRoot + "/" + m_fileList[m_fileListSelected]);
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	//==============================
	// 名前を付けて保存
	//==============================
	if (m_requestSaveAsPopup)
	{
		ImGui::OpenPopup(U8("名前を付けて保存###ComboSaveAs"));
		m_requestSaveAsPopup = false;
	}
	if (ImGui::BeginPopupModal(U8("名前を付けて保存###ComboSaveAs"), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::SetNextItemWidth(200.0f);
		ImGui::InputText(U8("持ち主（フォルダー）"), &m_saveAsOwner);
		ImGui::SetNextItemWidth(200.0f);
		ImGui::InputText(U8("武器名（ファイル名）"), &m_saveAsWeapon);

		const std::string path = kComboRoot + "/" + m_saveAsOwner + "/" + m_saveAsWeapon + ".json";
		ImGui::TextDisabled(U8("保存先：%s"), path.c_str());

		const bool valid = !m_saveAsOwner.empty() && !m_saveAsWeapon.empty();
		std::error_code ec;
		if (valid && std::filesystem::exists(path, ec))
		{
			ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), U8("同じ名前のファイルがあります。上書きします"));
		}

		ImGui::BeginDisabled(!valid);
		if (ImGui::Button(U8("保存")))
		{
			ImGui::CloseCurrentPopup();
			RequestSave(path);   // エラーがあれば、次のフレームで確認を出す
		}
		ImGui::EndDisabled();
		ImGui::SameLine();
		if (ImGui::Button(U8("キャンセル"))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}

	//==============================
	// 新規
	//==============================
	if (m_requestNewPopup)
	{
		ImGui::OpenPopup(U8("新規作成###ComboNew"));
		m_requestNewPopup = false;
	}
	if (ImGui::BeginPopupModal(U8("新規作成###ComboNew"), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (m_isDirty)
		{
			ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), U8("未保存の変更があります"));
		}
		ImGui::TextUnformatted(U8("段を1つだけの新しいコンボにします（モデルの設定はそのまま）"));

		bool doNew = false;
		if (m_isDirty)
		{
			ImGui::BeginDisabled(m_filePath.empty());
			if (ImGui::Button(U8("保存して新規")))
			{
				if (SaveToFile(m_filePath)) { doNew = true; }
			}
			ImGui::EndDisabled();
			ImGui::SameLine();
			if (ImGui::Button(U8("保存せずに新規"))) { doNew = true; }
		}
		else
		{
			if (ImGui::Button(U8("新規にする"))) { doNew = true; }
		}

		if (doNew)
		{
			m_steps.clear();
			ComboStep step;
			step.animeName = m_animeList.empty() ? std::string("New") : m_animeList.front().name;
			m_steps.push_back(step);   // 初期値のまま＝最後の段（9.9）

			m_filePath.clear();        // まだファイルはない（保存すると「名前を付けて保存」になる）
			m_selected = 0;
			m_undoStack.clear();
			m_redoStack.clear();
			m_preview = PreviewState();
			m_isDirty = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button(U8("キャンセル"))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}

	//==============================
	// 保存前の検査の確認
	//==============================
	if (m_requestSaveErrorPopup)
	{
		ImGui::OpenPopup(U8("検査エラーがあります###ComboSaveError"));
		m_requestSaveErrorPopup = false;
	}
	if (ImGui::BeginPopupModal(U8("検査エラーがあります###ComboSaveError"), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text(U8("検査のエラーが %d 件あります。それでも保存しますか？"), CountErrors());

		// エラーの中身を少しだけ見せる（多いときは先頭の5件）
		int shown = 0;
		for (const ComboStepIssue& issue : m_issues)
		{
			if (!issue.isError) { continue; }
			ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s", issue.message.c_str());
			if (++shown >= 5) { break; }
		}

		if (ImGui::Button(U8("それでも保存")))
		{
			SaveToFile(m_pendingSavePath);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button(U8("キャンセル"))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}

	//==============================
	// 閉じる前の確認
	//==============================
	if (m_requestClosePopup)
	{
		ImGui::OpenPopup(U8("保存しますか？###ComboClose"));
		m_requestClosePopup = false;
	}
	if (ImGui::BeginPopupModal(U8("保存しますか？###ComboClose"), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::TextUnformatted(U8("未保存の変更があります"));
		ImGui::TextDisabled(U8("（閉じても編集中の内容はエディターに残ります。ゲームを終了すると消えます）"));

		ImGui::BeginDisabled(m_filePath.empty());
		if (ImGui::Button(U8("保存して閉じる")))
		{
			if (SaveToFile(m_filePath)) { m_closeNow = true; }
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndDisabled();
		ImGui::SameLine();
		if (ImGui::Button(U8("保存せずに閉じる")))
		{
			m_closeNow = true;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button(U8("キャンセル"))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}
}

void ComboStepEditor::PushUndoState(const std::vector<ComboStep>& _state)
{
	m_undoStack.push_back(_state);

	// 古いものから捨てる
	if (m_undoStack.size() > kUndoMax)
	{
		m_undoStack.erase(m_undoStack.begin());
	}

	// 新しく変更したら、やり直しの履歴は使えなくなる
	m_redoStack.clear();
}

void ComboStepEditor::Undo()
{
	if (m_undoStack.empty()) { return; }

	m_redoStack.push_back(m_steps);
	m_steps = m_undoStack.back();
	m_undoStack.pop_back();

	// 選んでいる段が範囲外になっていたら直す
	if (m_selected >= static_cast<int>(m_steps.size()))
	{
		m_selected = static_cast<int>(m_steps.size()) - 1;
	}

	m_stableSteps = m_steps;
	m_editChanged = false;
	m_isDirty = true;
}

void ComboStepEditor::Redo()
{
	if (m_redoStack.empty()) { return; }

	m_undoStack.push_back(m_steps);
	m_steps = m_redoStack.back();
	m_redoStack.pop_back();

	if (m_selected >= static_cast<int>(m_steps.size()))
	{
		m_selected = static_cast<int>(m_steps.size()) - 1;
	}

	m_stableSteps = m_steps;
	m_editChanged = false;
	m_isDirty = true;
}

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
	PushUndo();
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
	PushUndo();

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

	PushUndo();

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

	PushUndo();

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

void ComboStepEditor::LoadPreviewModel()
{
	// 失敗したときに古い一覧が残らないよう、先に空にする
	m_spPreviewModel = nullptr;
	m_spPreviewObj->SetModel(nullptr);
	m_animeList.clear();
	m_animeNames.clear();

	if (m_CharaModelPath.empty()) { return; }

	// ファイルがないと、モデルの読み込み（テクスチャなど）がアサートで止まるので先に確かめる
	std::error_code ec;
	if (!std::filesystem::is_regular_file(m_CharaModelPath, ec))
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] プレビュー用モデルのファイルがありません : %s\n"), m_CharaModelPath.c_str());
		return;
	}

	auto spModel = std::make_shared<KdModelData>();
	if (!spModel->Load(m_CharaModelPath))
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] プレビュー用モデルが読み込めません : %s\n"), m_CharaModelPath.c_str());
		return;
	}

	// GetAnimation は範囲外の番号で nullptr を返すので、そこまで回す
	for (UINT i = 0; ; ++i)
	{
		const std::shared_ptr<KdAnimationData> spAnime = spModel->GetAnimation(i);
		if (!spAnime) { break; }

		AnimeInfo info;
		info.name = spAnime->m_name;
		info.length = spAnime->m_maxLength;
		m_animeList.push_back(info);
		m_animeNames.insert(info.name);
	}

	// 名前順に並べる（選ぶときに探しやすい）
	std::sort(m_animeList.begin(), m_animeList.end(),
		[](const AnimeInfo& _a, const AnimeInfo& _b) { return _a.name < _b.name; });

	m_spPreviewModel = spModel;
	m_spPreviewObj->SetModel(spModel);

	KdDebugGUI::Instance().AddLog(U8("アニメーションを %d 個読み込みました : %s\n"),
		static_cast<int>(m_animeList.size()), m_CharaModelPath.c_str());
}

void ComboStepEditor::DrawAnimeNameSelect(ComboStep& _step)
{
	// 一覧が空（モデル未設定・読み込み失敗）なら、手で打てるように InputText のまま
	if (m_animeList.empty())
	{
		if (ImGui::InputText(U8("アニメーション名"), &_step.animeName))
		{
			MarkEdited();
		}
		return;
	}

	// 今の名前がモデルにあるか
	const bool found = (m_animeNames.count(_step.animeName) > 0);

	// 閉じているときに見える文字（見つからないなら印を付ける）
	const std::string preview = found ? _step.animeName
		: _step.animeName + U8("（見つからない）");

	if (!found) { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f)); }
	const bool opened = ImGui::BeginCombo(U8("アニメーション名"), preview.c_str());
	if (!found) { ImGui::PopStyleColor(); }

	if (opened)
	{
		for (int i = 0; i < static_cast<int>(m_animeList.size()); ++i)
		{
			const AnimeInfo& info = m_animeList[i];
			const bool isSelected = (info.name == _step.animeName);

			ImGui::PushID(i);
			if (ImGui::Selectable(info.name.c_str(), isSelected))
			{
				_step.animeName = info.name;
				MarkEdited();
			}
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();   // 開いたときにここが見えるように
			}

			// 右に長さを薄く出す
			ImGui::SameLine();
			ImGui::TextDisabled("(%.2f)", info.length);
			ImGui::PopID();
		}
		ImGui::EndCombo();
	}
}

//==============================
// 連続再生プレビュー（第6章）
//==============================

float ComboStepEditor::GetAnimeLength(const std::string& _name) const
{
	for (const AnimeInfo& info : m_animeList)
	{
		if (info.name == _name) { return info.length; }
	}
	return 0.0f;
}

void ComboStepEditor::StartPreviewStep(int _stepIdx)
{
	m_preview.stepIdx = _stepIdx;
	m_preview.time = 0.0f;
	m_preview.reserved = false;
	m_preview.hitStopTimer = 0.0f;
	m_preview.hitStopDone = false;
}

void ComboStepEditor::UpdatePreview(float _dt)
{
	if (!m_preview.playing) { return; }

	// 段が消されたなどで範囲外になったら止める
	if (m_preview.stepIdx < 0 || m_preview.stepIdx >= static_cast<int>(m_steps.size()))
	{
		m_preview.playing = false;
		m_preview.stepIdx = -1;
		return;
	}

	// ヒットストップ中は時間を進めない（DeltaTime::HitStop と同じ考え方）
	if (m_preview.hitStopTimer > 0.0f)
	{
		m_preview.hitStopTimer -= _dt;
		return;
	}

	const ComboStep& step = m_steps[m_preview.stepIdx];
	const float length = GetAnimeLength(step.animeName);
	if (length <= 0.0f)
	{
		m_preview.playing = false;
		m_preview.lastEvent = U8("アニメーションが見つからないので止めました：") + step.animeName;
		return;
	}

	// KdAnimator::AdvanceTime と同じ進め方：1秒で 100 × speed フレーム
	m_preview.time += _dt * kAnimeFramePerSec * step.speed;
	const float progress = m_preview.time / length;

	// 当たり判定の開始で、当たったことにしてヒットストップ
	if (m_previewHitStop && !m_preview.hitStopDone && progress >= step.hitStart)
	{
		m_preview.hitStopDone = true;
		m_preview.hitStopTimer = step.hitStopTime;
	}

	// 入力の予約（ゲームと同じく、受付開始より前の入力は無視）
	if (!m_preview.reserved && progress >= step.acceptStart)
	{
		bool push = false;
		switch (m_previewInputMode)
		{
		case PreviewInput_Fastest: push = true; break;
		case PreviewInput_Latest:  push = (progress >= step.cancelPoint - 0.02f); break;
		case PreviewInput_Manual:  push = m_previewKeyPushed; break;
		default: break;
		}
		if (push)
		{
			m_preview.reserved = true;
		}
	}

	// 予約があって切替点を過ぎ、次の段があれば次へ（ゲームと同じく配列の次）
	if (m_preview.reserved && progress >= step.cancelPoint &&
		m_preview.stepIdx + 1 < static_cast<int>(m_steps.size()))
	{
		m_preview.lastEvent = std::to_string(step.comboIndex) + U8("番 → 次の段へ");
		StartPreviewStep(m_preview.stepIdx + 1);
		return;
	}

	// 予約がないまま最後まで再生したら終了
	if (m_preview.time >= length)
	{
		if (m_previewLoop)
		{
			m_preview.lastEvent = U8("コンボ終了 → 1段目から");
			StartPreviewStep(0);
		}
		else
		{
			m_preview.lastEvent = std::to_string(step.comboIndex) + U8("番でコンボ終了");
			m_preview.playing = false;
			m_preview.time = length;   // 最後の位置で止めておく
		}
	}
}

void ComboStepEditor::DrawPreview()
{
	//==============================
	// 操作
	//==============================
	const bool canPlay = !m_steps.empty() && !m_animeList.empty();

	ImGui::BeginDisabled(!canPlay);
	if (ImGui::Button(m_preview.playing ? U8("一時停止（Space）") : U8("再生（Space）")))
	{
		TogglePreviewPlay();
	}
	ImGui::SameLine();
	if (ImGui::Button(U8("最初から")))
	{
		StartPreviewStep(0);
		m_preview.lastEvent.clear();
		m_preview.playing = true;
	}
	ImGui::SameLine();
	if (ImGui::Button(U8("選んだ段から")) && m_selected >= 0 && m_selected < static_cast<int>(m_steps.size()))
	{
		StartPreviewStep(m_selected);
		m_preview.lastEvent.clear();
		m_preview.playing = true;
	}
	ImGui::SameLine();
	// 一時停止中に 1/60 秒だけ進める
	ImGui::BeginDisabled(m_preview.playing || m_preview.stepIdx < 0);
	if (ImGui::Button(U8("コマ送り")))
	{
		m_preview.playing = true;
		UpdatePreview(1.0f / 60.0f);
		m_preview.playing = false;
	}
	ImGui::EndDisabled();
	ImGui::EndDisabled();

	ImGui::SameLine();
	ImGui::SetNextItemWidth(120.0f);
	ImGui::SliderFloat(U8("倍速"), &m_previewPlaySpeed, 0.1f, 2.0f, "%.1f");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(160.0f);
	const char* inputModes[] = { U8("自動（最速）"), U8("自動（ギリギリ）"), U8("手動（J キー）") };
	ImGui::Combo(U8("入力"), &m_previewInputMode, inputModes, IM_ARRAYSIZE(inputModes));
	ImGui::SameLine();
	ImGui::Checkbox(U8("ループ"), &m_previewLoop);
	ImGui::SameLine();
	ImGui::Checkbox(U8("ヒットストップ"), &m_previewHitStop);

	ImGui::SameLine();
	ImGui::Checkbox(U8("モデルのウィンドウ"), &m_showPreviewModel);

	if (!canPlay)
	{
		ImGui::TextDisabled(U8("モデルを設定して、段を1つ以上作ると再生できます"));
		return;
	}

	//==============================
	// 今の状態
	//==============================
	if (m_preview.stepIdx >= 0 && m_preview.stepIdx < static_cast<int>(m_steps.size()))
	{
		const ComboStep& step = m_steps[m_preview.stepIdx];
		const float length = GetAnimeLength(step.animeName);
		const float progress = (length > 0.0f) ? m_preview.time / length : 0.0f;

		ImGui::Text(U8("%d番 %s　進行度 %.2f　予約 %s%s"),
			step.comboIndex, step.animeName.c_str(), progress,
			m_preview.reserved ? U8("あり") : U8("なし"),
			m_preview.hitStopTimer > 0.0f ? U8("　[ヒットストップ中]") : "");

		// 回避で切れるか
		ImGui::SameLine();
		if (progress >= step.evasionCancel)
		{
			ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), U8("　回避で切れる"));
		}
		else
		{
			ImGui::TextDisabled(U8("　回避不可"));
		}
	}
	else
	{
		ImGui::TextDisabled(U8("停止中"));
	}
	if (!m_preview.lastEvent.empty())
	{
		ImGui::SameLine();
		ImGui::TextDisabled(U8("　／ %s"), m_preview.lastEvent.c_str());
	}

	//==============================
	// タイムライン
	//==============================
	DrawComboTimeline();
}

void ComboStepEditor::DrawComboTimeline()
{
	const int num = static_cast<int>(m_steps.size());

	// 各段が実際に使う時間（秒）：最後の段は最後まで、それ以外は切替点まで
	std::vector<float> secs(num, 0.0f);
	std::vector<float> fullSecs(num, 0.0f);
	float totalSec = 0.0f;
	float totalStamina = 0.0f;
	float totalStep = 0.0f;
	for (int i = 0; i < num; ++i)
	{
		const ComboStep& step = m_steps[i];
		const float length = GetAnimeLength(step.animeName);
		const float speed = std::max(step.speed, 0.01f);
		fullSecs[i] = length / (kAnimeFramePerSec * speed);

		const bool isLast = (i == num - 1) || (step.cancelPoint > 1.0f);
		const float usedRate = isLast ? 1.0f : std::clamp(step.cancelPoint, 0.05f, 1.0f);
		secs[i] = fullSecs[i] * usedRate;
		totalSec += secs[i];
		if (i > 0) { totalStamina += step.staminaCost; }   // 1段目の消費は別（ゲームと同じ）
		totalStep += step.stepForward;
	}
	if (totalSec <= 0.0f) { return; }

	ImGui::Text(U8("最速でつないだときの長さ %.2f 秒　消費スタミナ合計 %.1f　踏み込み合計 %.2f m"),
		totalSec, totalStamina, totalStep);

	// 色
	const ImU32 colBg      = ImGui::GetColorU32(ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
	const ImU32 colBorder  = ImGui::GetColorU32(ImVec4(0.50f, 0.50f, 0.50f, 1.0f));
	const ImU32 colHit     = ImGui::GetColorU32(ImVec4(0.90f, 0.30f, 0.30f, 0.70f));
	const ImU32 colTrail   = ImGui::GetColorU32(ImVec4(0.30f, 0.60f, 1.00f, 0.70f));
	const ImU32 colAccept  = ImGui::GetColorU32(ImVec4(0.30f, 0.90f, 0.40f, 0.90f));
	const ImU32 colEvasion = ImGui::GetColorU32(ImVec4(1.00f, 0.80f, 0.20f, 0.90f));
	const ImU32 colCancel  = ImGui::GetColorU32(ImVec4(1.00f, 1.00f, 1.00f, 1.00f));
	const ImU32 colHead    = ImGui::GetColorU32(ImVec4(1.00f, 0.40f, 1.00f, 1.00f));
	const ImU32 colText    = ImGui::GetColorU32(ImGuiCol_Text);

	ImDrawList* draw = ImGui::GetWindowDrawList();
	const ImVec2 origin = ImGui::GetCursorScreenPos();
	const float width = std::max(ImGui::GetContentRegionAvail().x, 100.0f);
	const float barH = 34.0f;

	float x = origin.x;
	for (int i = 0; i < num; ++i)
	{
		const ComboStep& step = m_steps[i];
		const float segW = width * (secs[i] / totalSec);   // この段が使う幅
		const float fullW = (secs[i] > 0.0f) ? segW * (fullSecs[i] / secs[i]) : segW;   // 進行度1.0 までの幅
		const float x0 = x;
		const float y0 = origin.y;
		const float y1 = origin.y + barH;

		// 進行度 → 横位置（この段の中）。切替点より先ははみ出すので段の幅で切る
		auto px = [&](float _rate) { return x0 + std::min(fullW * std::clamp(_rate, 0.0f, 1.0f), segW); };

		draw->AddRectFilled(ImVec2(x0, y0), ImVec2(x0 + segW, y1), colBg);

		// 軌跡（上半分）と当たり判定（下半分）
		draw->AddRectFilled(ImVec2(px(step.trailStart), y0 + 4), ImVec2(px(step.trailEnd), y0 + barH * 0.5f), colTrail);
		draw->AddRectFilled(ImVec2(px(step.hitStart), y0 + barH * 0.5f), ImVec2(px(step.hitEnd), y1 - 4), colHit);

		// 受付（下の細い帯）と回避キャンセル（上の細い帯）
		if (step.acceptStart <= 1.0f)
		{
			draw->AddRectFilled(ImVec2(px(step.acceptStart), y1 - 3), ImVec2(x0 + segW, y1), colAccept);
		}
		draw->AddRectFilled(ImVec2(px(step.evasionCancel), y0), ImVec2(x0 + segW, y0 + 3), colEvasion);

		// 切替点
		if (step.cancelPoint <= 1.0f)
		{
			draw->AddLine(ImVec2(px(step.cancelPoint), y0), ImVec2(px(step.cancelPoint), y1), colCancel, 2.0f);
		}

		// 段の区切りと名前
		draw->AddRect(ImVec2(x0, y0), ImVec2(x0 + segW, y1), colBorder);
		const std::string label = std::to_string(step.comboIndex) + " " + step.animeName;
		draw->PushClipRect(ImVec2(x0, y1), ImVec2(x0 + segW, y1 + 18.0f), true);
		draw->AddText(ImVec2(x0 + 3, y1 + 2), colText, label.c_str());
		draw->PopClipRect();

		// 再生位置
		if (i == m_preview.stepIdx)
		{
			const float length = GetAnimeLength(step.animeName);
			const float progress = (length > 0.0f) ? m_preview.time / length : 0.0f;
			const float hx = px(progress);
			draw->AddLine(ImVec2(hx, y0 - 3), ImVec2(hx, y1 + 3), colHead, 3.0f);
		}

		x += segW;
	}

	// タイムラインの分だけ場所を取る（次の部品が重ならないように）
	ImGui::Dummy(ImVec2(width, barH + 20.0f));

	// 凡例
	ImGui::TextColored(ImVec4(0.30f, 0.60f, 1.00f, 1.0f), U8("■軌跡"));       ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.90f, 0.30f, 0.30f, 1.0f), U8("■当たり判定")); ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.30f, 0.90f, 0.40f, 1.0f), U8("▁受付"));       ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.00f, 0.80f, 0.20f, 1.0f), U8("▔回避で切れる")); ImGui::SameLine();
	ImGui::TextUnformatted(U8("│切替点"));                                     ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.00f, 0.40f, 1.00f, 1.0f), U8("│再生位置"));
}

void ComboStepEditor::UpdatePreviewObject()
{
	if (!m_showPreviewModel || !m_spPreviewModel) { return; }

	// どの段のどの時間を映すか
	//  再生中・一時停止中 → プレビューの位置
	//  止まっている       → 選んでいる段の最初
	int   stepIdx = m_preview.stepIdx;
	float time = m_preview.time;
	if (stepIdx < 0 || stepIdx >= static_cast<int>(m_steps.size()))
	{
		stepIdx = m_selected;
		time = 0.0f;
	}
	if (stepIdx < 0 || stepIdx >= static_cast<int>(m_steps.size())) { return; }

	const ComboStep& step = m_steps[stepIdx];
	const float length = GetAnimeLength(step.animeName);
	if (length <= 0.0f) { return; }

	const float progress = time / length;

	m_spPreviewObj->SetPose(step.animeName, time);
	m_spPreviewObj->SetHitActive(progress >= step.hitStart && progress <= step.hitEnd);
	m_spPreviewObj->SetTrailActive(progress >= step.trailStart && progress < step.trailEnd);
	m_spPreviewObj->KeepAlive();   // 呼ばれている間だけ描かれる
}

void ComboStepEditor::DrawPreviewWindow()
{
	// 最初だけ画面の右側に出す（あとは動かした位置が imgui.ini に残る）
	const ImVec2 display = ImGui::GetIO().DisplaySize;
	// 「初期配置に戻す」のときは毎回、それ以外は最初の1回だけ右側に置く
	const ImGuiCond cond = m_resetLayout ? ImGuiCond_Always : ImGuiCond_FirstUseEver;
	ImGui::SetNextWindowPos(ImVec2(display.x - 510.0f, 40.0f), cond);
	ImGui::SetNextWindowSize(ImVec2(500.0f, std::max(display.y - 80.0f, 300.0f)), cond);
	ImGui::SetNextWindowBgAlpha(1.0f);

	if (!ImGui::Begin(U8("コンボのプレビュー###ComboPreviewModel"), &m_showPreviewModel, GetLockFlags()))
	{
		ImGui::End();
		return;
	}

	ComboPreviewObject& obj = *m_spPreviewObj;

	// このウィンドウが選ばれているときも Space で再生・一時停止
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !ImGui::GetIO().WantTextInput &&
		!ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Space, false))
	{
		TogglePreviewPlay();
	}

	if (!m_spPreviewModel)
	{
		ImGui::TextDisabled(U8("エディターの「モデル」にパスを入れると、ここに映ります"));
		ImGui::End();
		return;
	}

	// 下の操作欄の分を残して、残りを全部モデルの表示にする
	const float controlsHeight = ImGui::GetFrameHeightWithSpacing() * 5.0f;
	ImVec2 avail = ImGui::GetContentRegionAvail();
	avail.y = std::max(avail.y - controlsHeight, 64.0f);
	avail.x = std::max(avail.x, 64.0f);

	obj.SetViewSize(static_cast<int>(avail.x), static_cast<int>(avail.y));

	// モデルを描いたテクスチャを貼る（初回はまだ描かれていないので枠だけ）
	const ImTextureID tex = obj.GetTextureID();
	if (tex)
	{
		ImGui::Image(tex, avail);
	}
	else
	{
		ImGui::Dummy(avail);
	}

	// マウス操作：左ドラッグで回す、右ドラッグで上下、ホイールで近づく
	if (ImGui::IsItemHovered())
	{
		const ImGuiIO& io = ImGui::GetIO();
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			obj.m_camYaw += io.MouseDelta.x * 0.4f;
			obj.m_camPitch = std::clamp(obj.m_camPitch + io.MouseDelta.y * 0.4f, -80.0f, 80.0f);
		}
		if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
		{
			obj.m_camHeight = std::clamp(obj.m_camHeight + io.MouseDelta.y * 0.01f, -1.0f, 3.0f);
		}
		if (io.MouseWheel != 0.0f)
		{
			obj.m_camDistance = std::clamp(obj.m_camDistance - io.MouseWheel * 0.25f, 0.5f, 15.0f);
		}
	}

	// 操作欄
	ImGui::TextDisabled(U8("左ドラッグ：回す　右ドラッグ：上下　ホイール：近づく"));
	if (ImGui::Button(U8("カメラを戻す"))) { obj.ResetCamera(); }
	ImGui::SameLine();
	ImGui::ColorEdit3(U8("背景"), &obj.m_bgColor.x, ImGuiColorEditFlags_NoInputs);
	ImGui::SetNextItemWidth(150.0f);
	ImGui::SliderFloat(U8("明るさ"), &obj.m_brightness, 0.5f, 4.0f, "%.1f");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(150.0f);
	ImGui::SliderFloat(U8("自己発光"), &obj.m_emissive, 0.0f, 1.0f, "%.2f");

	// 武器の持ち方（ゲームと同じ値が初期値。プレビューの見た目だけ変わる）
	ImGui::SetNextItemWidth(200.0f);
	ImGui::DragFloat3(U8("武器の回転"), &obj.m_weaponRot.x, 1.0f, -180.0f, 180.0f, U8("%.0f 度"));
	ImGui::SameLine();
	ImGui::Checkbox(U8("軌跡"), &obj.m_showTrail);

	ImGui::End();
}

void ComboStepEditor::LoadPreviewWeapon()
{
	if (!m_spPreviewObj->SetWeaponModel(m_WeaponModelPath))
	{
		KdDebugGUI::Instance().AddLog(U8("[Error] プレビュー用の武器モデルが読み込めません : %s\n"), m_WeaponModelPath.c_str());
	}
}

void ComboStepEditor::TogglePreviewPlay()
{
	// 段がない・モデルがないときは再生できない
	if (m_steps.empty() || m_animeList.empty()) { return; }

	if (m_preview.playing)
	{
		m_preview.playing = false;
		return;
	}

	// 止まっている（終わっている）なら1段目から
	if (m_preview.stepIdx < 0 || m_preview.stepIdx >= static_cast<int>(m_steps.size()) ||
		m_preview.time >= GetAnimeLength(m_steps[m_preview.stepIdx].animeName))
	{
		StartPreviewStep(0);
		m_preview.lastEvent.clear();
	}
	m_preview.playing = true;
}
