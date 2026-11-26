#pragma once

class PauseUI
{
public:

	~PauseUI() {};

	void Update();
	void DrawSprite();




	bool GetPauseMenuFlg() { return m_pauseMenuFlg; }
private:
	void Init();

	bool m_pauseMenuFlg = false;

	bool m_escFlg = false;





	//---- 型 ----

	//今どの画面を出しているか
	enum class PausePage
	{
		Main,			//メインメニュー
		Volume,			//音量設定
		ConfirmTitle,	//「タイトルへ戻りますか？」
		ConfirmExit,	//「ゲームを終了しますか？」
	};

	//---- メンバ変数 ----

	//今の画面
	PausePage m_pausePage = PausePage::Main;

	//マウスが乗っているか
	bool m_resumeButtonHoverFlg = false;
	bool m_volumeButtonHoverFlg = false;
	bool m_titleButtonHoverFlg = false;
	bool m_exitButtonHoverFlg = false;

	//---------- 関数 ----------

	//--- メインメニュー ---
	void MainMenuUpdate();
	void MainMenuDraw();

	//-----------------------

	//--- VOL ---
	void VolumePageUpdate();
	void VolumePageDraw();

	//「戻る」ボタンの更新
	void BackButtonUpdate();

	//つまみのドラッグの更新
	void SliderDragUpdate();

	//バーの上にマウスが乗っているか
	// _sliderPos ... バーの位置(中心)
	//戻り値 ... 乗っているならtrue
	bool IsMouseOnSlider(const Math::Vector2& _sliderPos);

	//つまみの上にマウスが乗っているか
	// _sliderPos ... バーの位置(中心)
	// _vol ... 今の音量(0〜1)
	//戻り値 ... 乗っているならtrue
	bool IsMouseOnThumb(const Math::Vector2& _sliderPos, float _vol);

	//マウスのxから音量を計算
	// _sliderPos ... バーの位置(中心)
	//戻り値 ... 音量(0〜1)
	float MouseXToVol(const Math::Vector2& _sliderPos);

	//音量をセット(VOLInfoへ)
	// _index ... 0:全体 1:BGM 2:効果音
	// _vol ... 音量(0〜1)
	void SetVolume(int _index, float _vol);

	//音量を取得(VOLInfoから)
	// _index ... 0:全体 1:BGM 2:効果音
	//戻り値 ... 音量(0〜1)
	float GetVolume(int _index);



	//ドラッグしているバー
	enum DragSlider
	{
		DragSlider_Master = 0,	//全体
		DragSlider_BGM,			//BGM
		DragSlider_SE,			//効果音
		DragSlider_Siz,	

		DragSlider_None,	//ドラッグしていない
	};

	//バーの位置を取得
	// _slider ... どのバーか
	//戻り値 ... バーの位置(中心)
	Math::Vector2 GetSliderPos(DragSlider _slider);

	//ドラッグしているバーの番号
	DragSlider m_dragIndex = DragSlider_None;

	//つまみにマウスが乗っているか(0:全体 1:BGM 2:効果音)
	bool m_thumbHoverFlg[DragSlider_Siz] = {};

	//「戻る」にマウスが乗っているか
	bool m_backButtonHoverFlg = false;

	//-----------------------------------------


	void ConfirmDraw();
	void ConfirmUpdate();
	//「はい」「いいえ」にマウスが乗っているか
	bool m_yesButtonHoverFlg = false;
	bool m_noButtonHoverFlg = false;





	//画像の上にマウスが乗っているか
	// _spTex ... 画像
	// _pos ... 画像の位置(中心)
	//戻り値 ... 乗っているならtrue
	bool IsMouseOnTex(const std::shared_ptr<KdTexture>& _spTex, const Math::Vector2& _pos);

	//メニューを閉じる(ESC・「ゲームに戻る」の両方で使う)
	void PauseClose();


	//-------------------　画像　-------------------

	void TexLoad();

	//---- パネル ----

	//メインメニューのパネル
	std::shared_ptr<KdTexture> m_spMainPanelTex;
	//音量設定のパネル
	std::shared_ptr<KdTexture> m_spVolumePanelTex;
	//確認ダイアログのパネル
	std::shared_ptr<KdTexture> m_spConfirmPanelTex;

	//---- ボタン ----

	//「ゲームに戻る」(通常・マウスを乗せたとき)
	std::shared_ptr<KdTexture> m_spResumeButtonTex;
	std::shared_ptr<KdTexture> m_spResumeButtonHoverTex;
	//「音量設定」(通常・マウスを乗せたとき)
	std::shared_ptr<KdTexture> m_spVolumeButtonTex;
	std::shared_ptr<KdTexture> m_spVolumeButtonHoverTex;
	//「タイトルへ」(通常・マウスを乗せたとき)
	std::shared_ptr<KdTexture> m_spTitleButtonTex;
	std::shared_ptr<KdTexture> m_spTitleButtonHoverTex;
	//「ゲーム終了」(通常・マウスを乗せたとき)
	std::shared_ptr<KdTexture> m_spExitButtonTex;
	std::shared_ptr<KdTexture> m_spExitButtonHoverTex;
	//「戻る」(通常・マウスを乗せたとき)
	std::shared_ptr<KdTexture> m_spBackButtonTex;
	std::shared_ptr<KdTexture> m_spBackButtonHoverTex;
	//「はい」(通常・マウスを乗せたとき)
	std::shared_ptr<KdTexture> m_spYesButtonTex;
	std::shared_ptr<KdTexture> m_spYesButtonHoverTex;
	//「いいえ」(通常・マウスを乗せたとき)
	std::shared_ptr<KdTexture> m_spNoButtonTex;
	std::shared_ptr<KdTexture> m_spNoButtonHoverTex;

	//---- 文字 ----

	//「一時停止」
	std::shared_ptr<KdTexture> m_spPauseTitleTex;
	//「音量設定」
	std::shared_ptr<KdTexture> m_spVolumeTitleTex;
	//「ESCで閉じる」
	std::shared_ptr<KdTexture> m_spEscCloseTex;

	//「ESCで戻る」
	std::shared_ptr<KdTexture> m_spEscBackTex;

	//「タイトルへ戻りますか？」
	std::shared_ptr<KdTexture> m_spConfirmTitleTitleTex;
	//「ゲームを終了しますか？」
	std::shared_ptr<KdTexture> m_spConfirmTitleExitTex;
	//「今回の探索の進み具合は失われます」
	std::shared_ptr<KdTexture> m_spConfirmMsgTitleTex;
	//「今回の探索の進み具合は保存されません」
	std::shared_ptr<KdTexture> m_spConfirmMsgExitTex;
	//「全体」
	std::shared_ptr<KdTexture> m_spMasterTex;
	//「BGM」
	std::shared_ptr<KdTexture> m_spBGMTex;
	//「効果音」
	std::shared_ptr<KdTexture> m_spSETex;
	//「%」
	std::shared_ptr<KdTexture> m_spPercentTex;


	std::shared_ptr<KdTexture> m_spBlackTex;

	//---- スライダー ----

	//バーの枠
	std::shared_ptr<KdTexture> m_spSliderTrackTex;
	//バーの中身(音量の分だけ左から切り取って描く)
	std::shared_ptr<KdTexture> m_spSliderFillTex;
	//つまみ(通常・マウスを乗せたとき)
	std::shared_ptr<KdTexture> m_spSliderThumbTex;
	std::shared_ptr<KdTexture> m_spSliderThumbHoverTex;

	//---- 位置----

	void SetTexPos();

	//パネル
	Math::Vector2 m_mainPanelPos;
	Math::Vector2 m_volumePanelPos;
	Math::Vector2 m_confirmPanelPos;

	//ボタン(通常・マウスを乗せたときは同じ位置)
	Math::Vector2 m_resumeButtonPos;
	Math::Vector2 m_volumeButtonPos;
	Math::Vector2 m_titleButtonPos;
	Math::Vector2 m_exitButtonPos;
	Math::Vector2 m_backButtonPos;
	Math::Vector2 m_yesButtonPos;
	Math::Vector2 m_noButtonPos;

	//文字
	Math::Vector2 m_pauseTitlePos;
	Math::Vector2 m_volumeTitlePos;
	Math::Vector2 m_escClosePos;			//メインメニューの「ESCで閉じる」
	Math::Vector2 m_volumeEscClosePos;		//音量設定の「ESCで閉じる」
	Math::Vector2 m_confirmTitlePos;		//質問(タイトルへ・ゲーム終了で同じ位置)
	Math::Vector2 m_confirmMsgPos;			//説明(タイトルへ・ゲーム終了で同じ位置)
	Math::Vector2 m_masterPos;				//「全体」(左端)
	Math::Vector2 m_BGMPos;					//「BGM」(左端)
	Math::Vector2 m_SEPos;					//「効果音」(左端)
	Math::Vector2 m_masterPercentPos;		//「%」(左端)
	Math::Vector2 m_BGMPercentPos;
	Math::Vector2 m_SEPercentPos;

	//スライダー(バーの中心)
	Math::Vector2 m_masterSliderPos;
	Math::Vector2 m_BGMSliderPos;
	Math::Vector2 m_SESliderPos;

public:
	// シングルトンパターン
	// 常に存在する && 必ず1つしか存在しない(1つしか存在出来ない)
	// どこからでもアクセスが可能で便利だが
	// 何でもかんでもシングルトンという思考はNG
	static PauseUI& Instance()
	{
		static PauseUI instance;
		return instance;
	}

private:
	PauseUI() { Init(); };
};
