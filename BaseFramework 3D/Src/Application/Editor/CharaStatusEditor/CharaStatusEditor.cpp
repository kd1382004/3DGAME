#include "CharaStatusEditor.h"

namespace
{
	// 編集するキャラ(名前・Status.json のパス・プレイヤーか)
	// キャラを増やしたら、ここに1行足す
	struct CharaDef
	{
		const char* m_name;
		const char* m_filePath;
		bool        m_isPlayer;
	};
	const CharaDef kCharaDefList[] =
	{
		{ U8("プレイヤー"),     "Asset/Data/ObjeData/Character/Praeyr/Status/Status.json",          true  },
		{ U8("ゴブリン"),       "Asset/Data/ObjeData/Character/Enemy/Goblin/Status/Status.json",    false },
		{ U8("ゴーレム"),       "Asset/Data/ObjeData/Character/Enemy/Golem/Status/Status.json",     false },
		{ U8("巨人(ボス)"),     "Asset/Data/ObjeData/Character/Enemy/Boss/Giant/Status/Status.json", false },
	};

	// グループ(JSONの1段目のキー)と、画面に出す見出し
	struct GroupDef
	{
		const char* m_key;
		const char* m_label;
	};
	const GroupDef kGroupList[] =
	{
		{ "HP",        U8("HP")       },
		{ "attck",     U8("攻撃力")   },
		{ "defense",   U8("防御力")   },
		{ "moveSpeed", U8("移動速度") },
		{ "stamina",   U8("スタミナ") },
		{ "LV",        U8("レベル")   },
	};

	const ImVec4 kErrorColor   = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);	// エラーの文字色
	const ImVec4 kDirtyColor   = ImVec4(1.0f, 0.6f, 0.2f, 1.0f);	// 未保存の文字色
	constexpr float kInputW    = 140.0f;							// 入力欄の幅
}

//編集する値の一覧(CharacterBase::LoadCharaStatus / SaveCharaStatus のキーと同じ)
//キーの打ち間違い(attck, baseAttckPowe など)もファイルに合わせてそのまま使う
// { グループ, キー, 表示名, 説明, 無いときに代わりに使うキー, +/-の量, 整数か }
const std::vector<CharaStatusEditor::FieldInfo>& CharaStatusEditor::GetFieldList()
{
	static const std::vector<FieldInfo> kFieldList =
	{
		// HP
		{ "HP", "baseHP", U8("基礎HP"),   U8("HPの元の値。敵はレベル(階)が上がると、この値を元に最大HPが増える"),               nullptr,   1.0f, false },
		{ "HP", "maxHP",  U8("最大HP"),   U8("始めの最大HP。無ければ基礎HPと同じ"),               "baseHP",  1.0f, false },
		{ "HP", "nowHP",  U8("始めのHP"), U8("出てきたときのHP。無ければ最大HPと同じ"),           "maxHP",   1.0f, false },

		// 攻撃力
		{ "attck", "baseAttckPowe", U8("基礎攻撃力"), U8("ダメージ計算に使う攻撃力。敵はレベル(階)が上がると、この値を元に増える"),                    nullptr, 1.0f, false },
		{ "attck", "addAttack",     U8("増加攻撃力"), U8("バフなどで足される攻撃力。ふつうは0"),          nullptr, 1.0f, false },

		// 防御力
		{ "defense", "baseDefensePowe", U8("基礎防御力"), U8("受けるダメージを減らす防御力"),             nullptr, 1.0f, false },
		{ "defense", "addDefense",      U8("増加防御力"), U8("バフなどで足される防御力。ふつうは0"),       nullptr, 1.0f, false },

		// 移動速度
		{ "moveSpeed", "baseSpeed",    U8("基礎速度"),       U8("歩き・ダッシュの元になる速度"),         nullptr, 0.1f, false },
		{ "moveSpeed", "walkMovePowe", U8("歩きで足す速度"), U8("歩く速度 = 基礎速度 + この値"),         nullptr, 0.1f, false },
		{ "moveSpeed", "runMovePowe",  U8("走りで足す速度"), U8("走る速度 = 基礎速度 + この値"),         nullptr, 0.1f, false },

		// スタミナ
		{ "stamina", "staminaMax",             U8("最大スタミナ"),       U8("スタミナの上限"),                              nullptr,      1.0f, false },
		{ "stamina", "staminaNow",             U8("始めのスタミナ"),     U8("出てきたときのスタミナ。無ければ最大と同じ"),  "staminaMax", 1.0f, false },
		{ "stamina", "dashStaminaDrainPerSec", U8("ダッシュの消費(秒)"), U8("ダッシュ中に1秒で減る量"),                     nullptr,      0.5f, false },
		{ "stamina", "staminaRegenPerSec",     U8("自然回復(秒)"),       U8("1秒で回復する量(回復し始めの値)"),             nullptr,      0.1f, false },
		{ "stamina", "staminaRegenBuff",       U8("回復バフ(秒)"),       U8("バフで足される回復量。ふつうは0"),             nullptr,      0.1f, false },

		// レベル
		{ "LV", "LV", U8("レベル"), U8("始めのレベル(1以上)"), nullptr, 1.0f, true },
	};
	return kFieldList;
}

CharaStatusEditor::CharaStatusEditor(const std::string& _playerSaveFilePath)
	: m_playerSaveFilePath(_playerSaveFilePath)
{
	for (const CharaDef& def : kCharaDefList)
	{
		CharaFile chara;
		chara.m_name     = def.m_name;
		chara.m_filePath = def.m_filePath;
		chara.m_isPlayer = def.m_isPlayer;
		m_charaList.push_back(chara);
	}

	Load();
}

void CharaStatusEditor::Draw(bool* _open)
{
	if (!ImGui::Begin("Chara Status Editor", _open))
	{
		ImGui::End();
		return;
	}

	// ---- 上部ボタン ----
	if (ImGui::Button("Load")) { Load(); }
	if (ImGui::IsItemHovered()) { ImGui::SetTooltip(U8("全キャラをファイルから読み直す(未保存の変更は消える)")); }
	ImGui::SameLine();
	if (ImGui::Button("Save")) { Save(); }
	if (ImGui::IsItemHovered()) { ImGui::SetTooltip(U8("変更したキャラだけ保存する")); }

	bool anyDirty = false;
	for (const CharaFile& chara : m_charaList)
	{
		if (chara.m_dirty) { anyDirty = true; }
	}
	if (anyDirty)
	{
		ImGui::SameLine();
		ImGui::TextColored(kDirtyColor, U8("* 未保存"));
	}
	if (!m_message.empty())
	{
		ImGui::SameLine();
		ImGui::TextUnformatted(m_message.c_str());
	}

	ImGui::TextDisabled(U8("敵は次に出てくる敵から、プレイヤーは死んでやり直したときから反映されます"));
	ImGui::Separator();

	// ---- キャラごとのタブ + 一覧 ----
	if (ImGui::BeginTabBar("CharaStatusTab"))
	{
		for (CharaFile& chara : m_charaList)
		{
			// 未保存のタブは名前に * を付ける(IDは名前で固定)
			std::string tabName = chara.m_name + (chara.m_dirty ? " *" : "") + "###" + chara.m_name;
			if (ImGui::BeginTabItem(tabName.c_str()))
			{
				DrawChara(chara);
				ImGui::EndTabItem();
			}
		}

		if (ImGui::BeginTabItem(U8("一覧")))
		{
			DrawCompareTable();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

bool CharaStatusEditor::Load()
{
	bool allLoaded = true;
	for (CharaFile& chara : m_charaList)
	{
		if (!LoadChara(chara)) { allLoaded = false; }
	}

	m_message = allLoaded ? U8("読み込みました") : U8("読み込めないキャラがあります");
	return allLoaded;
}

bool CharaStatusEditor::Save()
{
	// 決まりに合わないものが1件でもあれば保存しない
	if (HasError())
	{
		m_message = U8("保存失敗: 赤字のエラーを直してください");
		return false;
	}

	int savedNum = 0;
	bool allSaved = true;
	for (CharaFile& chara : m_charaList)
	{
		if (!chara.m_loaded || !chara.m_dirty) { continue; }

		if (SaveChara(chara)) { savedNum++; }
		else { allSaved = false; }
	}

	if (!allSaved)
	{
		m_message = U8("保存失敗: ファイルが開けないキャラがあります");
		return false;
	}

	if (savedNum == 0)
	{
		m_message = U8("変更はありません");
		return true;
	}

	m_savedFlg = true;
	m_message = std::to_string(savedNum) + U8("体分を保存しました");
	return true;
}

void CharaStatusEditor::DrawChara(CharaFile& _chara)
{
	ImGui::TextDisabled("%s", _chara.m_filePath.c_str());

	if (!_chara.m_loaded)
	{
		ImGui::TextColored(kErrorColor, U8("このファイルが読み込めませんでした"));
		if (ImGui::Button(U8("読み直す"))) { LoadChara(_chara); }
		return;
	}

	// ---- このキャラだけ元に戻す ----
	if (ImGui::Button(U8("このキャラの変更を戻す")))
	{
		LoadChara(_chara);
	}

	// ---- 決まりに合わないもの(赤字で出す。あると保存できない) ----
	for (const std::string& error : Validate(_chara))
	{
		ImGui::TextColored(kErrorColor, "%s", error.c_str());
	}

	ImGui::Separator();

	// ---- グループごとに値を並べる ----
	for (const GroupDef& group : kGroupList)
	{
		if (!ImGui::CollapsingHeader(group.m_label, ImGuiTreeNodeFlags_DefaultOpen)) { continue; }

		ImGui::PushID(group.m_key);

		for (const FieldInfo& field : GetFieldList())
		{
			if (std::string_view(field.m_group) != group.m_key) { continue; }

			const FieldInfo* pField = &field;

			ImGui::PushID(field.m_key);
			ImGui::SetNextItemWidth(kInputW);

			if (pField->m_isInt)
			{
				int value = static_cast<int>(GetValue(_chara.m_data, *pField));
				if (ImGui::InputInt(pField->m_label, &value, static_cast<int>(pField->m_step), static_cast<int>(pField->m_step) * 10))
				{
					SetValue(_chara.m_data, *pField, static_cast<float>(value));
					_chara.m_dirty = true;
				}
			}
			else
			{
				float value = GetValue(_chara.m_data, *pField);
				if (ImGui::InputFloat(pField->m_label, &value, pField->m_step, pField->m_step * 10.0f, "%.2f"))
				{
					SetValue(_chara.m_data, *pField, value);
					_chara.m_dirty = true;
				}
			}

			if (ImGui::IsItemHovered()) { ImGui::SetTooltip("%s", pField->m_help); }

			// ファイルに書いていない値(代わりの値を出している)ことを知らせる
			if (!_chara.m_data.contains(pField->m_group) || !_chara.m_data[pField->m_group].contains(pField->m_key))
			{
				ImGui::SameLine();
				ImGui::TextDisabled(U8("(未設定)"));
			}

			ImGui::PopID();
		}

		// ---- グループごとのおまけ ----
		const std::string groupKey = group.m_key;
		if (groupKey == "HP")
		{
			if (ImGui::SmallButton(U8("最大HP・始めのHPを基礎HPにそろえる")))
			{
				const float baseHP = GetValue(_chara.m_data, *FindField("HP", "baseHP"));
				SetValue(_chara.m_data, *FindField("HP", "maxHP"), baseHP);
				SetValue(_chara.m_data, *FindField("HP", "nowHP"), baseHP);
				_chara.m_dirty = true;
			}
		}
		else if (groupKey == "moveSpeed")
		{
			const float baseSpeed = GetValue(_chara.m_data, *FindField("moveSpeed", "baseSpeed"));
			const float walk      = GetValue(_chara.m_data, *FindField("moveSpeed", "walkMovePowe"));
			const float run       = GetValue(_chara.m_data, *FindField("moveSpeed", "runMovePowe"));
			ImGui::Text(U8("歩く速度 : %.2f   走る速度 : %.2f"), baseSpeed + walk, baseSpeed + run);
		}
		else if (groupKey == "stamina")
		{
			if (ImGui::SmallButton(U8("始めのスタミナを最大にそろえる")))
			{
				const float staminaMax = GetValue(_chara.m_data, *FindField("stamina", "staminaMax"));
				SetValue(_chara.m_data, *FindField("stamina", "staminaNow"), staminaMax);
				_chara.m_dirty = true;
			}

			const float drain = GetValue(_chara.m_data, *FindField("stamina", "dashStaminaDrainPerSec"));
			const float max   = GetValue(_chara.m_data, *FindField("stamina", "staminaMax"));
			if (drain > 0.0f)
			{
				ImGui::Text(U8("満タンからダッシュできる時間 : %.1f 秒"), max / drain);
			}
		}

		ImGui::PopID();
	}

	// ---- プレイヤーだけ : セーブデータを初期ステータスに戻す ----
	if (_chara.m_isPlayer)
	{
		ImGui::Separator();
		ImGui::TextUnformatted(U8("セーブデータ"));
		ImGui::TextDisabled("%s", m_playerSaveFilePath.c_str());

		// 赤字のエラーがあるときは押せない
		if (Validate(_chara).empty())
		{
			if (ImGui::Button(U8("セーブデータを今の初期ステータスで上書きする")))
			{
				ResetPlayerSaveData(_chara);
			}
		}
		else
		{
			ImGui::TextColored(kErrorColor, U8("赤字のエラーを直すと、セーブデータを上書きできます"));
		}

		ImGui::TextDisabled(U8("ゲームを始めるときはセーブデータの方を読むので、すぐ試したいときに使う"));
		ImGui::TextDisabled(U8("※ ゲーム中に押しても、その階を出るときにプレイヤーの今の値で上書きされます"));
	}
}

void CharaStatusEditor::DrawCompareTable()
{
	constexpr ImGuiTableFlags flags =
		ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollX;

	const int columnNum = 1 + static_cast<int>(m_charaList.size());
	if (!ImGui::BeginTable("CharaStatusCompare", columnNum, flags)) { return; }

	ImGui::TableSetupColumn(U8("項目"));
	for (const CharaFile& chara : m_charaList)
	{
		ImGui::TableSetupColumn(chara.m_name.c_str());
	}
	ImGui::TableHeadersRow();

	for (const FieldInfo& field : GetFieldList())
	{
		const FieldInfo* pField = &field;

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextUnformatted(pField->m_label);

		for (const CharaFile& chara : m_charaList)
		{
			ImGui::TableNextColumn();
			if (!chara.m_loaded)
			{
				ImGui::TextDisabled("-");
				continue;
			}

			const float value = GetValue(chara.m_data, *pField);
			if (pField->m_isInt) { ImGui::Text("%d", static_cast<int>(value)); }
			else                 { ImGui::Text("%.2f", value); }
		}
	}

	ImGui::EndTable();
}

float CharaStatusEditor::GetValue(const nlohmann::json& _data, const FieldInfo& _field) const
{
	// ファイルに書いてあればその値
	if (_data.contains(_field.m_group))
	{
		const nlohmann::json& group = _data[_field.m_group];
		if (group.is_object() && group.contains(_field.m_key) && group[_field.m_key].is_number())
		{
			return group[_field.m_key].get<float>();
		}
	}

	// 無ければ代わりのキーの値(LoadCharaStatus と同じ決まり)
	if (_field.m_fallbackKey)
	{
		const FieldInfo* pFallback = FindField(_field.m_group, _field.m_fallbackKey);
		if (pFallback) { return GetValue(_data, *pFallback); }
	}

	return 0.0f;
}

const CharaStatusEditor::FieldInfo* CharaStatusEditor::FindField(const char* _group, const char* _key) const
{
	for (const FieldInfo& field : GetFieldList())
	{
		if (std::string_view(field.m_group) == _group && std::string_view(field.m_key) == _key)
		{
			return &field;
		}
	}
	return nullptr;
}

bool CharaStatusEditor::HasError() const
{
	for (const CharaFile& chara : m_charaList)
	{
		if (chara.m_loaded && !Validate(chara).empty()) { return true; }
	}
	return false;
}

void CharaStatusEditor::SetValue(nlohmann::json& _data, const FieldInfo& _field, float _value)
{
	if (_field.m_isInt)
	{
		_data[_field.m_group][_field.m_key] = static_cast<int>(_value);
	}
	else
	{
		_data[_field.m_group][_field.m_key] = _value;
	}
}

bool CharaStatusEditor::LoadChara(CharaFile& _chara)
{
	_chara.m_dirty = false;

	std::ifstream file(_chara.m_filePath);
	if (!file)
	{
		_chara.m_loaded = false;
		return false;
	}

	nlohmann::json data = nlohmann::json::parse(file, nullptr, false);
	if (data.is_discarded() || !data.is_object())
	{
		_chara.m_loaded = false;
		return false;
	}

	_chara.m_data = std::move(data);
	_chara.m_loaded = true;
	return true;
}

bool CharaStatusEditor::SaveChara(CharaFile& _chara)
{
	std::ofstream file(_chara.m_filePath);
	if (!file) { return false; }

	file << _chara.m_data.dump(4);
	_chara.m_dirty = false;
	return true;
}

bool CharaStatusEditor::ResetPlayerSaveData(const CharaFile& _chara)
{
	// 書いていない値も、LoadCharaStatus で読んだときと同じ値で全部書き出す
	nlohmann::json data = _chara.m_data;
	for (const FieldInfo& field : GetFieldList())
	{
		SetValue(data, field, GetValue(_chara.m_data, field));
	}

	std::ofstream file(m_playerSaveFilePath);
	if (!file)
	{
		m_message = U8("セーブデータが開けませんでした");
		return false;
	}

	file << data.dump(4);
	m_message = U8("セーブデータを初期ステータスで上書きしました");
	return true;
}

std::vector<std::string> CharaStatusEditor::Validate(const CharaFile& _chara) const
{
	std::vector<std::string> errors;
	if (!_chara.m_loaded) { return errors; }

	auto get = [&](const char* _group, const char* _key)
		{
			const FieldInfo* pField = FindField(_group, _key);
			return pField ? GetValue(_chara.m_data, *pField) : 0.0f;
		};

	// マイナスの値は入れられない
	for (const FieldInfo& field : GetFieldList())
	{
		if (GetValue(_chara.m_data, field) < 0.0f)
		{
			errors.push_back(std::string(field.m_label) + U8(" : マイナスにはできません"));
		}
	}

	// HPが0だと出てきた瞬間に死んでしまう
	if (get("HP", "maxHP") <= 0.0f) { errors.push_back(U8("最大HP : 0より大きくしてください")); }
	if (get("HP", "nowHP") <= 0.0f) { errors.push_back(U8("始めのHP : 0より大きくしてください")); }
	if (get("HP", "nowHP") > get("HP", "maxHP")) { errors.push_back(U8("始めのHP : 最大HPより大きくなっています")); }

	// スタミナ
	if (get("stamina", "staminaNow") > get("stamina", "staminaMax")) { errors.push_back(U8("始めのスタミナ : 最大スタミナより大きくなっています")); }

	// レベルは1から
	if (get("LV", "LV") < 1.0f) { errors.push_back(U8("レベル : 1以上にしてください")); }

	return errors;
}
