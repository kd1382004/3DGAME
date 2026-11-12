#pragma once
#include"ComboStep.h"
#include"ComboStepValidator.h"

class ComboStepEditor
{
public:
	// 毎フレーム呼ぶ（画面を描く）
	void Draw(bool* _open);

	ComboStepEditor();

private:
	//selectID
	int m_selected = -1;;


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
};