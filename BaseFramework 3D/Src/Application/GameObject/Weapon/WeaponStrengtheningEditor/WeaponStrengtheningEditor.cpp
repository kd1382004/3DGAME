#include"WeaponStrengtheningEditor.h"

WeaponStrengtheningEditor::WeaponStrengtheningEditor(const std::string& _filePath)
	: m_filePath(_filePath)
{
	Load();
}

void WeaponStrengtheningEditor::Draw(bool* _open)
{
	if (!ImGui::Begin("Weapon Strengthening Editor", _open))
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

	ImGui::Separator();

	if (!m_data.is_object() || m_data.empty())
	{
		ImGui::TextUnformatted(U8("データがありません"));
		ImGui::End();
		return;
	}

	// ---- 武器ごとのタブ ----
	if (ImGui::BeginTabBar("Weapons"))
	{
		for (auto& [weaponKey, weapon] : m_data.items())
		{
			if (!weapon.is_object()) continue;

			if (ImGui::BeginTabItem(weaponKey.c_str()))
			{
				DrawWeapon(weapon);
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}

	ImGui::End();
}

bool WeaponStrengtheningEditor::Load()
{
	std::ifstream file(m_filePath);
	if (!file)
	{
		m_message = U8("読み込み失敗: ファイルが開けません");
		return false;
	}

	nlohmann::json data = nlohmann::json::parse(file, nullptr, false);
	if (data.is_discarded())
	{
		m_message = U8("読み込み失敗: JSONが壊れています");
		return false;
	}

	m_data = std::move(data);
	m_dirty = false;
	m_message = U8("読み込みました");
	return true;
}

bool WeaponStrengtheningEditor::Save()
{
	std::ofstream file(m_filePath);
	if (!file)
	{
		m_message = U8("保存失敗: ファイルが開けません");
		return false;
	}

	file << m_data.dump(1, '\t'); // タブインデントで保存
	m_dirty = false;
	m_message = U8("保存しました");
	return true;
}

void WeaponStrengtheningEditor::DrawWeapon(nlohmann::json& _weapon)
{
	constexpr ImGuiTableFlags flags =
		ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit;

	if (!ImGui::BeginTable("Items", 8, flags)) return;

	ImGui::TableSetupColumn(U8("項目"));
	ImGui::TableSetupColumn(U8("ノードID"));
	ImGui::TableSetupColumn("LvMax");
	ImGui::TableSetupColumn(U8("アイテムID"));
	ImGui::TableSetupColumn(U8("初期必要数"));
	ImGui::TableSetupColumn(U8("増加率"));
	ImGui::TableSetupColumn(U8("前提ノード"));
	ImGui::TableSetupColumn(U8("必要数 (Lv0→1, 1→2, ...)"));
	ImGui::TableHeadersRow();

	int rowIndex = 0; // MyNodeID がない項目に入れる初期値（表示順の番号）
	for (auto& [itemKey, item] : _weapon.items())
	{
		if (!item.is_object()) continue;

		ImGui::PushID(itemKey.c_str());
		ImGui::TableNextRow();

		ImGui::TableNextColumn(); ImGui::TextUnformatted(itemKey.c_str());
		// MyNodeID がまだないデータには表示順の番号を入れておく
		if (!item.contains("MyNodeID"))
		{
			item["MyNodeID"] = rowIndex;
			m_dirty = true;
		}
		ImGui::TableNextColumn(); EditInt(item, "MyNodeID", 0, 999);
		ImGui::TableNextColumn(); EditInt(item, "LvMax", 0, 99);
		ImGui::TableNextColumn(); EditInt(item, "strengtheningItem_ID", 0, 999);
		ImGui::TableNextColumn(); EditInt(item, "InitialNextLvNum", 1, 9999);
		ImGui::TableNextColumn(); EditFloat(item, "GrowthRate", 1.0f, 3.0f);
		ImGui::TableNextColumn(); EditInt(item, "PreconditionNodeID", -1, 999);
		ImGui::TableNextColumn(); DrawPreview(item);

		ImGui::PopID();
		++rowIndex;
	}

	ImGui::EndTable();
}

void WeaponStrengtheningEditor::EditInt(nlohmann::json& _obj, const char* _key, int _min, int _max)
{
	int v = _obj.value(_key, 0);
	ImGui::SetNextItemWidth(100.0f);
	if (ImGui::InputInt((std::string("##") + _key).c_str(), &v))
	{
		_obj[_key] = std::clamp(v, _min, _max);
		m_dirty = true;
	}
}

void WeaponStrengtheningEditor::EditFloat(nlohmann::json& _obj, const char* _key, float _min, float _max)
{
	float v = _obj.value(_key, 1.0f);
	ImGui::SetNextItemWidth(80.0f);
	if (ImGui::DragFloat((std::string("##") + _key).c_str(), &v, 0.01f, _min, _max, "%.2f",
		ImGuiSliderFlags_AlwaysClamp))
	{
		// 1.2000000476... のような値で保存されないよう小数2桁に丸める
		_obj[_key] = std::round(static_cast<double>(v) * 100.0) / 100.0;
		m_dirty = true;
	}
}

void WeaponStrengtheningEditor::DrawPreview(const nlohmann::json& _item)
{
	const int   lvMax = _item.value("LvMax", 0);
	const int   initial = _item.value("InitialNextLvNum", 1);
	const float rate = _item.value("GrowthRate", 1.0f);

	std::string text;
	int total = 0;
	for (int lv = 0; lv < lvMax; ++lv)
	{
		const int num = static_cast<int>(std::ceil(initial * std::pow(rate, lv)));
		total += num;
		if (!text.empty()) text += ", ";
		text += std::to_string(num);
	}
	text += U8("  (計 ") + std::to_string(total) + ")";

	ImGui::TextUnformatted(text.c_str());
}