#pragma once

//マウス入力
class MouseInfo
{
public:

	//---- 型 ----

	//マウスの状態
	enum MouseType
	{
		//何にもあったってない(デフォルト)
		MouseType_Normal,	

		//何かに当たってる(Clickで何かがおこる)
		MouseType_HIT,	

		//クリックされてる
		MouseType_Click,

		//マウスがフリーじゃない(例:カメラを動かすために固定している)
		MouseType_NotFree,

		//何かを掴んで動かしている(例:スクロールバーのつまみをドラッグ中)
		MouseType_Drag,

		//何かに当たっているが今は使えない(例:前提ノード未解放・素材不足)
		MouseType_Disabled,
	};

	//---- コンストラクタ・デストラクタ ----

	//デストラクタ
	~MouseInfo();

	//---- 基本の処理 ----

	//マウスの状態を更新する(毎フレーム呼ぶ)
	void Update();

	//マウスカーソルを描く
	void DrawSprite();

	//---- 取得 ----

	//マウスがゲーム内で自由に動かせる状態か
	//戻り値 ... 自由に動かせるならtrue
	bool GetMouseFreeFlg() { return m_mouseFreeFlg; }

	//カメラ回転用のマウスの移動量を取得
	//戻り値 ... マウスの移動量
	POINT GetFixMousePos() const { return m_fixMousePos; }

	//ウィンドウマウスがフォーカスしているか
	bool GetFocusWindowFlg() const { return m_focusWindowFlg; }

	//ウィンドウ上でのマウスの中央
	POINT GetWindowMousePos();

	//マウスホイール
	int GetMouseWheelValue()const;

	//---- 設定 ----

	//マウスを自由に動かせる状態にするかをセット
	// _free ... 自由に動かせるならtrue(falseで中央に固定)
	void SetMouseFreeFlg(bool _free);


	//マウスの状態セッター
	// _type ... マウスの状態
	void SetMouseType(MouseType _type);

	//マウスをウィンドウ上の中央にセット
	void SetMousePosFixMousePos();

	//---- その他の処理 ----

	//クリックの音を鳴らす
	void SEMouseClickPlay();

	//---- シングルトン ----

	// シングルトンパターン
	// 常に存在する && 必ず1つしか存在しない(1つしか存在出来ない)
	// どこからでもアクセスが可能で便利だが
	// 何でもかんでもシングルトンという思考はNG
	static MouseInfo& Instance()
	{
		static MouseInfo instance;
		return instance;
	}

	//---- メンバ変数 ----

	//マウスのウィンドウ上座標
	POINT m_windowPos = {};

private:

	//マウスをゲーム内においてフリーにするか
	bool m_mouseFreeFlg = true;

	// カメラ回転用マウス座標の差分
	POINT m_fixMousePos{};

	//マウスの状態管理
	MouseType m_mouseType = MouseType::MouseType_Normal;

	//前のフレームのマウスの状態
	MouseType m_mouseOldType = MouseType::MouseType_Normal;

	//ウィンドウ単位でのフォーカスがあるかどうか
	bool m_focusWindowFlg = false;

	//マウスカーソルの画像
	std::shared_ptr<KdTexture> m_spMousePointer;

	//音
	std::shared_ptr<KdSoundInstance> m_spClickSE;

	//何かに当たったときの音
	std::shared_ptr<KdSoundInstance> m_spHitSE;

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ(シングルトンなので外から作れない)
	MouseInfo() { Init(); };

	//---- 基本の処理 ----

	//初期化(画像・音の読み込み)
	void Init();
};

