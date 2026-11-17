#pragma once
#include"../../UIBase.h"
#include"../../../../Scene/SceneManager.h"

class PrepareScene;

class PrepareSceneDisplay :public UIBase
{
public:
	PrepareSceneDisplay() {};
	~PrepareSceneDisplay() override {};

	void Init()override;
	void Update()override;
	void DrawSprite()override;

	// 準備シーンをセット
	void SetPrepareScene(std::shared_ptr<PrepareScene> _spPrepareScene) { m_wpPrepareScene = _spPrepareScene; }

	// プレイ記録をセット(PrepareScene::Initから呼ばれる)
	void SetGameRecord(const GameRecord& _gameRecord) { m_gameRecord = _gameRecord; }

	// 特有の情報を表示する(true)・しない(false)
	// PrepareScene::ChangeTabから呼ばれる
	void SetActive(bool _flg) { m_activeFlg = _flg; }
	bool GetActive() const { return m_activeFlg; }

private:

	// 表示中かどうか(開いているタブの時だけtrue)
	bool m_activeFlg = false;

	// 準備シーン(循環参照を防ぐためweak_ptr)
	std::weak_ptr<PrepareScene> m_wpPrepareScene;

	// プレイ記録(最新1件 + 最高階層ベスト3)
	GameRecord m_gameRecord;

	// メニューの種類(上から並ぶ順)
	enum class PrepareMenu
	{
		Explore,	// 本編(EXPLORE)
		Equip,		// 装備(EQUIP)
		Shop,		// ショップ(SHOP)
		Records,	// Play履歴(RECORDS)
		MenuSize
	};

	// メニューボタンの画像
	std::shared_ptr<KdTexture> m_spMenuTex[static_cast<int>(PrepareMenu::MenuSize)];
	Math::Vector2 m_spMenuPos[static_cast<int>(PrepareMenu::MenuSize)];
	Math::Vector2 m_spMenuSiz[static_cast<int>(PrepareMenu::MenuSize)];
	Math::Vector2 m_spMenuDrawSiz[static_cast<int>(PrepareMenu::MenuSize)];
	bool m_mouseHitFlg[static_cast<int>(PrepareMenu::MenuSize)];


	// 画像のパス(PrepareMenuの順)
	const std::string m_menuTexPath[static_cast<int>(PrepareMenu::MenuSize)] =
	{
		"Asset/Textures/Button/PrepareMenu/ExploreButton.png",
		"Asset/Textures/Button/PrepareMenu/EquipButton.png",
		"Asset/Textures/Button/PrepareMenu/ShopButton.png",
		"Asset/Textures/Button/PrepareMenu/RecordsButton.png",
	};

	// 画像の読み込み
	void LoadMenuTex();

	////////////////////////////////////////////////////
	// 情報パネル(右側)の画像
	////////////////////////////////////////////////////

	// パネル(背景)の種類
	enum class InfoPanel
	{
		NowEquip,	// 今の装備
		Money,	// 所持金
		LastRecord,	// 前回の記録
		PanelSize
	};

	// パネルの画像
	std::shared_ptr<KdTexture> m_spPanelTex[static_cast<int>(InfoPanel::PanelSize)];

	// パネル画像のパス(InfoPanelの順)
	const std::string m_panelTexPath[static_cast<int>(InfoPanel::PanelSize)] =
	{
		"Asset/Textures/GameUI/Prepare/Menu/Panel/NowEquipPanel.png",
		"Asset/Textures/GameUI/Prepare/Menu/Panel/MoneyPanel.png",
		"Asset/Textures/GameUI/Prepare/Menu/Panel/LastRecordPanel.png",
	};

	// 文字の種類
	enum class InfoText
	{
		Title,	// 出発の準備
		NowEquip,	// 今の装備
		Money,	// 所持金
		LastRecord,	// 前回の記録
		Weapon,	// 武器
		Potion,	// ポーション
		Sword,	// 剣
		Dagger,	// ダガー
		Escape,	// 脱出
		Lost,	// ロスト
		Best,	// 最高
		Floor,	// 階
		Count,	// 個
		Gold,	// G
		Reached,		// 到達階
		ResultLabel,	// 結果
		TextSize
	};

	// 文字の画像
	std::shared_ptr<KdTexture> m_spTextTex[static_cast<int>(InfoText::TextSize)];
	Math::Vector2 m_spTextPos[static_cast<int>(InfoText::TextSize)];
	Math::Vector2 m_panelPos[static_cast<int>(InfoPanel::PanelSize)];

	// 文字画像のパス(InfoTextの順)
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

	// 情報パネル・文字の画像の読み込み
	void LoadInfoTex();

	// 画像をまとめて読み込む(_num枚分)
	void LoadTexArray(std::shared_ptr<KdTexture>* _spTex, const std::string* _path, int _num);


	// 武器アイコン(PlayerWeaponInfoの順)
	std::shared_ptr<KdTexture> m_spWeaponIconTex[2];

	const std::string m_weaponIconTexPath[2] =
	{
		"Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/Dagger/WeponStrengtheningIcon8.png",	// PlayerWeaponInfo_Dagger
		"Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/Sword/WeponStrengtheningIcon8.png",	// PlayerWeaponInfo_Sword
	};

	// 武器アイコンの位置と大きさ(左端・上下中央を基準)
	Math::Vector2 m_weaponIconPos = { 440.0f, 146.0f };
	float         m_weaponIconSiz = 40.0f;


	void Draw();

	//マウスと当たってるかを返す
	bool MouseHit(Math::Vector2 _pos, float _w, float _h);


	// メニューが押された時の処理
	void OnExplore();	// 本編(EXPLORE) … Gameシーンへ
	void OnEquip();		// 装備(EQUIP)   … 装備画面を開く
	void OnShop();		// ショップ(SHOP) … ショップ画面を開く
	void OnRecords();	// Play履歴(RECORDS) … 履歴画面を開く
};
