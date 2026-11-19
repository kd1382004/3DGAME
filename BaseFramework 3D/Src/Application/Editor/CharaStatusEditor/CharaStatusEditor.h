#pragma once
//////////////////////////////////////////////////////////////
// キャラの初期ステータスを決める ImGui ウィンドウ
// キャラごとの Status.json (HP・攻撃力・防御力・移動速度・スタミナ・LV) を編集する
// ・敵 ...... 出てくるとき(Init)にこの値を読む
// ・プレイヤー ... 死んで最初からやり直すときに、この値に戻る
//                 (ふだんはセーブデータ PlayerStatusData.json を読む)
//////////////////////////////////////////////////////////////
class CharaStatusEditor
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ(全キャラの Status.json を読み込む)
	// _playerSaveFilePath ... プレイヤーのセーブデータ(PlayerStatusData.json)のパス
	explicit CharaStatusEditor(const std::string& _playerSaveFilePath);

	//---- 基本の処理 ----

	// 毎フレーム ImGui のフレーム内で呼ぶ
	// _open ... ウィンドウの×ボタンで閉じたときにfalseを入れるフラグ(nullptrなら×ボタンなし)
	void Draw(bool* _open = nullptr);

	//---- その他の処理 ----

	//全キャラのファイルを読み込む(未保存の変更は消える)
	//戻り値 ... 全部読み込めたらtrue
	bool Load();

	//変更したキャラのファイルだけ保存する(赤字のエラーがあるときは保存しない)
	//戻り値 ... 保存できたらtrue
	bool Save();

	// 保存した直後のフレームだけtrue
	bool PopSaved() { bool saved = m_savedFlg; m_savedFlg = false; return saved; }

private:

	//---- 型 ----

	// 編集するキャラ1体分
	struct CharaFile
	{
		std::string    m_name;				// タブに出す名前
		std::string    m_filePath;			// Status.json のパス
		bool           m_isPlayer = false;	// プレイヤーか(セーブデータのボタンを出す)
		nlohmann::json m_data;				// 編集中のJSONの中身
		bool           m_loaded = false;	// 読み込めたか
		bool           m_dirty = false;		// 未保存の変更があるか
	};

	// 編集する値1つ分(JSONのどこにあるか・どう表示するか)
	struct FieldInfo
	{
		const char* m_group;			// JSONの1段目のキー("HP" など)
		const char* m_key;				// JSONの2段目のキー("baseHP" など)
		const char* m_label;			// 表示名
		const char* m_help;				// マウスを乗せたときの説明
		const char* m_fallbackKey;		// 値が無いときに代わりに使う同じグループのキー(無ければnullptr)
		float       m_step;				// +/- ボタンで増える量
		bool        m_isInt;			// 整数で扱うか
	};

	//---- メンバ変数 ----

	//編集するキャラの一覧
	std::vector<CharaFile> m_charaList;

	//プレイヤーのセーブデータのパス
	std::string m_playerSaveFilePath;

	//ウィンドウの上に出すメッセージ
	std::string m_message;

	//保存した直後か
	bool m_savedFlg = false;

	//---- 基本の処理 ----

	// 1体分の編集画面
	// _chara ... 編集するキャラ
	void DrawChara(CharaFile& _chara);

	// 全キャラの値を並べた表(見るだけ)
	void DrawCompareTable();

	//---- 取得 ----

	// 編集する値の一覧(表示する順番)
	static const std::vector<FieldInfo>& GetFieldList();

	// 値を取り出す(無ければ m_fallbackKey の値、それも無ければ0)
	// _data ... キャラのJSON
	// _field ... 取り出す値
	float GetValue(const nlohmann::json& _data, const FieldInfo& _field) const;

	// 同じグループの、キーが _key の FieldInfo を探す(無ければnullptr)
	const FieldInfo* FindField(const char* _group, const char* _key) const;

	// 赤字のエラーが1件でもあるキャラがいるか
	bool HasError() const;

	//---- 設定 ----

	// 値を書き込む(整数の値は整数で書く)
	// _data ... キャラのJSON
	// _field ... 書き込む値
	// _value ... 書き込む数
	void SetValue(nlohmann::json& _data, const FieldInfo& _field, float _value);

	//---- その他の処理 ----

	// 1体分のファイルを読み込む
	//戻り値 ... 読み込めたらtrue
	bool LoadChara(CharaFile& _chara);

	// 1体分のファイルに保存する
	//戻り値 ... 保存できたらtrue
	bool SaveChara(CharaFile& _chara);

	// プレイヤーのセーブデータを、今の初期ステータスで上書きする
	// _chara ... プレイヤーのキャラ
	//戻り値 ... 書き込めたらtrue
	bool ResetPlayerSaveData(const CharaFile& _chara);

	// 値の決まりに合っているか。問題があればメッセージを返す(空ならOK)
	// _chara ... 調べるキャラ
	std::vector<std::string> Validate(const CharaFile& _chara) const;
};
