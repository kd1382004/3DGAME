#include"TreasureChestTableEditor.h"
#include"../LootTableManager/LootTableManager.h"
#include<set>
#include<cstdio>

TreasureChestTableEditor::TreasureChestTableEditor(const std::string& _filePath)
	: m_filePath(_filePath)
{
	LoadMasters();
	Load();
}

void TreasureChestTableEditor::Draw(bool* _open)
{
	if (!ImGui::Begin("Treasure Chest Table Editor", _open))
	{
		ImGui::End();
		return;
	}

	// ---- 上部ボタン ----
	if (ImGui::Button("Load")) Load();
	ImGui::SameLine();
	if (ImGui::Button("Save")) Save();

	if (m_dirty)
	{
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), U8("* 未保存"));
	}
	if (!m_message.empty())
	{
		ImGui::SameLine();
		ImGui::TextUnformatted(m_message.c_str());
	}

	ImGui::TextDisabled(U8("宝箱は「slot + 宝箱のLv」のテーブルを使います（例: Lv2 → slot2）。保存するとゲーム中の宝箱にもすぐ反映されます。"));
	ImGui::Separator();

	if (!m_data.is_object()) { m_data = nlohmann::json::object(); }
	if (!m_data.contains("tables") || !m_data["tables"].is_object()) { m_data["tables"] = nlohmann::json::object(); }

	// ---- 左：テーブル一覧 ----
	ImGui::BeginChild("TableList", ImVec2(200.0f, 0.0f), true);
	DrawTableList();
	ImGui::EndChild();

	ImGui::SameLine();

	// ---- 右：選択中のテーブル ----
	ImGui::BeginChild("TableBody", ImVec2(0.0f, 0.0f), true);
	if (!m_selectTable.empty() && Tables().contains(m_selectTable))
	{
		const std::string tableName = m_selectTable; // 名前変更で m_selectTable が変わるのでコピーを渡す
		DrawTable(tableName, Tables()[tableName]);
	}
	else
	{
		ImGui::TextUnformatted(U8("左の一覧からテーブルを選んでください"));
	}
	ImGui::EndChild();

	ImGui::End();
}

bool TreasureChestTableEditor::Load()
{
	std::ifstream file(m_filePath);
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
	if (!m_data.contains("tables") || !m_data["tables"].is_object())
	{
		m_data["tables"] = nlohmann::json::object();
	}

	// 選択中のテーブルがなくなっていたら先頭を選ぶ
	if (!Tables().contains(m_selectTable))
	{
		m_selectTable.clear();
		for (auto& [name, table] : Tables().items())
		{
			m_selectTable = name;
			break;
		}
	}
	std::snprintf(m_renameBuf, sizeof(m_renameBuf), "%s", m_selectTable.c_str());

	m_dirty = false;
	m_message = U8("読み込みました");
	return true;
}

bool TreasureChestTableEditor::Save()
{
	// エラーが1件でもあれば保存しない
	if (!ValidateAll().empty())
	{
		m_message = U8("保存失敗: (!) の付いたテーブルにエラーがあります");
		return false;
	}

	// 名前はアイテム一覧（マスタ）に合わせておく（JSONを見たときに分かりやすいように）
	for (auto& [tableName, table] : Tables().items())
	{
		if (!table.contains("items") || !table["items"].is_array()) { continue; }
		for (auto& item : table["items"])
		{
			const std::string name = FindItemName(item.value("type", -1), item.value("id", -1));
			if (!name.empty()) { item["name"] = name; }
		}
	}

	std::ofstream file(m_filePath);
	if (!file)
	{
		m_message = U8("保存失敗: ファイルが開けません");
		return false;
	}

	file << m_data.dump(1, '\t'); // タブインデントで保存
	file.close();

	// ゲーム中の宝箱が使うテーブルを読み直す
	LootTableManager::GetShared()->Load(m_filePath);

	m_dirty = false;
	m_message = U8("保存しました（ゲームに反映済み）");
	return true;
}

void TreasureChestTableEditor::LoadMasters()
{
	m_potions = LoadMaster(m_potionMasterPath);
	m_materials = LoadMaster(m_materialMasterPath);
}

std::vector<TreasureChestTableEditor::MasterItem> TreasureChestTableEditor::LoadMaster(const std::string& _filePath)
{
	std::vector<MasterItem> list;

	std::ifstream file(_filePath);
	if (!file) { return list; }

	nlohmann::json data = nlohmann::json::parse(file, nullptr, false);
	if (data.is_discarded() || !data.contains("inventory") || !data["inventory"].is_array()) { return list; }

	for (const auto& itemData : data["inventory"])
	{
		MasterItem item;
		item.id = itemData.value("id", 0);
		item.name = itemData.value("name", std::string{});
		list.push_back(item);
	}
	return list;
}

const std::vector<TreasureChestTableEditor::MasterItem>& TreasureChestTableEditor::GetMaster(int _type) const
{
	if (_type == ItemType_Material) { return m_materials; }
	return m_potions;
}

std::string TreasureChestTableEditor::FindItemName(int _type, int _id) const
{
	if (_type < 0 || _type >= ItemTypeSiz) { return {}; }

	for (const MasterItem& item : GetMaster(_type))
	{
		if (item.id == _id) { return item.name; }
	}
	return {};
}

void TreasureChestTableEditor::SelectTable(const std::string& _tableName)
{
	m_selectTable = _tableName;
	std::snprintf(m_renameBuf, sizeof(m_renameBuf), "%s", m_selectTable.c_str());
}

void TreasureChestTableEditor::DrawTableList()
{
	ImGui::TextUnformatted(U8("テーブル"));
	ImGui::Separator();

	std::string clicked;
	for (auto& [name, table] : Tables().items())
	{
		std::string label = name;
		if (!Validate(name, table).empty()) { label += " (!)"; }

		if (ImGui::Selectable((label + "##" + name).c_str(), name == m_selectTable))
		{
			clicked = name;
		}
	}
	if (!clicked.empty()) { SelectTable(clicked); }

	ImGui::Separator();

	// 追加
	if (ImGui::Button(U8("追加")))
	{
		const std::string name = MakeNewTableName();
		Tables()[name] = MakeDefaultTable();
		SelectTable(name);
		m_dirty = true;
	}

	const bool hasSelect = !m_selectTable.empty() && Tables().contains(m_selectTable);

	// 複製
	ImGui::SameLine();
	if (ImGui::Button(U8("複製")) && hasSelect)
	{
		const std::string name = MakeNewTableName();
		Tables()[name] = Tables()[m_selectTable];
		SelectTable(name);
		m_dirty = true;
	}

	// 削除（確認あり）
	ImGui::SameLine();
	if (ImGui::Button(U8("削除")) && hasSelect)
	{
		ImGui::OpenPopup("DeleteTable");
	}

	if (ImGui::BeginPopupModal("DeleteTable", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("%s", (m_selectTable + U8(" を削除しますか？")).c_str());
		if (ImGui::Button(U8("削除する")))
		{
			Tables().erase(m_selectTable);
			m_selectTable.clear();
			for (auto& [name, table] : Tables().items())
			{
				m_selectTable = name;
				break;
			}
			SelectTable(m_selectTable);
			m_dirty = true;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button(U8("やめる")))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void TreasureChestTableEditor::DrawTable(const std::string& _tableName, nlohmann::json& _table)
{
	// ---- 名前 ----
	ImGui::SetNextItemWidth(160.0f);
	ImGui::InputText(U8("名前"), m_renameBuf, sizeof(m_renameBuf));
	ImGui::SameLine();
	if (ImGui::Button(U8("名前を変更")))
	{
		const std::string newName = m_renameBuf;
		if (newName.empty())
		{
			m_message = U8("名前が空です");
		}
		else if (newName != _tableName && Tables().contains(newName))
		{
			m_message = U8("同じ名前のテーブルがあります: ") + newName;
		}
		else if (newName != _tableName)
		{
			nlohmann::json copy = _table;
			Tables().erase(_tableName); // ここで _table は使えなくなるので、すぐ抜ける
			Tables()[newName] = std::move(copy);
			SelectTable(newName);
			m_dirty = true;
			return;
		}
	}

	// ---- 出る個数 ----
	int dropMin = _table.value("dropMin", 1);
	int dropMax = _table.value("dropMax", dropMin);

	ImGui::SetNextItemWidth(100.0f);
	if (ImGui::InputInt(U8("最小個数"), &dropMin))
	{
		_table["dropMin"] = (std::max)(0, dropMin);
		m_dirty = true;
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100.0f);
	if (ImGui::InputInt(U8("最大個数"), &dropMax))
	{
		_table["dropMax"] = (std::max)(0, dropMax);
		m_dirty = true;
	}

	dropMin = _table.value("dropMin", 1);
	dropMax = _table.value("dropMax", dropMin);
	const float avgDropNum = (dropMin + dropMax) * 0.5f;

	// ---- アイテム ----
	if (!_table.contains("items") || !_table["items"].is_array())
	{
		_table["items"] = nlohmann::json::array();
	}
	nlohmann::json& items = _table["items"];

	float totalWeight = 0.0f;
	for (const auto& item : items)
	{
		totalWeight += (std::max)(0.0f, item.value("weight", 0.0f));
	}

	ImGui::Separator();
	ImGui::Text(U8("重みの合計: %.2f　／　1箱あたり平均 %.1f 個"), totalWeight, avgDropNum);
	ImGui::TextDisabled(U8("出現率 = 重み ÷ 合計（1個ごとの抽選）。平均個数 = 1箱あたりに出る数の目安"));

	constexpr ImGuiTableFlags flags =
		ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit;

	int removeIndex = -1;
	if (ImGui::BeginTable("Items", 6, flags))
	{
		ImGui::TableSetupColumn(U8("種類"));
		ImGui::TableSetupColumn(U8("アイテム"));
		ImGui::TableSetupColumn(U8("重み"));
		ImGui::TableSetupColumn(U8("出現率"));
		ImGui::TableSetupColumn(U8("平均個数/箱"));
		ImGui::TableSetupColumn("");
		ImGui::TableHeadersRow();

		for (int i = 0; i < static_cast<int>(items.size()); ++i)
		{
			if (DrawItemRow(items[i], i, totalWeight, avgDropNum))
			{
				removeIndex = i;
			}
		}

		ImGui::EndTable();
	}

	if (removeIndex >= 0)
	{
		items.erase(static_cast<size_t>(removeIndex));
		m_dirty = true;
	}

	// ---- 追加・調整 ----
	auto addItem = [&](int _type)
	{
		const std::vector<MasterItem>& master = GetMaster(_type);
		nlohmann::json item;
		item["type"] = _type;
		item["id"] = master.empty() ? 0 : master[0].id;
		item["name"] = master.empty() ? std::string{} : master[0].name;
		item["weight"] = 10.0;
		items.push_back(item);
		m_dirty = true;
	};

	if (ImGui::Button(U8("ポーションを追加"))) { addItem(ItemType_Potion); }
	ImGui::SameLine();
	if (ImGui::Button(U8("強化素材を追加"))) { addItem(ItemType_Material); }
	ImGui::SameLine();
	if (ImGui::Button(U8("重みの合計を100にそろえる")) && totalWeight > 0.0f)
	{
		// 比率は変えずに、重み = 出現率(%) になるようにする
		for (auto& item : items)
		{
			const double w = (std::max)(0.0f, item.value("weight", 0.0f)) / totalWeight * 100.0;
			item["weight"] = std::round(w * 100.0) / 100.0;
		}
		m_dirty = true;
	}

	// ---- 注意（保存はできる） ----
	std::set<std::pair<int, int>> seen;
	for (const auto& item : items)
	{
		const std::pair<int, int> key = { item.value("type", -1), item.value("id", -1) };
		if (!seen.insert(key).second)
		{
			ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.3f, 1.0f),
				U8("注意: %s が2回以上入っています（重みは合算されます）"), FindItemName(key.first, key.second).c_str());
		}
	}

	// ---- エラー（保存できない） ----
	const std::vector<std::string> errors = Validate(_tableName, _table);
	if (errors.empty())
	{
		ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "OK");
	}
	for (const std::string& e : errors)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s", e.c_str());
	}
}

bool TreasureChestTableEditor::DrawItemRow(nlohmann::json& _item, int _index, float _totalWeight, float _avgDropNum)
{
	bool remove = false;

	ImGui::PushID(_index);
	ImGui::TableNextRow();

	int type = _item.value("type", 0);
	int id = _item.value("id", 0);

	// ---- 種類 ----
	ImGui::TableNextColumn();
	const char* typeNames[ItemTypeSiz] = { U8("ポーション"), U8("強化素材") };
	int selectType = (type >= 0 && type < ItemTypeSiz) ? type : 0;
	ImGui::SetNextItemWidth(110.0f);
	if (ImGui::Combo("##type", &selectType, typeNames, ItemTypeSiz) && selectType != type)
	{
		// 種類を変えたら、その種類の先頭のアイテムにする
		const std::vector<MasterItem>& master = GetMaster(selectType);
		_item["type"] = selectType;
		_item["id"] = master.empty() ? 0 : master[0].id;
		_item["name"] = master.empty() ? std::string{} : master[0].name;
		type = selectType;
		id = _item.value("id", 0);
		m_dirty = true;
	}

	// ---- アイテム ----
	ImGui::TableNextColumn();
	std::string current = FindItemName(type, id);
	current = current.empty() ? (U8("? (id ") + std::to_string(id) + ")") : (std::to_string(id) + ": " + current);

	ImGui::SetNextItemWidth(280.0f);
	if (ImGui::BeginCombo("##item", current.c_str()))
	{
		for (const MasterItem& master : GetMaster(type))
		{
			const bool selected = (master.id == id);
			if (ImGui::Selectable((std::to_string(master.id) + ": " + master.name).c_str(), selected))
			{
				_item["id"] = master.id;
				_item["name"] = master.name;
				m_dirty = true;
			}
			if (selected) { ImGui::SetItemDefaultFocus(); }
		}
		ImGui::EndCombo();
	}

	// ---- 重み ----
	ImGui::TableNextColumn();
	float weight = _item.value("weight", 0.0f);
	ImGui::SetNextItemWidth(90.0f);
	if (ImGui::DragFloat("##weight", &weight, 0.5f, 0.0f, 100000.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
	{
		// 10.0000002 のような値で保存されないよう小数2桁に丸める
		_item["weight"] = std::round(static_cast<double>(weight) * 100.0) / 100.0;
		m_dirty = true;
	}

	// ---- 出現率 ----
	const float rate = (_totalWeight > 0.0f) ? (std::max)(0.0f, weight) / _totalWeight : 0.0f;
	ImGui::TableNextColumn();
	ImGui::Text("%.2f%%", rate * 100.0f);

	// ---- 平均個数/箱 ----
	ImGui::TableNextColumn();
	ImGui::Text("%.2f", rate * _avgDropNum);

	// ---- 削除 ----
	ImGui::TableNextColumn();
	if (ImGui::SmallButton(U8("削除"))) { remove = true; }

	ImGui::PopID();
	return remove;
}

std::vector<std::string> TreasureChestTableEditor::Validate(const std::string& _tableName, const nlohmann::json& _table) const
{
	std::vector<std::string> errors;

	if (!_table.is_object())
	{
		errors.push_back(_tableName + U8(": テーブルの形式が違います"));
		return errors;
	}

	const int dropMin = _table.value("dropMin", 1);
	const int dropMax = _table.value("dropMax", dropMin);
	if (dropMin < 0) { errors.push_back(U8("最小個数が0未満です")); }
	if (dropMin > dropMax) { errors.push_back(U8("最小個数が最大個数より大きいです")); }

	if (!_table.contains("items") || !_table["items"].is_array() || _table["items"].empty())
	{
		errors.push_back(U8("アイテムがありません"));
		return errors;
	}

	float totalWeight = 0.0f;
	for (const auto& item : _table["items"])
	{
		const int type = item.value("type", -1);
		const int id = item.value("id", -1);
		const float weight = item.value("weight", 0.0f);

		if (type < 0 || type >= ItemTypeSiz)
		{
			errors.push_back(U8("種類が不正です: type ") + std::to_string(type));
			continue;
		}
		if (FindItemName(type, id).empty())
		{
			errors.push_back(U8("存在しないアイテムです: type ") + std::to_string(type) + " id " + std::to_string(id));
		}
		if (weight < 0.0f)
		{
			errors.push_back(U8("重みがマイナスです: ") + FindItemName(type, id));
		}
		totalWeight += (std::max)(0.0f, weight);
	}

	if (totalWeight <= 0.0f)
	{
		errors.push_back(U8("重みの合計が0なので何も出ません"));
	}

	return errors;
}

std::vector<std::string> TreasureChestTableEditor::ValidateAll() const
{
	std::vector<std::string> errors;
	if (!m_data.contains("tables") || !m_data["tables"].is_object()) { return errors; }

	for (const auto& [name, table] : m_data["tables"].items())
	{
		for (const std::string& e : Validate(name, table))
		{
			errors.push_back(name + ": " + e);
		}
	}
	return errors;
}

std::string TreasureChestTableEditor::MakeNewTableName() const
{
	const nlohmann::json& tables = m_data["tables"];
	for (int i = 0; ; ++i)
	{
		const std::string name = "slot" + std::to_string(i);
		if (!tables.contains(name)) { return name; }
	}
}

nlohmann::json TreasureChestTableEditor::MakeDefaultTable() const
{
	nlohmann::json table;
	table["dropMin"] = 2;
	table["dropMax"] = 4;
	table["items"] = nlohmann::json::array();

	// 全アイテムを同じ重みで入れておく
	for (int type = 0; type < ItemTypeSiz; ++type)
	{
		for (const MasterItem& master : GetMaster(type))
		{
			nlohmann::json item;
			item["type"] = type;
			item["id"] = master.id;
			item["name"] = master.name;
			item["weight"] = 10.0;
			table["items"].push_back(item);
		}
	}
	return table;
}
