#pragma once

//============================================================
// アプリケーションクラス
//	APP.～ でどこからでもアクセス可能
//============================================================
class Application
{
public:

	//---- 取得 ----

	//ウィンドウハンドルを取得
	HWND GetWindowHandle()		const	{ return m_window.GetWndHandle(); }
	//マウスホイールの回転量を取得
	int GetMouseWheelValue()	const	{ return m_window.GetMouseWheelVal(); }
	//今のFPSを取得
	int		GetNowFPS()			const	{ return m_fpsController.m_nowfps; }
	//FPSの上限を取得
	int		GetMaxFPS()			const	{ return m_fpsController.m_maxFps; }
	//1フレームの経過時間(秒)を取得
	float	GetDeltaTime()		const	{ return m_fpsController.GetDeltaTime(); }

	//---- その他の処理 ----

	// アプリケーション実行
	void Execute();

	// アプリケーション終了
	void End()							{ m_endFlag = true; }

	//---- シングルトン ----

	//インスタンスを取得する(シングルトン)
	//戻り値 ... Applicationのインスタンス
	static Application &Instance(){
		static Application Instance;
		return Instance;
	}

private:

	//---- メンバ変数 ----

	// ゲームウィンドウクラス
	KdWindow		m_window;

	// FPSコントローラー
	KdFPSController	m_fpsController;

	// ゲーム終了フラグ trueで終了する
	bool		m_endFlag = false;

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ(シングルトンなので外から作れない)
	Application() {}

	//---- 基本の処理 ----

	//更新前の処理
	void PreUpdate();

	//更新
	void Update();

	//更新後の処理
	void PostUpdate();

	//描画前の処理
	void PreDraw();

	//3Dの描画
	void Draw();

	//2Dの描画
	void DrawSprite();

	// アプリケーション初期化
	bool Init(int w, int h);

	// アプリケーション解放
	void Release();

	//---- その他の処理 ----

	//フレームワークの更新開始(入力・環境の更新)
	void KdBeginUpdate();

	//フレームワークの更新後処理(3Dサウンドのリスナー・オーディオの更新)
	void KdPostUpdate();

	//フレームワークの描画開始(画面クリア・ポストプロセスの準備)
	// usePostProcess ... ポストプロセスを使うならtrue
	void KdBeginDraw(bool usePostProcess = true);

	//描画の後処理(ポストエフェクト・デバッグ描画)
	void PostDraw();

	//フレームワークの描画終了(ImGuiの描画・画面に表示)
	void KdPostDraw();
};
