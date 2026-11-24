#pragma once
//////////////////////////////////////////////////////////////
// ショップの値段を決める ImGui ウィンドウ
// ShopMasterData.json の 買値・売値・規定数 を編集する
// (アイコンと名前を見ながら決められる)
//////////////////////////////////////////////////////////////
class ShopEditor
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ(編集するJSONを読み込む)
	// _filePath ... 編集するShopMasterData.jsonのパス
	explicit ShopEditor(const std::string& _filePath);

	//---- 基本の処理 ----

	// 毎フレーム ImGui のフレーム内で呼ぶ
	// _open ... ウィンドウの×ボタンで閉じたときにfalseを入れるフラグ(nullptrなら×ボタンなし)
	void Draw(bool* _open = nullptr);

	//---- その他の処理 ----

	//JSONファイルを読み込む
	//戻り値 ... 読み込めたらtrue
	bool Load();

	//JSONファイルに保存する(値段の決まりに合っていないときは保存しない)
	//戻り値 ... 保存できたらtrue
	bool Save();

	// 保存した直後のフレームだけtrue(ショップ画面を読み直す合図)
	bool PopSaved() { bool saved = m_savedFlg; m_savedFlg = false; return saved; }

private:

	//---- 型 ----

	// 商品の種類(JSONのキー)
	struct Category
	{
		const char* m_key = nullptr;		// "potions" / "materials" / "weapons"
		const char* m_label = nullptr;		// タブに出す名前
	};

	//---- メンバ変数 ----

	//編集しているJSONファイルのパス
	std::string    m_filePath;

	//編集中のJSONの中身
	nlohmann::json m_data;
	bool           m_dirty = false;		// 未保存の変更があるか
	bool           m_savedFlg = false;	// 保存した直後か
	std::string    m_message;			// 画面に出すメッセージ(保存の結果など)

	// 売値を買値の何%にするか(「売値を一括で決める」用)
	int m_sellRate = 50;

	//読み込んだ画像(パス → 画像)。同じ画像を何回も読み込まないように覚えておく
	std::unordered_map<std::string, std::shared_ptr<KdTexture>> m_texCache;

	//---- 基本の処理 ----

	// 1つの種類の商品を表で表示
	// _category ... 表示する商品の種類
	void DrawCategory(const Category& _category);

	//---- 取得 ----

	// 商品のアイコン・名前画像のパス
	// _key ... 商品の種類のキー("potions"など)
	// _name ... 商品の名前(画像のフォルダ名)
	std::string GetIconPath(const std::string& _key, const std::string& _name) const;

	//商品の名前画像のパス
	// _key ... 商品の種類のキー("potions"など)
	// _name ... 商品の名前(画像のフォルダ名)
	//戻り値 ... 名前画像のパス
	std::string GetNamePath(const std::string& _key, const std::string& _name) const;

	// 画像を読み込んで返す(1回読んだら覚えておく)
	// _path ... 画像のパス
	std::shared_ptr<KdTexture> GetTex(const std::string& _path);

	//---- その他の処理 ----

	// 値段の決まりにあっているか。問題があればメッセージを返す(空ならOK)
	std::vector<std::string> Validate() const;
};
