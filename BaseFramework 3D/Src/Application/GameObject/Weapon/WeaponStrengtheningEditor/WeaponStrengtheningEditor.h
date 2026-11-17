#pragma once
// 武器強化マスタ（WeaponsStrengtheningInfo.json）を編集する ImGui ウィンドウ
class WeaponStrengtheningEditor
{
public:
	//コンストラクタ(ファイルを読み込む)
	// _filePath ... 編集するjsonのパス
	explicit WeaponStrengtheningEditor(const std::string& _filePath);

	// 毎フレーム ImGui のフレーム内で呼ぶ
	// _open ... ウィンドウを開いているか(×で閉じるとfalseになる)
	void Draw(bool* _open = nullptr);

	//ファイルを読み込む
	//戻り値 ... 読めたらtrue
	bool Load();
	//ファイルに保存する
	//戻り値 ... 保存できたらtrue
	bool Save();

	//読み込んだjsonを返す
	//戻り値 ... 読み込んだjson
	const nlohmann::json& GetData() const { return m_data; }

private:
	// 1つの武器の全項目を表で表示
	// _weapon ... 武器のjson
	void DrawWeapon(nlohmann::json& _weapon);

	//整数の項目を編集する
	// _obj ... 編集するjson
	// _key ... 項目の名前
	// _min ... 最小値
	// _max ... 最大値
	void EditInt(nlohmann::json& _obj, const char* _key, int _min, int _max);
	//小数の項目を編集する
	// _obj ... 編集するjson
	// _key ... 項目の名前
	// _min ... 最小値
	// _max ... 最大値
	void EditFloat(nlohmann::json& _obj, const char* _key, float _min, float _max);

	// 各レベルで必要な個数と合計を表示
	// _item ... 強化項目のjson
	void DrawPreview(const nlohmann::json& _item);

	// MyNodeID / PreconditionNodeID の検証。問題があればメッセージを返す（空なら OK）
	// _weapon ... 武器のjson
	std::vector<std::string> Validate(const nlohmann::json& _weapon) const;

private:
	//編集するファイルのパス
	std::string    m_filePath;
	//読み込んだjson
	nlohmann::json m_data;
	bool           m_dirty = false; // 未保存の変更があるか
	std::string    m_message;
};