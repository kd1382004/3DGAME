#pragma once
#include"../../UIBase.h"
#include"../../../../Scene/SceneManager.h"

class PrepareScene;

//冒険の記録(RECORDS)の画面
//左に「最新のプレイ」と「最高階層ランキング」の枠、右に選んだ記録の詳細を出す
//座標はフレームワークと同じ(画面の真ん中が0,0・右と上がプラス)
class RecordsDisplay :public UIBase
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	RecordsDisplay() {};

	//デストラクタ
	~RecordsDisplay() override {};

	//---- 基本の処理 ----

	//初期化
	void Init()override;

	//更新
	void Update()override;

	//2Dの描画
	void DrawSprite()override;

	//---- 取得 ----

	//表示中かを返す
	//戻り値 ... 表示中ならtrue
	bool GetActive() const { return m_activeFlg; }

	//---- 設定 ----

	//準備シーンをセット
	// _spPrepareScene ... 準備シーン
	void SetPrepareScene(std::shared_ptr<PrepareScene> _spPrepareScene) { m_wpPrepareScene = _spPrepareScene; }

	//プレイ記録をセット(PrepareScene::Initから呼ばれる)
	//セットしたら、選んでいる記録を最初の枠に戻す
	// _gameRecord ... プレイ記録
	void SetGameRecord(const GameRecord& _gameRecord);

	//表示する(true)・しない(false)
	//PrepareScene::ChangeTabから呼ばれる
	// _flg ... 表示するか
	void SetActive(bool _flg) { m_activeFlg = _flg; }

private:

	//---- 型 ----

	//画面で使う画像
	enum class RecordsTex
	{
		//パネル・枠
		CardPanel,		// 記録1件分の枠の背景(360×110)
		DetailPanel,	// 記録の詳細の背景(330×558)
		SelectFrame,	// 選んでいる枠(金)
		HoverFrame,		// カーソルが乗っている枠(銀)
		CardDivider,	// 枠の中の横の点線
		CardDividerV,	// 枠の中の縦の点線
		DetailDivider,	// 詳細の中の横の点線

		//Records用の文字
		Title,			// 画面タイトル(RECORDS)
		LatestPlay,		// 最新のプレイ
		RankingLabel,	// 最高階層ランキング
		DetailTitle,	// 記録の詳細
		KillLabel,		// 撃破
		TreasureLabel,	// 宝箱
		TimeLabel,		// 時間
		GainLabel,		// 獲得
		CarryLabel,		// 持ち込み
		RankSuffix,		// 位
		ResultEscape,	// で脱出
		ResultLost,		// でロスト
		NumColon,		// 数字用の「:」
		NumSlash,		// 数字用の「/」
		MoneyLabel,		// 所持金(行の項目用。メニュー画面の見出しより小さい)

		//メニュー画面と同じ文字
		Floor,			// 階
		Escape,			// 脱出
		Lost,			// ロスト
		Body,			// 体
		Count,			// 個
		Gold,			// G
		Weapon,			// 武器
		Potion,			// ポーション

		TexSize
	};

	//---- メンバ変数 ----

	//準備シーン(循環参照を防ぐためweak_ptr)
	std::weak_ptr<PrepareScene> m_wpPrepareScene;

	//プレイ記録(最新1件 + 最高階層ベスト3)
	GameRecord m_gameRecord;

	//表示中かどうか(開いているタブの時だけtrue)
	bool m_activeFlg = false;

	//選んでいる枠の番号(0が最新のプレイ、1〜3がランキング。何も無ければ-1)
	int m_selectIndex = -1;

	//カーソルが乗っている枠の番号(乗っていなければ-1)
	int m_hoverIndex = -1;

	//画像
	std::shared_ptr<KdTexture> m_spTex[static_cast<int>(RecordsTex::TexSize)];

	//画像のパス(RecordsTexの順)
	const std::string m_texPath[static_cast<int>(RecordsTex::TexSize)] =
	{
		"Asset/Textures/GameUI/Prepare/Records/Panel/RecordCardPanel.png",
		"Asset/Textures/GameUI/Prepare/Records/Panel/RecordDetailPanel.png",
		"Asset/Textures/GameUI/Prepare/Records/Frame/RecordSelectFrame.png",
		"Asset/Textures/GameUI/Prepare/Records/Frame/RecordHoverFrame.png",
		"Asset/Textures/GameUI/Prepare/Records/Panel/CardDivider.png",
		"Asset/Textures/GameUI/Prepare/Records/Panel/CardDividerV.png",
		"Asset/Textures/GameUI/Prepare/Records/Panel/DetailDivider.png",

		"Asset/Textures/GameUI/Prepare/Records/Text/RecordsTitle.png",
		"Asset/Textures/GameUI/Prepare/Records/Text/LatestPlay.png",
		"Asset/Textures/GameUI/Prepare/Records/Text/RankingLabel.png",
		"Asset/Textures/GameUI/Prepare/Records/Text/DetailTitle.png",
		"Asset/Textures/GameUI/Prepare/Records/Text/KillLabel.png",
		"Asset/Textures/GameUI/Prepare/Records/Text/TreasureLabel.png",
		"Asset/Textures/GameUI/Prepare/Records/Text/TimeLabel.png",
		"Asset/Textures/GameUI/Prepare/Records/Text/GainLabel.png",
		"Asset/Textures/GameUI/Prepare/Records/Text/CarryLabel.png",
		"Asset/Textures/GameUI/Prepare/Records/Text/RankSuffix.png",
		"Asset/Textures/GameUI/Prepare/Records/Text/ResultEscape.png",
		"Asset/Textures/GameUI/Prepare/Records/Text/ResultLost.png",
		"Asset/Textures/GameUI/Prepare/Records/Text/NumColon.png",
		"Asset/Textures/GameUI/Prepare/Records/Text/NumSlash.png",
		"Asset/Textures/GameUI/Prepare/Records/Text/MoneyLabel.png",

		"Asset/Textures/GameUI/Prepare/Menu/Text/Floor.png",
		"Asset/Textures/GameUI/Prepare/Menu/Text/Escape.png",
		"Asset/Textures/GameUI/Prepare/Menu/Text/Lost.png",
		"Asset/Textures/GameUI/Prepare/Menu/Text/Body.png",
		"Asset/Textures/GameUI/Prepare/Menu/Text/Count.png",
		"Asset/Textures/GameUI/Prepare/Menu/Text/Gold.png",
		"Asset/Textures/GameUI/Prepare/Menu/Text/Weapon.png",
		"Asset/Textures/GameUI/Prepare/Menu/Text/Potion.png",
	};

	//武器アイコン(PlayerWeaponInfoの順)
	std::shared_ptr<KdTexture> m_spWeaponIconTex[PlayerWeaponInfoSize];

	//武器アイコンのパス(PlayerWeaponInfoの順。メニュー画面と同じ)
	const std::string m_weaponIconTexPath[PlayerWeaponInfoSize] =
	{
		"Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/Dagger/WeponStrengtheningIcon8.png",
		"Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/Sword/WeponStrengtheningIcon8.png",
	};

	//武器の名前(PlayerWeaponInfoの順)
	std::shared_ptr<KdTexture> m_spWeaponNameTex[PlayerWeaponInfoSize];

	//武器の名前のパス(PlayerWeaponInfoの順。メニュー画面と同じ)
	const std::string m_weaponNameTexPath[PlayerWeaponInfoSize] =
	{
		"Asset/Textures/GameUI/Prepare/Menu/Text/Dagger.png",
		"Asset/Textures/GameUI/Prepare/Menu/Text/Sword.png",
	};

	//---- レイアウト(特に書いていなければ、画像の真ん中の位置) ----

	//画面タイトル(RECORDS)の左端・上下中央
	const Math::Vector2 m_titlePos = { -89.0f, 289.0f };

	//「最新のプレイ」の左端・上下中央
	const Math::Vector2 m_latestLabelPos = { -88.0f, 228.0f };

	//「最高階層ランキング」の左端・上下中央
	const Math::Vector2 m_rankingLabelPos = { -88.0f, 56.0f };

	//最新のプレイの枠
	const Math::Vector2 m_latestCardPos = { 88.0f, 145.0f };

	//ランキング1位の枠
	const Math::Vector2 m_rankingCardPos = { 88.0f, -18.0f };

	//ランキングの枠同士の間隔(下へ)
	const float m_rankingSpace = 120.0f;

	//枠の横幅
	const float m_cardW = 360.0f;

	//枠の高さ
	const float m_cardH = 110.0f;

	//ランキングに出す数
	static const int m_rankingNum = 3;

	//記録の詳細パネル
	const Math::Vector2 m_detailPos = { 443.0f, -31.0f };

	//記録の詳細パネルの横幅
	const float m_detailW = 330.0f;

	//記録の詳細パネルの高さ
	const float m_detailH = 558.0f;

	//パネルの点線の内側に入れるための余白(枠)
	const float m_cardPad = 14.0f;

	//パネルの点線の内側に入れるための余白(詳細)
	const float m_detailPad = 22.0f;

	//枠の中の武器アイコンの大きさ
	const float m_cardIconSize = 36.0f;

	//詳細の中の武器アイコンの大きさ
	const float m_detailIconSize = 36.0f;

	//数字の大きさ(元画像9×14px → 18×28px。メニュー画面と同じ)
	const float m_numSize = 2.0f;

	//数字と単位のすき間
	const float m_numGap = 4.0f;

	//1行に並べるもの同士のすき間(日付と時刻の間)
	const float m_itemGap = 12.0f;

	//ランキングの「1位」と「8階」の間のすき間
	const float m_rankGap = 2.0f;

	//枠の2行目の並び(枠の点線の内側の左端からの距離)
	//武器アイコン | 撃破 | 宝箱 の3列。撃破・宝箱は3桁まで入る

	//1本目の縦の点線(武器と撃破の間)
	const float m_colDivider1X = 46.0f;

	//「撃破」の左端
	const float m_colKillLeft = 54.0f;

	//撃破の数字の右端
	const float m_colKillRight = 184.0f;

	//2本目の縦の点線(撃破と宝箱の間)
	const float m_colDivider2X = 193.0f;

	//「宝箱」の左端(数字の右端は枠の点線の内側の右端)
	const float m_colTreasureLeft = 202.0f;

	//数字の色
	const Math::Color m_numColor = { 1.0f, 1.0f, 1.0f, 1.0f };

	//日時の数字の色(少し暗くする)
	const Math::Color m_dateColor = { 0.75f, 0.68f, 0.58f, 1.0f };

	//左端(上下は真ん中)を基準に描く
	const Math::Vector2 m_pivotLeft = { 0.0f, 0.5f };

	//右端(上下は真ん中)を基準に描く
	const Math::Vector2 m_pivotRight = { 1.0f, 0.5f };

	//---- その他の処理 ----

	//マウスと当たってるかを返す
	// _pos ... 四角の真ん中
	// _w   ... 四角の横幅
	// _h   ... 四角の高さ
	//戻り値 ... 当たっていたらtrue
	bool MouseHit(Math::Vector2 _pos, float _w, float _h);

	//画像の読み込み
	void LoadTex();

	//画像を1枚読み込む(読み込み済みなら何もしない)
	// _spTex ... 読み込んだ画像を入れる先
	// _path  ... 画像のパス
	void LoadOneTex(std::shared_ptr<KdTexture>& _spTex, const std::string& _path);

	//選んでいる記録を最初の枠に戻す(最新があれば最新、無ければランキング1位)
	void ResetSelect();

	//枠の番号の記録を返す
	// _index ... 枠の番号(0が最新のプレイ、1〜3がランキング)
	//戻り値 ... 記録(その枠に記録が無ければnullptr)
	const GameResult* GetCardResult(int _index) const;

	//枠の真ん中の座標を返す
	// _index ... 枠の番号(0が最新のプレイ、1〜3がランキング)
	//戻り値 ... 座標
	Math::Vector2 GetCardPos(int _index) const;

	//記録の順位を返す
	// _index ... 枠の番号(0が最新のプレイ、1〜3がランキング)
	//戻り値 ... 順位(1〜3。ランキングに入っていなければ0)
	int GetRank(int _index) const;

	//画像の取得
	// _tex ... 画像の種類
	//戻り値 ... 画像(読み込めていなければnullptr)
	const std::shared_ptr<KdTexture>& GetTex(RecordsTex _tex) const { return m_spTex[static_cast<int>(_tex)]; }

	//画像の横幅を返す(読み込めていなければ0)
	// _spTex ... 画像
	//戻り値 ... 横幅
	float GetTexW(const std::shared_ptr<KdTexture>& _spTex) const;

	//数字を描いたときの横幅を返す
	// _num       ... 数字
	// _digit     ... 桁数(足りない分は0で埋める。-1ならそのまま)
	// _separator ... 3桁区切りにするか
	//戻り値 ... 横幅
	float GetNumW(long _num, int _digit = -1, bool _separator = false) const;

	//画像を描く
	// _spTex ... 描く画像
	// _pos   ... _pivotの点の位置
	// _pivot ... 基準点({0.5,0.5}で真ん中、{0,0.5}で左端)
	void DrawTex(const std::shared_ptr<KdTexture>& _spTex, const Math::Vector2& _pos,
		const Math::Vector2& _pivot = { 0.5f, 0.5f });

	//大きさを指定して描く(武器アイコン・「:」「/」用)
	// _spTex ... 描く画像
	// _pos   ... _pivotの点の位置
	// _w     ... 横幅
	// _h     ... 高さ
	// _pivot ... 基準点({0.5,0.5}で真ん中)
	void DrawTexSize(const std::shared_ptr<KdTexture>& _spTex, const Math::Vector2& _pos, float _w, float _h,
		const Math::Vector2& _pivot = { 0.5f, 0.5f });

	//画像を左から並べて描く
	// _spTex ... 描く画像
	// _x     ... 左端のx
	// _y     ... 上下中央のy
	//戻り値 ... 描いた画像の右端のx
	float DrawTexL(const std::shared_ptr<KdTexture>& _spTex, float _x, float _y);

	//数字を左から並べて描く
	// _num   ... 数字
	// _x     ... 左端のx
	// _y     ... 上下中央のy
	// _color ... 色
	// _digit ... 桁数(足りない分は0で埋める。-1ならそのまま)
	//戻り値 ... 描いた数字の右端のx
	float DrawNumL(long _num, float _x, float _y, const Math::Color& _color, int _digit = -1);

	//「:」や「/」を数字と同じ大きさで描く
	// _tex ... RecordsTex::NumColon か RecordsTex::NumSlash
	// _x   ... 左端のx
	// _y   ... 上下中央のy
	//戻り値 ... 描いた右端のx
	float DrawNumSymbol(RecordsTex _tex, float _x, float _y);

	//数字と単位を右揃えで描く(「48体」「320 G」など)
	// _num       ... 数字
	// _spUnit    ... 単位の画像(nullptrなら数字だけ)
	// _right     ... 右端のx
	// _y         ... 上下中央のy
	// _separator ... 3桁区切りにするか
	void DrawNumUnitR(long _num, const std::shared_ptr<KdTexture>& _spUnit, float _right, float _y, bool _separator = false);

	//プレイ時間を右揃えで描く(分:秒。「08:54」)
	// _timeSc ... プレイ時間(秒)
	// _right  ... 右端のx
	// _y      ... 上下中央のy
	void DrawTimeR(float _timeSc, float _right, float _y);

	//プレイした日時を左から描く(月/日 時:分。「11/20 21:59」)
	// _playDate ... プレイした日時(time_tの値)
	// _x        ... 左端のx
	// _y        ... 上下中央のy
	void DrawDateL(long long _playDate, float _x, float _y);

	//記録1件分の枠を描く
	// _index ... 枠の番号(0が最新のプレイ、1〜3がランキング)
	void DrawCard(int _index);

	//記録の詳細を描く(選んでいる記録)
	void DrawDetail();
};
