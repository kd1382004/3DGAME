#include "ShopEditor.h"

namespace
{
	// タブの順番(ショップ画面と同じ)
	const char* kCategoryKey[]   = { "potions", "materials", "weapons" };
	const char* kCategoryLabel[] = { U8("消耗品"), U8("素材"), U8("武器") };
	constexpr int kCategoryNum = 3;

	constexpr float kIconSiz = 40.0f;	// 表に出すアイコンの大きさ
	constexpr float kNameW   = 150.0f;	// 名前画像の幅(元は200×53)
	constexpr float kNameH   = 40.0f;
}

ShopEditor::ShopEditor(const std::string& _filePath)
	: m_filePath(_filePath)
{
	Load();
}

void ShopEditor::Draw(bool* _open)
{
	if (!ImGui::Begin("Shop Editor", _open))
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

	// ---- 売値を一括で決める ----
	ImGui::SetNextItemWidth(120.0f);
	ImGui::SliderInt(U8("売値 = 買値の %"), &m_sellRate, 0, 100);
	ImGui::SameLine();
	if (ImGui::Button(U8("全部の売値に反映")))
	{
		for (int c = 0; c < kCategoryNum; c++)
		{
			if (!m_data.contains(kCategoryKey[c])) { continue; }
			for (auto& item : m_data[kCategoryKey[c]])
			{
				item["sellPrice"] = item.value("buyPrice", 0) * m_sellRate / 100;
			}
		}
		m_dirty = true;
	}

	ImGui::Separator();

	// ---- 決まりに合わないもの(赤字で出す。あると保存できない) ----
	const std::vector<std::string> errors = Validate();
	for (const std::string& error : errors)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s", error.c_str());
	}

	if (!m_data.is_object() || m_data.empty())
	{
		ImGui::TextUnformatted(U8("データがありません"));
		ImGui::End();
		return;
	}

	// ---- 種類ごとのタブ ----
	if (ImGui::BeginTabBar("ShopCategory"))
	{
		for (int c = 0; c < kCategoryNum; c++)
		{
			if (!m_data.contains(kCategoryKey[c])) { continue; }

			if (ImGui::BeginTabItem(kCategoryLabel[c]))
			{
				DrawCategory({ kCategoryKey[c], kCategoryLabel[c] });
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}

	ImGui::End();
}

void ShopEditor::DrawCategory(const Category& _category)
{
	constexpr ImGuiTableFlags flags =
		ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit;

	if (!ImGui::BeginTable(_category.key, 6, flags)) return;

	ImGui::TableSetupColumn(U8("アイコン"));
	ImGui::TableSetupColumn(U8("名前"));
	ImGui::TableSetupColumn(U8("買値"));
	ImGui::TableSetupColumn(U8("売値"));
	ImGui::TableSetupColumn(U8("売値 / 買値"));
	ImGui::TableSetupColumn(U8("規定数"));
	ImGui::TableHeadersRow();

	nlohmann::json& items = m_data[_category.key];
	for (auto& item : items)
	{
		const int id = item.value("id", -1);
		const std::string name = item.value("name", std::string());

		ImGui::PushID(id);
		ImGui::TableNextRow();

		// ---- アイコン ----
		ImGui::TableNextColumn();
		std::shared_ptr<KdTexture> spIcon = GetTex(GetIconPath(_category.key, name));
		if (spIcon && spIcon->WorkSRView())
		{
			ImGui::Image((ImTextureID)spIcon->WorkSRView(), ImVec2(kIconSiz, kIconSiz));
		}
		else
		{
			ImGui::Dummy(ImVec2(kIconSiz, kIconSiz));
		}

		// ---- 名前(ゲーム内の名前画像 + JSONの名前) ----
		ImGui::TableNextColumn();
		std::shared_ptr<KdTexture> spName = GetTex(GetNamePath(_category.key, name));
		if (spName && spName->WorkSRView())
		{
			ImGui::Image((ImTextureID)spName->WorkSRView(), ImVec2(kNameW, kNameH));
		}
		ImGui::TextDisabled("%d : %s", id, name.c_str());

		// ---- 買値 ----
		ImGui::TableNextColumn();
		int buyPrice = item.value("buyPrice", 0);
		ImGui::SetNextItemWidth(100.0f);
		if (ImGui::InputInt("##buy", &buyPrice, 10, 100))
		{
			item["buyPrice"] = std::max(0, buyPrice);
			m_dirty = true;
		}

		// ---- 売値 ----
		ImGui::TableNextColumn();
		int sellPrice = item.value("sellPrice", 0);
		ImGui::SetNextItemWidth(100.0f);
		if (ImGui::InputInt("##sell", &sellPrice, 5, 50))
		{
			item["sellPrice"] = std::max(0, sellPrice);
			m_dirty = true;
		}
		if (ImGui::SmallButton(U8("半分")))
		{
			item["sellPrice"] = item.value("buyPrice", 0) / 2;
			m_dirty = true;
		}

		// ---- 売値 / 買値(目安) ----
		ImGui::TableNextColumn();
		const int buy  = item.value("buyPrice", 0);
		const int sell = item.value("sellPrice", 0);
		if (buy > 0)
		{
			const int rate = sell * 100 / buy;
			// 売値が買値より高いと、買って売るだけでお金が増えてしまう
			const ImVec4 color = (sell > buy) ? ImVec4(1.0f, 0.3f, 0.3f, 1.0f) : ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
			ImGui::TextColored(color, "%d %%", rate);
		}
		else
		{
			ImGui::TextDisabled("-");
		}

		// ---- 規定数(在庫が売り切れたらここまで補充) ----
		ImGui::TableNextColumn();
		int startStock = item.value("startStock", 0);
		ImGui::SetNextItemWidth(80.0f);
		if (ImGui::InputInt("##stock", &startStock, 1, 5))
		{
			item["startStock"] = std::max(0, startStock);
			m_dirty = true;
		}

		ImGui::PopID();
	}

	ImGui::EndTable();
}

std::string ShopEditor::GetIconPath(const std::string& _key, const std::string& _name) const
{
	const std::string item = "Asset/Textures/GameUI/Item/";
	if (_key == "potions")   { return item + "Potion/" + _name + "/Icon.png"; }
	if (_key == "materials") { return item + "WeaponsStrengtheningItem/" + _name + "/Icon.png"; }
	if (_key == "weapons")   { return item + "PlayerInventory/WeponStrengtheningIcon/" + _name + "/WeponStrengtheningIcon8.png"; }
	return "";
}

std::string ShopEditor::GetNamePath(const std::string& _key, const std::string& _name) const
{
	const std::string item = "Asset/Textures/GameUI/Item/";
	if (_key == "potions")   { return item + "Potion/" + _name + "/SelectNameBase.png"; }
	if (_key == "materials") { return item + "WeaponsStrengtheningItem/" + _name + "/SelectNameBase.png"; }
	if (_key == "weapons")   { return item + "WeaponsStrengthening/" + _name + "/Icon/SelectNameBase.png"; }
	return "";
}

std::shared_ptr<KdTexture> ShopEditor::GetTex(const std::string& _path)
{
	if (_path.empty()) { return nullptr; }

	auto it = m_texCache.find(_path);
	if (it != m_texCache.end()) { return it->second; }

	// 読めなかったときもnullptrを覚えておく(毎フレーム読み直さないように)
	std::shared_ptr<KdTexture> spTex = std::make_shared<KdTexture>();
	if (!spTex->Load(_path)) { spTex = nullptr; }

	m_texCache[_path] = spTex;
	return spTex;
}

std::vector<std::string> ShopEditor::Validate() const
{
	std::vector<std::string> errors;

	for (int c = 0; c < kCategoryNum; c++)
	{
		if (!m_data.contains(kCategoryKey[c])) { continue; }
		for (const auto& item : m_data[kCategoryKey[c]])
		{
			const std::string name = item.value("name", std::string());
			const int buy  = item.value("buyPrice", 0);
			const int sell = item.value("sellPrice", 0);

			// 売値が買値より高いと、買って売るだけでお金が増やせてしまう
			if (sell > buy)
			{
				errors.push_back(std::string(kCategoryLabel[c]) + " / " + name + U8(" : 売値が買値より高いです"));
			}
		}
	}
	return errors;
}

bool ShopEditor::Load()
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

bool ShopEditor::Save()
{
	// 決まりに合わないものが1件でもあれば保存しない
	if (!Validate().empty())
	{
		m_message = U8("保存失敗: 赤字のエラーを直してください");
		return false;
	}

	std::ofstream file(m_filePath);
	if (!file)
	{
		m_message = U8("保存失敗: ファイルが開けません");
		return false;
	}

	file << m_data.dump(4);
	m_dirty = false;
	m_savedFlg = true;
	m_message = U8("保存しました");
	return true;
}
