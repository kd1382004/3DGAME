//---- 自分のヘッダー ----
#include "EnemyAttackPatternEditor.h"									//敵の攻撃パターン編集エディター

//---- 武器 ----
#include "../../GameObject/Weapon/ComboStep/ComboStepSaveLoad.h"		//コンボの保存・読み込み

namespace
{
	// 移動の種類(JSONの値 / 画面に出す名前)
	const char* kMoveTypeKey[]   = { "None", "Step", "LeapToTarget" };
	const char* kMoveTypeLabel[] = { U8("なし"), U8("踏み込み(stepForward)"), U8("着地点へ飛ぶ") };
	constexpr int kMoveTypeNum = 3;

	// 当たり判定の形
	const char* kHitShapeKey[]   = { "Bone", "Circle", "Box" };
	const char* kHitShapeLabel[] = { U8("骨に付ける"), U8("円(敵の前・着地点)"), U8("箱(敵の前)") };
	constexpr int kHitShapeNum = 3;

	// イベントの種類
	const char* kEventTypeKey[]   = { "Effect", "Sound", "CameraShake" };
	const char* kEventTypeLabel[] = { U8("エフェクト"), U8("音"), U8("画面の揺れ") };
	constexpr int kEventTypeNum = 3;

	// よく使う骨の名前(ゴブリン)
	const char* kBoneNames[] = { "thumb_2_L", "hand_L", "hand_R", "head", "foot_L", "foot_R" };
	constexpr int kBoneNameNum = 6;

	const ImVec4 kErrorColor = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
	const ImVec4 kDirtyColor = ImVec4(1.0f, 0.6f, 0.2f, 1.0f);

	// keys の中から value の番号を探す(なければ0)
	int FindIndex(const char* const* _keys, int _num, const std::string& _value)
	{
		for (int i = 0; i < _num; i++)
		{
			if (_value == _keys[i]) { return i; }
		}
		return 0;
	}

	// 文字列の入力欄。変わったらtrue
	bool InputString(const char* _label, nlohmann::json& _obj, const char* _key, float _width = 0.0f)
	{
		char buf[256] = {};
		const std::string now = _obj.value(_key, std::string());
		strncpy_s(buf, now.c_str(), _TRUNCATE);
		if (_width > 0.0f) { ImGui::SetNextItemWidth(_width); }
		if (ImGui::InputText(_label, buf, sizeof(buf)))
		{
			_obj[_key] = std::string(buf);
			return true;
		}
		return false;
	}

	// 小数の入力欄。変わったらtrue
	bool DragNumber(const char* _label, nlohmann::json& _obj, const char* _key, float _default,
		float _speed, float _min, float _max, const char* _format = "%.2f")
	{
		float value = _obj.value(_key, _default);
		ImGui::SetNextItemWidth(120.0f);
		if (ImGui::DragFloat(_label, &value, _speed, _min, _max, _format))
		{
			_obj[_key] = std::clamp(value, _min, _max);
			return true;
		}
		return false;
	}

	// 整数の入力欄。変わったらtrue
	bool DragInteger(const char* _label, nlohmann::json& _obj, const char* _key, int _default,
		int _min, int _max)
	{
		int value = _obj.value(_key, _default);
		ImGui::SetNextItemWidth(120.0f);
		if (ImGui::DragInt(_label, &value, 0.1f, _min, _max))
		{
			_obj[_key] = std::clamp(value, _min, _max);
			return true;
		}
		return false;
	}

	// 選択肢から選ぶ欄。変わったらtrue
	bool SelectKey(const char* _label, nlohmann::json& _obj, const char* _key,
		const char* const* _keys, const char* const* _labels, int _num)
	{
		int index = FindIndex(_keys, _num, _obj.value(_key, std::string(_keys[0])));
		bool changed = false;
		ImGui::SetNextItemWidth(180.0f);
		if (ImGui::BeginCombo(_label, _labels[index]))
		{
			for (int i = 0; i < _num; i++)
			{
				if (ImGui::Selectable(_labels[i], i == index))
				{
					_obj[_key] = std::string(_keys[i]);
					changed = true;
				}
			}
			ImGui::EndCombo();
		}
		return changed;
	}
}

//=============================================
// コンストラクタ・デストラクタ
//=============================================

EnemyAttackPatternEditor::EnemyAttackPatternEditor()
{
	RefreshEnemyList();
	RefreshComboFileList();

	if (!m_enemyNames.empty())
	{
		m_nowEnemyName = m_enemyNames.front();
		Load();
	}
}

//=============================================
// 基本の処理
//=============================================

void EnemyAttackPatternEditor::Draw(bool* _open)
{
	if (!ImGui::Begin("Enemy Attack Pattern Editor", _open))
	{
		ImGui::End();
		return;
	}

	DrawFileBar();
	ImGui::Separator();

	if (m_nowEnemyName.empty())
	{
		ImGui::TextUnformatted(U8("敵がありません。上の欄に名前を入れて「新規作成」してください"));
		ImGui::End();
		return;
	}

	// ---- 決まりに合わないもの(赤字で出す。あると保存できない) ----
	const std::vector<std::string> errors = Validate();
	for (const std::string& error : errors)
	{
		ImGui::TextColored(kErrorColor, "%s", error.c_str());
	}

	DrawDistanceTest();
	ImGui::Separator();

	// ---- 左:パターン一覧 / 右:選んでいるパターン ----
	ImGui::BeginChild("PatternList", ImVec2(230.0f, 0.0f), true);
	DrawPatternList();
	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("PatternDetail", ImVec2(0.0f, 0.0f), true);
	nlohmann::json& patterns = GetPatterns();
	if (m_selectPatternIndex >= 0 && m_selectPatternIndex < (int)patterns.size())
	{
		DrawPatternDetail(patterns[m_selectPatternIndex]);
	}
	else
	{
		ImGui::TextDisabled(U8("左の一覧からパターンを選んでください"));
	}
	ImGui::EndChild();

	ImGui::End();
}

//=============================================
// その他の処理
//=============================================

bool EnemyAttackPatternEditor::Load()
{
	if (m_nowEnemyName.empty()) { return false; }

	const std::string path = m_dirPath + m_nowEnemyName + ".json";
	std::ifstream file(path);
	if (!file)
	{
		m_message = U8("読み込み失敗: ファイルが開けません");
		return false;
	}

	nlohmann::json data = nlohmann::json::parse(file, nullptr, false);
	if (data.is_discarded() || !data.is_object())
	{
		m_message = U8("読み込み失敗: JSONが壊れています");
		return false;
	}

	m_data = std::move(data);
	GetPatterns();
	m_selectPatternIndex = GetPatterns().empty() ? -1 : 0;
	m_stepCache.clear();
	m_dirty = false;
	m_message = U8("読み込みました");
	return true;
}

bool EnemyAttackPatternEditor::Save()
{
	if (m_nowEnemyName.empty()) { return false; }

	// 決まりに合わないものが1件でもあれば保存しない
	if (!Validate().empty())
	{
		m_message = U8("保存失敗: 赤字のエラーを直してください");
		return false;
	}

	std::error_code ec;
	std::filesystem::create_directories(m_dirPath, ec);

	const std::string path = m_dirPath + m_nowEnemyName + ".json";
	std::ofstream file(path);
	if (!file)
	{
		m_message = U8("保存失敗: ファイルが開けません");
		return false;
	}

	m_data["version"] = 1;
	file << m_data.dump(4);
	m_dirty = false;
	m_savedFlg = true;
	m_message = U8("保存しました");
	return true;
}

//=============================================
// 基本の処理
//=============================================

void EnemyAttackPatternEditor::DrawFileBar()
{
	// ---- 敵の切り替え ----
	ImGui::SetNextItemWidth(160.0f);
	if (ImGui::BeginCombo(U8("敵"), m_nowEnemyName.empty() ? "-" : m_nowEnemyName.c_str()))
	{
		for (const std::string& name : m_enemyNames)
		{
			if (ImGui::Selectable(name.c_str(), name == m_nowEnemyName) && name != m_nowEnemyName)
			{
				m_nowEnemyName = name;
				Load();
			}
		}
		ImGui::EndCombo();
	}

	ImGui::SameLine();
	if (ImGui::Button("Load")) { Load(); }
	ImGui::SameLine();
	if (ImGui::Button("Save")) { Save(); }
	ImGui::SameLine();
	if (ImGui::Button(U8("一覧を読み直す")))
	{
		RefreshEnemyList();
		RefreshComboFileList();
		m_stepCache.clear();
	}

	if (m_dirty)
	{
		ImGui::SameLine();
		ImGui::TextColored(kDirtyColor, U8("* 未保存"));
	}
	if (!m_message.empty())
	{
		ImGui::SameLine();
		ImGui::TextUnformatted(m_message.c_str());
	}

	// ---- 新しい敵のファイルを作る ----
	ImGui::SetNextItemWidth(160.0f);
	ImGui::InputText(U8("新しい敵の名前"), m_newEnemyName, sizeof(m_newEnemyName));
	ImGui::SameLine();
	if (ImGui::Button(U8("新規作成")))
	{
		const std::string name = m_newEnemyName;
		if (name.empty())
		{
			m_message = U8("名前を入れてください");
		}
		else if (std::find(m_enemyNames.begin(), m_enemyNames.end(), name) != m_enemyNames.end())
		{
			m_message = U8("その名前はもうあります");
		}
		else
		{
			m_nowEnemyName = name;
			m_data = nlohmann::json::object();
			m_data["version"] = 1;
			m_data["m_patterns"] = nlohmann::json::array();
			m_selectPatternIndex = -1;
			m_stepCache.clear();
			m_dirty = true;
			if (Save())
			{
				RefreshEnemyList();
				m_newEnemyName[0] = '\0';
				m_message = U8("作りました");
			}
		}
	}
}

void EnemyAttackPatternEditor::DrawPatternList()
{
	nlohmann::json& patterns = GetPatterns();

	// 重みの合計(出やすさの割合を出す用)
	float weightSum = 0.0f;
	for (const nlohmann::json& pattern : patterns)
	{
		weightSum += std::max(0.0f, pattern.value("m_weight", 0.0f));
	}

	// ---- 一覧 ----
	for (int i = 0; i < (int)patterns.size(); i++)
	{
		const std::string name = patterns[i].value("m_name", std::string());
		const float weight = std::max(0.0f, patterns[i].value("m_weight", 0.0f));
		const int percent = (weightSum > 0.0f) ? (int)(weight / weightSum * 100.0f + 0.5f) : 0;
		const int minLV = patterns[i].value("m_minLV", 1);

		std::string label = name + "  (" + std::to_string(percent) + "%)";
		if (minLV > 1) { label += "  LV" + std::to_string(minLV) + U8("〜"); }
		label += "##" + std::to_string(i);
		if (ImGui::Selectable(label.c_str(), i == m_selectPatternIndex))
		{
			m_selectPatternIndex = i;
		}
	}

	ImGui::Separator();
	ImGui::TextDisabled(U8("( )内は距離を考えない出やすさ"));

	// ---- 追加・複製・削除・並べ替え ----
	if (ImGui::Button(U8("追加")))
	{
		patterns.push_back(MakeDefaultPattern());
		m_selectPatternIndex = (int)patterns.size() - 1;
		m_dirty = true;
	}

	const bool selected = (m_selectPatternIndex >= 0 && m_selectPatternIndex < (int)patterns.size());
	if (!selected) { return; }

	ImGui::SameLine();
	if (ImGui::Button(U8("複製")))
	{
		nlohmann::json copy = patterns[m_selectPatternIndex];
		copy["m_name"] = copy.value("m_name", std::string()) + "_Copy";
		patterns.insert(patterns.begin() + m_selectPatternIndex + 1, copy);
		m_selectPatternIndex++;
		m_dirty = true;
		return;
	}

	ImGui::SameLine();
	if (ImGui::Button(U8("削除")))
	{
		patterns.erase(patterns.begin() + m_selectPatternIndex);
		m_selectPatternIndex = std::min(m_selectPatternIndex, (int)patterns.size() - 1);
		m_dirty = true;
		return;
	}

	if (ImGui::Button(U8("上へ")) && m_selectPatternIndex > 0)
	{
		std::swap(patterns[m_selectPatternIndex], patterns[m_selectPatternIndex - 1]);
		m_selectPatternIndex--;
		m_dirty = true;
	}
	ImGui::SameLine();
	if (ImGui::Button(U8("下へ")) && m_selectPatternIndex < (int)patterns.size() - 1)
	{
		std::swap(patterns[m_selectPatternIndex], patterns[m_selectPatternIndex + 1]);
		m_selectPatternIndex++;
		m_dirty = true;
	}
}

void EnemyAttackPatternEditor::DrawPatternDetail(nlohmann::json& _pattern)
{
	ImGui::PushID(m_selectPatternIndex);

	// ---- 名前 ----
	if (InputString(U8("パターン名"), _pattern, "m_name", 200.0f)) { m_dirty = true; }

	// ---- コンボファイル(単発アニメの並び) ----
	const std::string comboPath = _pattern.value("m_comboFilePath", std::string());
	ImGui::SetNextItemWidth(360.0f);
	if (ImGui::BeginCombo(U8("コンボファイル"), comboPath.empty() ? "-" : comboPath.c_str()))
	{
		for (const std::string& path : m_comboFilePaths)
		{
			if (ImGui::Selectable(path.c_str(), path == comboPath))
			{
				_pattern["m_comboFilePath"] = path;
				m_dirty = true;
			}
		}
		ImGui::EndCombo();
	}
	ImGui::TextDisabled(U8("コンボファイルは「武器のコンボ」エディターで、持ち主を敵の名前にして作ります"));

	// ---- 使う条件 ----
	ImGui::Separator();
	ImGui::TextUnformatted(U8("使う条件"));
	if (DragInteger(U8("出始めるLV(この敵のLVがこれ以上で出る)"), _pattern, "m_minLV", 1, 1, 99)) { m_dirty = true; }
	if (DragNumber(U8("最小距離(m)"), _pattern, "m_minDistance", 0.0f, 0.05f, 0.0f, 100.0f)) { m_dirty = true; }
	ImGui::SameLine();
	if (DragNumber(U8("最大距離(m)"), _pattern, "m_maxDistance", 5.0f, 0.05f, 0.0f, 100.0f)) { m_dirty = true; }
	if (DragNumber(U8("重み(出やすさ)"), _pattern, "m_weight", 1.0f, 0.05f, 0.0f, 100.0f)) { m_dirty = true; }
	if (DragNumber(U8("クールタイム(秒)"), _pattern, "m_cooldown", 0.0f, 0.05f, 0.0f, 60.0f)) { m_dirty = true; }
	ImGui::SameLine();
	if (DragNumber(U8("終わった後の待ち(秒)"), _pattern, "m_waitAfter", 0.0f, 0.05f, 0.0f, 60.0f)) { m_dirty = true; }
	ImGui::TextDisabled(U8("終わった後の待ちが0なら、敵の今までの攻撃待ち時間を使います"));

	// ---- 段ごとの設定 ----
	ImGui::Separator();
	const std::vector<StepInfo>& steps = GetSteps(comboPath);
	const int stepNum = (int)steps.size();
	ImGui::Text(U8("段ごとの設定(%d段)"), stepNum);

	if (comboPath.empty())
	{
		ImGui::TextDisabled(U8("コンボファイルを選ぶと段が出ます"));
	}
	else if (stepNum == 0)
	{
		ImGui::TextColored(kErrorColor, U8("コンボファイルが読めないか、段がありません"));
	}

	// 段の数と、骨・特別な設定の数がずれていたら合わせる(コンボファイルの段を増やした・減らしたとき)
	const int hitNodeNum = _pattern.contains("m_hitNodes") && _pattern["m_hitNodes"].is_array() ? (int)_pattern["m_hitNodes"].size() : 0;
	const int extraNum = _pattern.contains("m_stepExtras") && _pattern["m_stepExtras"].is_array() ? (int)_pattern["m_stepExtras"].size() : 0;
	if (stepNum > 0 && (hitNodeNum != stepNum || extraNum != stepNum))
	{
		SyncStepArrays(_pattern, stepNum);
		m_dirty = true;
		m_message = U8("段の数に合わせて設定を増やしました(減らしました)");
	}

	for (int i = 0; i < stepNum; i++)
	{
		DrawStep(_pattern, i, steps);
	}

	ImGui::PopID();
}

void EnemyAttackPatternEditor::DrawStep(nlohmann::json& _pattern, int _stepIndex, const std::vector<StepInfo>& _steps)
{
	const StepInfo& info = _steps[_stepIndex];

	ImGui::PushID(_stepIndex);

	const std::string title = std::to_string(_stepIndex + 1) + U8("段目 : ") + info.m_animeName;
	if (!ImGui::TreeNodeEx(title.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::PopID();
		return;
	}

	// コンボファイル側の値(ここでは変えられない。目安として出す)
	ImGui::TextDisabled(U8("当たる範囲 %.2f〜%.2f / 次の段へ %.2f(コンボファイルの値)"),
		info.m_hitStart, info.m_hitEnd, info.m_cancelPoint);

	// ---- 当たり判定の骨 ----
	nlohmann::json& hitNodes = _pattern["m_hitNodes"];
	char buf[128] = {};
	strncpy_s(buf, hitNodes[_stepIndex].get<std::string>().c_str(), _TRUNCATE);
	ImGui::SetNextItemWidth(160.0f);
	if (ImGui::InputText(U8("当たり判定の骨"), buf, sizeof(buf)))
	{
		hitNodes[_stepIndex] = std::string(buf);
		m_dirty = true;
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(120.0f);
	if (ImGui::BeginCombo("##BoneList", U8("よく使う骨"), ImGuiComboFlags_NoArrowButton))
	{
		for (int b = 0; b < kBoneNameNum; b++)
		{
			if (ImGui::Selectable(kBoneNames[b]))
			{
				hitNodes[_stepIndex] = std::string(kBoneNames[b]);
				m_dirty = true;
			}
		}
		ImGui::EndCombo();
	}

	// ---- 特別な設定 ----
	nlohmann::json& extra = _pattern["m_stepExtras"][_stepIndex];

	if (SelectKey(U8("移動"), extra, "m_moveType", kMoveTypeKey, kMoveTypeLabel, kMoveTypeNum)) { m_dirty = true; }
	if (extra.value("m_moveType", std::string("None")) == "LeapToTarget")
	{
		ImGui::Indent();
		if (DragNumber(U8("狙いを決める進行度"), extra, "m_aimLockEnd", 0.3f, 0.005f, 0.0f, 1.0f)) { m_dirty = true; }
		if (DragNumber(U8("飛ぶ開始"), extra, "m_moveStart", 0.0f, 0.005f, 0.0f, 1.0f)) { m_dirty = true; }
		ImGui::SameLine();
		if (DragNumber(U8("飛ぶ終了"), extra, "m_moveEnd", 0.3f, 0.005f, 0.0f, 1.0f)) { m_dirty = true; }
		ImGui::Unindent();
	}

	bool cancelRootMotion = extra.value("m_cancelRootMotion", false);
	if (ImGui::Checkbox(U8("アニメ自体の前後の移動を打ち消す"), &cancelRootMotion))
	{
		extra["m_cancelRootMotion"] = cancelRootMotion;
		m_dirty = true;
	}

	if (SelectKey(U8("当たり判定の形"), extra, "m_hitShape", kHitShapeKey, kHitShapeLabel, kHitShapeNum)) { m_dirty = true; }
	const std::string hitShape = extra.value("m_hitShape", std::string("Bone"));
	ImGui::Indent();
	if (hitShape == "Bone")
	{
		if (DragNumber(U8("判定の球の半径(m)"), extra, "m_hitRadius", 1.0f, 0.05f, 0.0f, 50.0f)) { m_dirty = true; }
	}
	else if (hitShape == "Circle")
	{
		if (DragNumber(U8("円の半径(m)"), extra, "m_hitRadius", 1.0f, 0.05f, 0.0f, 50.0f)) { m_dirty = true; }
		ImGui::SameLine();
		if (DragNumber(U8("前に出す距離(m)"), extra, "m_hitForward", 0.0f, 0.05f, -50.0f, 50.0f)) { m_dirty = true; }
		ImGui::TextDisabled(U8("「着地点へ飛ぶ」のときは着地点に出ます(前に出す距離は使いません)"));
	}
	else if (hitShape == "Box")
	{
		// 箱の大きさの半分 [横, 高さ, 奥行き]
		float size[3] = { 1.0f, 1.0f, 1.0f };
		if (extra.contains("m_hitBoxSize") && extra["m_hitBoxSize"].is_array() && extra["m_hitBoxSize"].size() == 3)
		{
			for (int i = 0; i < 3; i++) { size[i] = extra["m_hitBoxSize"][i].get<float>(); }
		}
		ImGui::SetNextItemWidth(240.0f);
		if (ImGui::DragFloat3(U8("箱の大きさの半分(横・高さ・奥行き)"), size, 0.05f, 0.0f, 50.0f, "%.2f"))
		{
			extra["m_hitBoxSize"] = { size[0], size[1], size[2] };
			m_dirty = true;
		}
		if (DragNumber(U8("前に出す距離(m)"), extra, "m_hitForward", 0.0f, 0.05f, -50.0f, 50.0f)) { m_dirty = true; }
	}
	if (hitShape != "Bone")
	{
		if (DragNumber(U8("予告を出す進行度"), extra, "m_warnStart", 0.0f, 0.005f, 0.0f, 1.0f)) { m_dirty = true; }
	}
	ImGui::Unindent();

	DrawEvents(extra);

	if (InputString(U8("特殊処理の名前(普通は空)"), extra, "m_specialType", 160.0f)) { m_dirty = true; }

	ImGui::TreePop();
	ImGui::PopID();
}

void EnemyAttackPatternEditor::DrawEvents(nlohmann::json& _extra)
{
	if (!_extra.contains("m_events") || !_extra["m_events"].is_array())
	{
		_extra["m_events"] = nlohmann::json::array();
	}
	nlohmann::json& events = _extra["m_events"];

	ImGui::Text(U8("イベント(%d件)"), (int)events.size());
	ImGui::SameLine();
	if (ImGui::SmallButton(U8("イベント追加")))
	{
		events.push_back({ { "p", 0.5f }, { "type", "Effect" }, { "name", "" } });
		m_dirty = true;
	}

	int eraseIndex = -1;
	for (int i = 0; i < (int)events.size(); i++)
	{
		nlohmann::json& ev = events[i];
		ImGui::PushID(i);
		ImGui::Indent();

		if (DragNumber(U8("進行度"), ev, "p", 0.5f, 0.005f, 0.0f, 1.0f)) { m_dirty = true; }
		ImGui::SameLine();
		if (SelectKey(U8("種類"), ev, "type", kEventTypeKey, kEventTypeLabel, kEventTypeNum)) { m_dirty = true; }
		ImGui::SameLine();
		if (ImGui::SmallButton(U8("消す"))) { eraseIndex = i; }

		if (ev.value("type", std::string("Effect")) == "CameraShake")
		{
			if (DragNumber(U8("強さ"), ev, "power", 0.5f, 0.01f, 0.0f, 5.0f)) { m_dirty = true; }
			ImGui::SameLine();
			if (DragNumber(U8("時間(秒)"), ev, "time", 0.3f, 0.01f, 0.0f, 5.0f)) { m_dirty = true; }
		}
		else
		{
			if (InputString(U8("名前"), ev, "name", 360.0f)) { m_dirty = true; }
		}

		ImGui::Unindent();
		ImGui::PopID();
	}

	if (eraseIndex >= 0)
	{
		events.erase(events.begin() + eraseIndex);
		m_dirty = true;
	}
}

void EnemyAttackPatternEditor::DrawDistanceTest()
{
	ImGui::SetNextItemWidth(200.0f);
	ImGui::SliderFloat(U8("プレイヤーとの距離(確認用)"), &m_testDistance, 0.0f, 30.0f, "%.1f m");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(120.0f);
	ImGui::SliderInt(U8("敵のLV(確認用)"), &m_testLV, 1, 99);

	// この距離とLVで選べるパターン(クールタイムは考えない)
	const nlohmann::json& patterns = GetPatterns();
	float weightSum = 0.0f;
	for (const nlohmann::json& pattern : patterns)
	{
		const bool inRange = m_testDistance >= pattern.value("m_minDistance", 0.0f) && m_testDistance <= pattern.value("m_maxDistance", 0.0f)
			&& m_testLV >= pattern.value("m_minLV", 1);
		if (inRange) { weightSum += std::max(0.0f, pattern.value("m_weight", 0.0f)); }
	}

	if (weightSum <= 0.0f)
	{
		ImGui::TextDisabled(U8("この距離とLVで出るパターンはありません(追いかけ続けます)"));
		return;
	}

	std::string text = U8("この距離とLVで出るもの : ");
	for (const nlohmann::json& pattern : patterns)
	{
		const bool inRange = m_testDistance >= pattern.value("m_minDistance", 0.0f) && m_testDistance <= pattern.value("m_maxDistance", 0.0f)
			&& m_testLV >= pattern.value("m_minLV", 1);
		const float weight = std::max(0.0f, pattern.value("m_weight", 0.0f));
		if (!inRange || weight <= 0.0f) { continue; }

		const int percent = (int)(weight / weightSum * 100.0f + 0.5f);
		text += pattern.value("m_name", std::string()) + " " + std::to_string(percent) + "%  ";
	}
	ImGui::TextUnformatted(text.c_str());
}

//=============================================
// 取得
//=============================================

nlohmann::json& EnemyAttackPatternEditor::GetPatterns()
{
	if (!m_data.is_object()) { m_data = nlohmann::json::object(); }
	if (!m_data.contains("m_patterns") || !m_data["m_patterns"].is_array())
	{
		m_data["m_patterns"] = nlohmann::json::array();
	}
	return m_data["m_patterns"];
}

const std::vector<EnemyAttackPatternEditor::StepInfo>& EnemyAttackPatternEditor::GetSteps(const std::string& _path)
{
	auto it = m_stepCache.find(_path);
	if (it != m_stepCache.end()) { return it->second; }

	// 読めなかったときも空で覚えておく(毎フレームエラーを出さないため)
	std::vector<StepInfo>& steps = m_stepCache[_path];
	if (_path.empty()) { return steps; }

	ComboFileData data;
	if (!ComboStepSaveLoad::Load(_path, data)) { return steps; }

	for (const ComboStep& step : data.m_steps)
	{
		StepInfo info;
		info.m_animeName = step.m_animeName;
		info.m_hitStart = step.m_hitStart;
		info.m_hitEnd = step.m_hitEnd;
		info.m_cancelPoint = step.m_cancelPoint;
		steps.push_back(info);
	}
	return steps;
}

//=============================================
// その他の処理
//=============================================

void EnemyAttackPatternEditor::RefreshEnemyList()
{
	m_enemyNames.clear();

	std::error_code ec;
	std::filesystem::create_directories(m_dirPath, ec);

	for (const auto& entry : std::filesystem::directory_iterator(m_dirPath, ec))
	{
		if (!entry.is_regular_file()) { continue; }
		if (entry.path().extension() != ".json") { continue; }
		m_enemyNames.push_back(entry.path().stem().string());
	}
	std::sort(m_enemyNames.begin(), m_enemyNames.end());
}

void EnemyAttackPatternEditor::RefreshComboFileList()
{
	m_comboFilePaths.clear();

	std::error_code ec;
	if (!std::filesystem::exists(m_comboDirPath, ec)) { return; }

	for (const auto& entry : std::filesystem::recursive_directory_iterator(m_comboDirPath, ec))
	{
		if (!entry.is_regular_file()) { continue; }
		if (entry.path().extension() != ".json") { continue; }
		m_comboFilePaths.push_back(entry.path().generic_string());
	}
	std::sort(m_comboFilePaths.begin(), m_comboFilePaths.end());
}

void EnemyAttackPatternEditor::SyncStepArrays(nlohmann::json& _pattern, int _stepNum)
{
	if (!_pattern.contains("m_hitNodes") || !_pattern["m_hitNodes"].is_array())
	{
		_pattern["m_hitNodes"] = nlohmann::json::array();
	}
	if (!_pattern.contains("m_stepExtras") || !_pattern["m_stepExtras"].is_array())
	{
		_pattern["m_stepExtras"] = nlohmann::json::array();
	}

	nlohmann::json& hitNodes = _pattern["m_hitNodes"];
	nlohmann::json& extras = _pattern["m_stepExtras"];

	// 足りない分は初期値で足し、多い分は消す
	while ((int)hitNodes.size() < _stepNum) { hitNodes.push_back(""); }
	while ((int)hitNodes.size() > _stepNum) { hitNodes.erase(hitNodes.size() - 1); }
	while ((int)extras.size() < _stepNum) { extras.push_back(MakeDefaultExtra()); }
	while ((int)extras.size() > _stepNum) { extras.erase(extras.size() - 1); }

	// 骨の名前が文字列でないもの(壊れたデータ)は空にする
	for (auto& node : hitNodes)
	{
		if (!node.is_string()) { node = ""; }
	}
}

nlohmann::json EnemyAttackPatternEditor::MakeDefaultPattern()
{
	nlohmann::json pattern;
	pattern["m_name"] = "NewPattern";
	pattern["m_comboFilePath"] = "";
	pattern["m_minLV"] = 1;
	pattern["m_minDistance"] = 0.0f;
	pattern["m_maxDistance"] = 5.0f;
	pattern["m_weight"] = 1.0f;
	pattern["m_cooldown"] = 0.0f;
	pattern["m_waitAfter"] = 0.0f;
	pattern["m_hitNodes"] = nlohmann::json::array();
	pattern["m_stepExtras"] = nlohmann::json::array();
	return pattern;
}

nlohmann::json EnemyAttackPatternEditor::MakeDefaultExtra()
{
	nlohmann::json extra;
	extra["m_moveType"] = "None";
	extra["m_aimLockEnd"] = 0.3f;
	extra["m_moveStart"] = 0.0f;
	extra["m_moveEnd"] = 0.3f;
	extra["m_cancelRootMotion"] = false;
	extra["m_hitShape"] = "Bone";
	extra["m_hitRadius"] = 1.0f;
	extra["m_hitBoxSize"] = { 1.0f, 1.0f, 1.0f };
	extra["m_hitForward"] = 0.0f;
	extra["m_warnStart"] = 0.0f;
	extra["m_events"] = nlohmann::json::array();
	extra["m_specialType"] = "";
	return extra;
}

std::vector<std::string> EnemyAttackPatternEditor::Validate()
{
	std::vector<std::string> errors;
	const nlohmann::json& patterns = GetPatterns();

	std::vector<std::string> names;
	for (int i = 0; i < (int)patterns.size(); i++)
	{
		const nlohmann::json& pattern = patterns[i];
		const std::string name = pattern.value("m_name", std::string());
		const std::string head = "[" + (name.empty() ? std::to_string(i + 1) : name) + "] ";

		if (name.empty()) { errors.push_back(head + U8("パターン名が空です")); }
		else if (std::find(names.begin(), names.end(), name) != names.end()) { errors.push_back(head + U8("同じ名前のパターンがあります")); }
		names.push_back(name);

		const std::string comboPath = pattern.value("m_comboFilePath", std::string());
		const int stepNum = (int)GetSteps(comboPath).size();
		if (comboPath.empty()) { errors.push_back(head + U8("コンボファイルを選んでください")); }
		else if (stepNum == 0) { errors.push_back(head + U8("コンボファイルが読めないか、段がありません")); }

		if (pattern.value("m_minDistance", 0.0f) > pattern.value("m_maxDistance", 0.0f))
		{
			errors.push_back(head + U8("最小距離が最大距離より大きいです"));
		}

		// 段ごとの設定
		const int extraNum = pattern.contains("m_stepExtras") && pattern["m_stepExtras"].is_array() ? (int)pattern["m_stepExtras"].size() : 0;
		const int hitNodeNum = pattern.contains("m_hitNodes") && pattern["m_hitNodes"].is_array() ? (int)pattern["m_hitNodes"].size() : 0;
		if (stepNum > 0 && (extraNum != stepNum || hitNodeNum != stepNum))
		{
			errors.push_back(head + U8("段の数と設定の数が違います(このパターンを開くと合わせます)"));
		}

		for (int s = 0; s < extraNum; s++)
		{
			const nlohmann::json& extra = pattern["m_stepExtras"][s];
			const std::string stepHead = head + std::to_string(s + 1) + U8("段目 ");

			if (extra.value("m_moveType", std::string("None")) == "LeapToTarget" &&
				extra.value("m_moveStart", 0.0f) > extra.value("m_moveEnd", 0.0f))
			{
				errors.push_back(stepHead + U8("飛ぶ開始が飛ぶ終了より後です"));
			}

			if (extra.value("m_hitShape", std::string("Bone")) == "Circle" && extra.value("m_hitRadius", 0.0f) <= 0.0f)
			{
				errors.push_back(stepHead + U8("円の半径が0です"));
			}

			if (extra.value("m_hitShape", std::string("Bone")) == "Box" && extra.contains("m_hitBoxSize") && extra["m_hitBoxSize"].is_array())
			{
				for (const nlohmann::json& v : extra["m_hitBoxSize"])
				{
					if (v.is_number() && v.get<float>() <= 0.0f)
					{
						errors.push_back(stepHead + U8("箱の大きさに0があります"));
						break;
					}
				}
			}

			if (extra.value("m_hitShape", std::string("Bone")) == "Bone" && s < hitNodeNum &&
				pattern["m_hitNodes"][s].is_string() && pattern["m_hitNodes"][s].get<std::string>().empty())
			{
				errors.push_back(stepHead + U8("当たり判定の骨が空です"));
			}

			if (extra.contains("m_events") && extra["m_events"].is_array())
			{
				for (const nlohmann::json& ev : extra["m_events"])
				{
					if (ev.value("type", std::string("Effect")) != "CameraShake" && ev.value("name", std::string()).empty())
					{
						errors.push_back(stepHead + U8("名前が空のイベントがあります"));
						break;
					}
				}
			}
		}
	}

	return errors;
}
