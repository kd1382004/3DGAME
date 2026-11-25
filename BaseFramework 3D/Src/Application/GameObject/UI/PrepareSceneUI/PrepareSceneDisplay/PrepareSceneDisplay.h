#pragma once
#include"../../UIBase.h"
#include"../../../../Scene/SceneManager.h"

class PrepareScene;

enum PrepareMenu;

//準備シーンの画面管理
class PrepareSceneDisplay :public UIBase
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	PrepareSceneDisplay() {};

	//デストラクタ
	~PrepareSceneDisplay() override {};

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
	// _gameRecord ... プレイ記録
	void SetGameRecord(const GameRecord& _gameRecord) { m_gameRecord = _gameRecord; }

	//特有の情報を表示する(true)・しない(false)
	//PrepareScene::ChangeTabから呼ばれる
	// _flg ... 表示するか
	void SetActive(bool _flg) { m_activeFlg = _flg; }

	//Tab選択を表示するかどうか
	void SetTabActive(bool _flg) { m_TabActiveFlg = _flg; }

	//持っているポーションの合計をセット
	// _num ... ポーションの合計(0未満は0にする)
	void SetPotionNum(int _num) { m_potionNum = std::max(0, _num); }

	//所持金をセット
	// _money ... 所持金
	void SetMoney(long _money);



private:

	//---- 型 ----

	//メニューの種類(上から並ぶ順)
	enum class PrepareMenu
	{
		Explore,	// 本編(EXPLORE)
		Equip,		// 装備(EQUIP)
		Shop,		// ショップ(SHOP)
		Records,	// Play履歴(RECORDS)
		MenuSize
	};

	//情報パネル(右側)の背景の種類
	enum class InfoPanel
	{
		NowEquip,	// 今の装備
		Money,		// 所持金
		LastRecord,	// 前回の記録
		PanelSize
	};

	//文字の種類
	enum class InfoText
	{
		Title,			// 画面タイトル(EXPLORE)
		NowEquip,		// 今の装備
		Money,			// 所持金
		LastRecord,		// 前回の記録
		Weapon,			// 武器
		Potion,			// ポーション
		Sword,			// 剣
		Dagger,			// ダガー
		Escape,			// 脱出
		Lost,			// ロスト
		Best,			// 最高
		Floor,			// 階
		Count,			// 個
		Gold,			// G
		Reached,		// 到達階
		ResultLabel,	// 結果
		TextSize
	};

	//---- メンバ変数 ----

	//表示中かどうか(開いているタブの時だけtrue)
	bool m_activeFlg = false;


	bool m_TabActiveFlg = false;

	//準備シーン(循環参照を防ぐためweak_ptr)
	std::weak_ptr<PrepareScene> m_wpPrepareScene;

	//プレイ記録(最新1件 + 最高階層ベスト3)
	GameRecord m_gameRecord;

	//メニューボタンの画像
	std::shared_ptr<KdTexture> m_spMenuTex[static_cast<int>(PrepareMenu::MenuSize)];

	//メニューボタンの選ばれてないとき画像
	std::shared_ptr<KdTexture> m_spNotMenuTex[static_cast<int>(PrepareMenu::MenuSize)];

	//メニューボタンの位置
	Math::Vector2 m_menuPos[static_cast<int>(PrepareMenu::MenuSize)];

	//メニューボタンの大きさ(画像)
	Math::Vector2 m_menuSize[static_cast<int>(PrepareMenu::MenuSize)];

	//メニューボタンの描く大きさ
	Math::Vector2 m_menuDrawSize[static_cast<int>(PrepareMenu::MenuSize)];

	//マウスが乗っているか
	bool m_mouseHitFlg[static_cast<int>(PrepareMenu::MenuSize)];

	//メニューボタンの画像のパス(PrepareMenuの順)
	const std::string m_menuTexPath[static_cast<int>(PrepareMenu::MenuSize)] =
	{
		"Asset/Textures/Button/PrepareMenu/ExploreButton.png",
		"Asset/Textures/Button/PrepareMenu/EquipButton.png",
		"Asset/Textures/Button/PrepareMenu/ShopButton.png",
		"Asset/Textures/Button/PrepareMenu/RecordsButton.png",
	};
	//メニューボタンの選ばれてない画像のパス(PrepareMenuの順)
	const std::string m_menuNotTexPath[static_cast<int>(PrepareMenu::MenuSize)] =
	{
		"Asset/Textures/Button/PrepareMenu/ExploreButton_Unselected.png",
		"Asset/Textures/Button/PrepareMenu/EquipButton_Unselected.png",
		"Asset/Textures/Button/PrepareMenu/ShopButton_Unselected.png",
		"Asset/Textures/Button/PrepareMenu/RecordsButton_Unselected.png",
	};

	//パネルの画像
	std::shared_ptr<KdTexture> m_spPanelTex[static_cast<int>(InfoPanel::PanelSize)];

	//パネル画像のパス(InfoPanelの順)
	const std::string m_panelTexPath[static_cast<int>(InfoPanel::PanelSize)] =
	{
		"Asset/Textures/GameUI/Prepare/Menu/Panel/NowEquipPanel.png",
		"Asset/Textures/GameUI/Prepare/Menu/Panel/MoneyPanel.png",
		"Asset/Textures/GameUI/Prepare/Menu/Panel/LastRecordPanel.png",
	};

	//文字の画像
	std::shared_ptr<KdTexture> m_spTextTex[static_cast<int>(InfoText::TextSize)];

	//文字の位置
	Math::Vector2 m_textPos[static_cast<int>(InfoText::TextSize)];

	//パネルの位置
	Math::Vector2 m_panelPos[static_cast<int>(InfoPanel::PanelSize)];

	//文字画像のパス(InfoTextの順)
	const std::string m_textTexPath[static_cast<int>(InfoText::TextSize)] =
	{
		"Asset/Textures/GameUI/Prepare/Menu/Text/Title.png",
		"Asset/Textures/GameUI/Prepare/Menu/Text/NowEquip.png",
		"Asset/Textures/GameUI/Prepare/Menu/Text/Money.png",
		"Asset/Textures/GameUI/Prepare/Menu/Text/LastRecord.png",
		"Asset/Textures/GameUI/Prepare/Menu/Text/Weapon.png",
		"Asset/Textures/GameUI/Prepare/Menu/Text/Potion.png",
		"Asset/Textures/GameUI/Prepare/Menu/Text/Sword.png",
		"Asset/Textures/GameUI/Prepare/Menu/Text/Dagger.png",
		"Asset/Textures/GameUI/Prepare/Menu/Text/Escape.png",
		"Asset/Textures/GameUI/Prepare/Menu/Text/Lost.png",
		"Asset/Textures/GameUI/Prepare/Menu/Text/Best.png",
		"Asset/Textures/GameUI/Prepare/Menu/Text/Floor.png",
		"Asset/Textures/GameUI/Prepare/Menu/Text/Count.png",
		"Asset/Textures/GameUI/Prepare/Menu/Text/Gold.png",
		"Asset/Textures/GameUI/Prepare/Menu/Text/Reached.png",
		"Asset/Textures/GameUI/Prepare/Menu/Text/ResultLabel.png",
	};

	//武器アイコン(PlayerWeaponInfoの順)
	std::shared_ptr<KdTexture> m_spWeaponIconTex[2];

	//武器アイコンのパス(PlayerWeaponInfoの順)
	const std::string m_weaponIconTexPath[2] =
	{
		"Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/Dagger/WeponStrengtheningIcon8.png",	// PlayerWeaponInfo_Dagger
		"Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/Sword/WeponStrengtheningIcon8.png",	// PlayerWeaponInfo_Sword
	};

	//武器アイコンの位置(左端・上下中央を基準)
	Math::Vector2 m_weaponIconPos = { 440.0f, 146.0f };

	//武器アイコンの大きさ
	float m_weaponIconSize = 40.0f;

	//所持金
	long m_money = 0;

	//持っているポーションの合計(PrepareScene::OnShopDataChanged から入る)
	int m_potionNum = 0;

	//メニュー画像の拡大率(元画像は高さ32px → 96px)
	const float m_menuScale = 3.0f;

	//マウスが当たった時の拡大率
	const float m_mouseHitScale = 1.1f;

	//数字の大きさ(元画像9×14px → 18×28px)
	const float m_numSize = 2.0f;

	//数字と単位のすき間
	const float m_numGap = 4.0f;

	//---- 基本の処理 ----

	//情報パネルの描画
	void Draw();

	//---- その他の処理 ----

	//メニューボタンの画像の読み込み
	void LoadMenuTex();

	//情報パネル・文字の画像の読み込み
	void LoadInfoTex();

	//画像をまとめて読み込む(_num枚分)
	// _spTex ... 読み込んだ画像を入れる配列
	// _path  ... 画像のパスの配列
	// _num   ... 枚数
	void LoadTexArray(std::shared_ptr<KdTexture>* _spTex, const std::string* _path, int _num);

	//マウスと当たってるかを返す
	// _pos ... 四角の左端・上下中央
	// _w   ... 四角の横幅
	// _h   ... 四角の高さ
	//戻り値 ... 当たっていたらtrue
	bool MouseHit(Math::Vector2 _pos, float _w, float _h);

	//文字の位置をセット(左端・上下中央を基準。画面中央が0,0、上がプラス)
	// _text ... 文字の種類
	// _x    ... 左端のx
	// _y    ... 上下中央のy
	void SetTextPos(InfoText _text, float _x, float _y);

	//文字を1つ描く
	// _text   ... 文字の種類
	// _offset ... 位置をずらす量
	void DrawInfoText(InfoText _text, Math::Vector2 _offset = {});

	//数字を単位の文字のすぐ左に右揃えで描く
	// _num       ... 数字
	// _unit      ... 単位の文字(この左に描く)
	// _separator ... 3桁区切りにするか
	// _maxW      ... 超えてはいけない横幅(項目の文字にかぶらないように。0なら制限なし)
	void DrawInfoNum(long _num, InfoText _unit, bool _separator = false, float _maxW = 0.0f);

	//---- メニューが押された時の処理 ----

	//本編(EXPLORE) … Gameシーンへ
	void OnExplore();

	//装備(EQUIP) … 装備画面を開く
	void OnEquip();

	//ショップ(SHOP) … ショップ画面を開く
	void OnShop();

	//Play履歴(RECORDS) … 履歴画面を開く
	void OnRecords();


	//メニューボタンが選ばれているかを返す
	//タブを何も開いていないときは、全部選ばれている見た目にする
	// _menu ... メニューの種類
	//戻り値 ... 選ばれていればtrue
	bool IsMenuSelected(PrepareMenu _menu) const;
};
