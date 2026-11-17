#pragma once
//////////////////////////////////////////////////////////////
// ショップの値段を決める ImGui ウィンドウ
// ShopMasterData.json の 買値・売値・規定数 を編集する
// (アイコンと名前を見ながら決められる)
//////////////////////////////////////////////////////////////
class ShopEditor
{
public:
	explicit ShopEditor(const std::string& _filePath);

	// 毎フレーム ImGui のフレーム内で呼ぶ
	void Draw(bool* _open = nullptr);

	bool Load();
	bool Save();

	// 保存した直後のフレームだけtrue(ショップ画面を読み直す合図)
	bool PopSaved() { bool saved = m_savedFlg; m_savedFlg = false; return saved; }

private:

	// 商品の種類(JSONのキー)
	struct Category
	{
		const char* key;		// "potions" / "materials" / "weapons"
		const char* label;		// タブに出す名前
	};

	// 1つの種類の商品を表で表示
	void DrawCategory(const Category& _category);

	// 商品のアイコン・名前画像のパス
	std::string GetIconPath(const std::string& _key, const std::string& _name) const;
	std::string GetNamePath(const std::string& _key, const std::string& _name) const;

	// 画像を読み込んで返す(1回読んだら覚えておく)
	std::shared_ptr<KdTexture> GetTex(const std::string& _path);

	// 値段の決まりにあっているか。問題があればメッセージを返す(空ならOK)
	std::vector<std::string> Validate() const;

private:
	std::string    m_filePath;
	nlohmann::json m_data;
	bool           m_dirty = false;		// 未保存の変更があるか
	bool           m_savedFlg = false;	// 保存した直後か
	std::string    m_message;

	// 売値を買値の何%にするか(「売値を一括で決める」用)
	int m_sellRate = 50;

	std::unordered_map<std::string, std::shared_ptr<KdTexture>> m_texCache;
};
