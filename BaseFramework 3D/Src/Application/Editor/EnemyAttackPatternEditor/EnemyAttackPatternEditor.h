#pragma once
//////////////////////////////////////////////////////////////
// 敵の攻撃パターンを決める ImGui ウィンドウ
// Asset/Data/ObjeData/EnemyAttackPattern/<敵の名前>.json を編集する
// (パターン = 単発アニメを並べたコンボファイル + 使う条件 + 段ごとの特別な設定)
//////////////////////////////////////////////////////////////
class EnemyAttackPatternEditor
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ(フォルダの中の敵の一覧を読み、先頭の敵を読み込む)
	EnemyAttackPatternEditor();

	//---- 基本の処理 ----

	// 毎フレーム ImGui のフレーム内で呼ぶ
	// _open ... ウィンドウの×ボタンで閉じたときにfalseを入れるフラグ(nullptrなら×ボタンなし)
	void Draw(bool* _open = nullptr);

	//---- その他の処理 ----

	//今の敵のJSONファイルを読み込む
	//戻り値 ... 読み込めたらtrue
	bool Load();

	//今の敵のJSONファイルに保存する(エラーがあるときは保存しない)
	//戻り値 ... 保存できたらtrue
	bool Save();

	// 保存した直後のフレームだけtrue(敵側でパターンを読み直す合図)
	bool PopSaved() { bool saved = m_savedFlg; m_savedFlg = false; return saved; }

	//---- 取得 ----

	//今編集している敵の名前
	const std::string& GetNowEnemyName() const { return m_nowEnemyName; }

private:

	//---- 型 ----

	// コンボファイルの段1つ分のうち、エディターで見せるもの
	struct StepInfo
	{
		std::string m_animeName;		// アニメ名
		float       m_hitStart = 0.0f;	// 当たり判定の開始(進行度)
		float       m_hitEnd = 0.0f;		// 当たり判定の終了(進行度)
		float       m_cancelPoint = 0.0f;	// 次の段に移る進行度
	};

	//---- メンバ変数 ----

	//パターン一覧ファイルを置くフォルダ
	const std::string m_dirPath = "Asset/Data/ObjeData/EnemyAttackPattern/";

	//コンボファイルを置くフォルダ
	const std::string m_comboDirPath = "Asset/Data/ObjeData/Combo/";

	//フォルダにある敵の名前(ファイル名から.jsonを除いたもの)
	std::vector<std::string> m_enemyNames;

	//今編集している敵の名前
	std::string m_nowEnemyName;

	//選べるコンボファイルのパス
	std::vector<std::string> m_comboFilePaths;

	//コンボファイルの段の中身(パス → 段の一覧)。毎フレーム読まないように覚えておく
	std::unordered_map<std::string, std::vector<StepInfo>> m_stepCache;

	//編集中のJSONの中身
	nlohmann::json m_data;

	//未保存の変更があるか
	bool m_dirty = false;

	//保存した直後か
	bool m_savedFlg = false;

	//画面に出すメッセージ
	std::string m_message;

	//選んでいるパターンの番号(-1なら選んでいない)
	int m_selectPatternIndex = -1;

	//新しく作る敵の名前の入力欄
	char m_newEnemyName[64] = "";

	//「この距離ならどれが出るか」を確かめる距離
	float m_testDistance = 3.0f;

	//「このLVならどれが出るか」を確かめる敵のLV
	int m_testLV = 1;

	//---- 基本の処理 ----

	//上の段(敵の切り替え・新規作成・読み込み・保存)
	void DrawFileBar();

	//左側のパターン一覧(追加・複製・削除・並べ替え)
	void DrawPatternList();

	//右側の選んでいるパターンの中身
	// _pattern ... 編集するパターン
	void DrawPatternDetail(nlohmann::json& _pattern);

	//段1つ分の当たり判定の骨と特別な設定
	// _pattern ... 編集するパターン
	// _stepIndex ... 何段目か
	// _steps ... コンボファイルの段の一覧
	void DrawStep(nlohmann::json& _pattern, int _stepIndex, const std::vector<StepInfo>& _steps);

	//段1つ分のイベント(エフェクト・音・画面の揺れ)の一覧
	// _extra ... 段の特別な設定
	void DrawEvents(nlohmann::json& _extra);

	//「この距離とLVならどのパターンがどれくらいの確率で出るか」
	void DrawDistanceTest();

	//---- 取得 ----

	//パターンの一覧(なければ空の配列を作って返す)
	nlohmann::json& GetPatterns();

	//コンボファイルの段の一覧(1回読んだら覚えておく。読めなければ空)
	// _path ... コンボファイルのパス
	const std::vector<StepInfo>& GetSteps(const std::string& _path);

	//---- その他の処理 ----

	//フォルダにある敵の一覧を読み直す
	void RefreshEnemyList();

	//選べるコンボファイルの一覧を読み直す
	void RefreshComboFileList();

	//当たり判定の骨と特別な設定の数を、段の数に合わせる
	// _pattern ... 合わせるパターン
	// _stepNum ... 段の数
	void SyncStepArrays(nlohmann::json& _pattern, int _stepNum);

	//新しいパターンの初期値
	//戻り値 ... パターン1つ分のjson
	static nlohmann::json MakeDefaultPattern();

	//段の特別な設定の初期値(普通の単発と同じ動き)
	//戻り値 ... 段1つ分の特別な設定のjson
	static nlohmann::json MakeDefaultExtra();

	//決まりに合っているか。問題があればメッセージを返す(空ならOK)
	std::vector<std::string> Validate();
};
