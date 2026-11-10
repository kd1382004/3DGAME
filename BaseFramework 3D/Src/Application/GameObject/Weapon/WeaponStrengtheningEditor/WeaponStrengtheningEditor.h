#pragma once
// 武器強化マスタ（WeaponsStrengtheningInfo.json）を編集する ImGui ウィンドウ
class WeaponStrengtheningEditor
{
public:
	explicit WeaponStrengtheningEditor(const std::string& _filePath);

	// 毎フレーム ImGui のフレーム内で呼ぶ
	void Draw(bool* _open = nullptr);

	bool Load();
	bool Save();

	const nlohmann::json& GetData() const { return m_data; }

private:
	// 1つの武器の全項目を表で表示
	void DrawWeapon(nlohmann::json& _weapon);

	void EditInt(nlohmann::json& _obj, const char* _key, int _min, int _max);
	void EditFloat(nlohmann::json& _obj, const char* _key, float _min, float _max);

	// 各レベルで必要な個数と合計を表示
	void DrawPreview(const nlohmann::json& _item);

private:
	std::string    m_filePath;
	nlohmann::json m_data;
	bool           m_dirty = false; // 未保存の変更があるか
	std::string    m_message;
};