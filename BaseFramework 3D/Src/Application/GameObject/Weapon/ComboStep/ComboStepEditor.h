#pragma once
#include"ComboStep.h"
#include"ComboStepValidator.h"
#include"ComboPreviewObject.h"

class ComboStepEditor
{
public:
	// 毎フレーム呼ぶ（画面を描く）
	void Draw(bool* _open);

	ComboStepEditor();

	// プレビュー用モデル（シーンの m_objList に入れてもらう）
	std::shared_ptr<ComboPreviewObject> GetPreviewObject() const { return m_spPreviewObj; }

	// 保存に成功したときに呼ばれる（ゲームに読み直させるため）
	void SetOnSaved(const std::function<void(const std::string&)>& _func) { m_onSaved = _func; }

	// ゲームのカメラ（プレビューを描いた後、元に戻すため）
	void SetPreviewMainCamera(const std::shared_ptr<KdGameObject>& _spCamera) { m_spPreviewObj->SetMainCamera(_spCamera); }

private:
	//selectID
	int m_selected = -1;

	//==============================
	// ファイル（開く・名前を付けて保存・新規）
	//==============================

	// コンボファイルを置く一番上のフォルダー（この下に 持ち主/武器.json）
	const std::string kComboRoot = "Asset/Data/ObjeData/Combo";

	// _path のファイルを開く（成功したら編集中のデータを入れ替える）
	bool LoadFromFile(const std::string& _path);

	// _path に保存する（成功したら m_filePath も _path にする）
	bool SaveToFile(const std::string& _path);

	// kComboRoot の下の .json を集める（「開く」の一覧用）
	void RefreshFileList();

	// 「開く」「名前を付けて保存」「新規」のポップアップ
	void DrawFilePopups();

	std::vector<std::string> m_fileList;          // kComboRoot からの相対パス（"Player/Sword.json"）
	int  m_fileListSelected = -1;
	bool m_requestOpenPopup = false;              // 次のフレームでポップアップを開く
	bool m_requestSaveAsPopup = false;
	bool m_requestNewPopup = false;
	std::string m_saveAsOwner = "Player";         // 名前を付けて保存：持ち主
	std::string m_saveAsWeapon = "Sword";         // 名前を付けて保存：武器名

	std::function<void(const std::string&)> m_onSaved;

	//==============================
	// 元に戻す・やり直す
	//==============================

	// 変更する前に呼ぶ（今の m_steps を履歴に積む）。ボタンやメニューの操作用
	void PushUndo() { PushUndoState(m_steps); }
	void PushUndoState(const std::vector<ComboStep>& _state);

	// 詳細の部品で値を変えたときに呼ぶ（未保存の印＋操作が終わったら履歴に積む）
	void MarkEdited() { m_isDirty = true; m_editChanged = true; }

	static constexpr size_t kUndoMax = 100;   // 履歴の上限
	std::vector<ComboStep> m_stableSteps;     // 何も操作していないときの m_steps（ドラッグ前の状態）
	bool m_editChanged = false;               // 操作中に値が変わったか

	//==============================
	// 保存・閉じる前の確認
	//==============================

	// 検査のエラーがあれば確認してから保存する
	void RequestSave(const std::string& _path);

	// 検査のエラーの数
	int CountErrors() const
	{
		int n = 0;
		for (const ComboStepIssue& issue : m_issues) { if (issue.isError) { ++n; } }
		return n;
	}

	std::string m_pendingSavePath;            // エラー確認のあとに保存するパス
	bool m_requestSaveErrorPopup = false;
	bool m_requestClosePopup = false;
	bool m_closeNow = false;                  // 次の End の前にウィンドウを閉じる

	//==============================
	// プレビューの再生（Space キー）
	//==============================
	void TogglePreviewPlay();


	//==============================
	// メニューから呼ぶ処理
	//==============================

	// ファイル
	void NewFile();
	void OpenFile();
	void Save();
	void SaveAs();

	// 編集
	void Undo();
	void Redo();
	void RenumberComboIndex();

	// 一覧の操作

	 // 末尾に追加
	void AddStep();          

	// 選んだ段を複製して、すぐ下に入れる
	void DuplicateStep();       

	// 選んだ段を消す
	void DeleteStep();           

	// 選んだ段を上(-1)／下(+1)に動かす
	void MoveStep(int _dir);     

	//==============================
	// データ
	//==============================

	//仮データ作成
	void CreateDummyData();


	// 編集中のコンボ
	std::vector<ComboStep> m_steps;

	// 開いているファイルのパス（空なら新規）
	std::string m_filePath;

	// 未保存の変更があるか
	bool m_isDirty = false;

	//==============================
	// 元に戻す／やり直す
	//==============================

	std::vector<std::vector<ComboStep>> m_undoStack;
	std::vector<std::vector<ComboStep>> m_redoStack;

	//==============================
	// 表示の切り替え（「表示」メニュー）
	//==============================

	bool m_showPreview = true;
	bool m_showValidation = true;


	//問題を保存
	std::vector<ComboStepIssue> m_issues;


	// 動作確認用の固定パス（後で「開く」「名前を付けて保存」で選べるようにする）
	const std::string kTestPath = "Asset/Data/ObjeData/Combo/Player/Sword.json";

	//モデルのパス
	std::string m_CharaModelPath;

	//武器モデルのパス（プレビューで手に持たせる）
	std::string m_WeaponModelPath;

	// m_WeaponModelPath の武器をプレビューに持たせる
	void LoadPreviewWeapon();

	//==============================
	// プレビュー用モデルとアニメーション一覧（第5章）
	//==============================

	// アニメーション1つ分の情報
	struct AnimeInfo
	{
		std::string name;
		float       length = 0.0f;   // KdAnimationData::m_maxLength と同じ単位
	};

	// m_CharaModelPath のモデルを読み込んで、アニメーション一覧を作り直す
	void LoadPreviewModel();

	// 詳細の「アニメーション名」を描く（一覧があればコンボボックス、なければ InputText）
	void DrawAnimeNameSelect(ComboStep& _step);

	std::shared_ptr<KdModelData>    m_spPreviewModel = nullptr;
	std::vector<AnimeInfo>          m_animeList;    // 表示用（名前順）
	std::unordered_set<std::string> m_animeNames;   // 検査用（名前だけ）

	//==============================
	// 連続再生プレビュー（第6章）
	//==============================

	// 1秒で進むフレーム数（PlayerBase::Update の AdvanceTime(…, 100 * m_animeSpeedRate) と同じ）
	static constexpr float kAnimeFramePerSec = 100.0f;

	// 入力の真似方
	enum PreviewInputMode
	{
		PreviewInput_Fastest,   // 自動（受付が始まった瞬間に予約）
		PreviewInput_Latest,    // 自動（切替点の直前に予約）
		PreviewInput_Manual,    // 手動（Space キー）
	};

	struct PreviewState
	{
		bool  playing = false;   // 再生中か
		int   stepIdx = -1;      // 再生中の段（m_steps の添字）。-1 = 止まっている
		float time = 0.0f;       // 経過フレーム（KdAnimator::m_time と同じ単位）
		bool  reserved = false;  // 次の段の予約があるか
		float hitStopTimer = 0.0f;   // ヒットストップの残り（秒）
		bool  hitStopDone = false;   // この段のヒットストップを出したか
		std::string lastEvent;       // 最後に起きたこと（画面に出す）
	};

	// プレビューの画面（下の領域の中身）
	void DrawPreview();

	// 毎フレーム進める（ゲームの PlayerBase::ComboUpdate と同じルール）
	void UpdatePreview(float _dt);

	// 指定した段から再生を始める
	void StartPreviewStep(int _stepIdx);

	// 全段を横に並べたタイムラインを描く
	void DrawComboTimeline();

	// アニメーションの長さ（フレーム）。見つからなければ 0
	float GetAnimeLength(const std::string& _name) const;

	PreviewState m_preview;
	int   m_previewInputMode = PreviewInput_Fastest;
	float m_previewPlaySpeed = 1.0f;     // プレビューだけの倍速（スロー確認用）
	bool  m_previewLoop = false;         // 最後まで行ったら1段目から
	bool  m_previewHitStop = true;       // 当たり判定の開始でヒットストップを入れるか
	bool  m_previewKeyPushed = false;    // 手動モードで、このフレームに Space が押されたか

	// モデルの表示（第6章 6-5）
	void UpdatePreviewObject();          // 今の再生位置の姿勢をモデルに入れる
	void DrawPreviewWindow();            // モデルを映す専用ウィンドウ

	std::shared_ptr<ComboPreviewObject> m_spPreviewObj = std::make_shared<ComboPreviewObject>();
	bool m_showPreviewModel = true;      // 「モデル」ウィンドウを出すか

	//==============================
	// ウィンドウの固定（「表示」メニュー）
	//==============================

	// 固定中のフラグ（動かせない・大きさを変えられない・たためない）
	ImGuiWindowFlags GetLockFlags() const
	{
		return m_lockWindows ? (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse) : 0;
	}

	bool m_lockWindows = false;   // 2つのウィンドウを今の位置で固定するか
	bool m_resetLayout = false;   // 次のフレームで初期配置に並べ直すか
};