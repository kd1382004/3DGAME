#pragma once
#include"../../UIBase.h"
#include"../../../../Scene/SceneManager.h"

class PrepareScene;

//座標はフレームワークと同じ(画面の真ん中が0,0・右と上がプラス)
class EquipDisplay :public UIBase
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	EquipDisplay() {};

	//デストラクタ
	~EquipDisplay() override {};

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

	//表示する(true)・しない(false)
	//PrepareScene::ChangeTabから呼ばれる
	// _flg ... 表示するか
	void SetActive(bool _flg) { m_activeFlg = _flg; }

	//持っている武器をセット(PrepareScene::Initから、Initより前に呼ぶ)
	// _owned ... _owned[PlayerWeaponInfo] = その武器を持っているか
	void SetOwnedWeapons(const std::vector<bool>& _owned);

private:

	//---- 型 ----

	//画面共通の画像
	enum class EquipTex
	{
		ListPanel,		// 武器一覧の背景
		DetailPanel,	// 武器詳細の背景
		SelectFrame,	// 装備中の枠(金)
		HoverFrame,		// カーソルが乗っている枠(銀)
		Title,			// 画面タイトル(EQUIP)
		Equipped,		// 「装備中」
		ClickEquip,		// 「クリックで装備」
		LockIcon,		// 未所持の「？」
		ScrollTrack,	// スクロールのレール
		ThumbLeft,		// つまみの左端
		ThumbMid,		// つまみの中央(繰り返す)
		ThumbRight,		// つまみの右端
		ThumbGrip,		// つまみの持ち手
		ThumbLeftHover,	// マウスを乗せたときのつまみ(左端)
		ThumbMidHover,	// 〃(中央)
		ThumbRightHover,// 〃(右端)
		ThumbGripHover,	// 〃(持ち手)
		TexSize
	};

	//武器ごとの画像
	enum class WeaponTex
	{
		Icon,			// アイコン
		Name,			// 名前(ソード・ダガー)
		Explanation,	// 説明
		WeaponTexSize
	};

	//---- メンバ変数 ----

	//準備シーン(循環参照を防ぐためweak_ptr)
	std::weak_ptr<PrepareScene> m_wpPrepareScene;

	//表示中かどうか(開いているタブの時だけtrue)
	bool m_activeFlg = false;

	//画面共通の画像
	std::shared_ptr<KdTexture> m_spTex[static_cast<int>(EquipTex::TexSize)];

	//画面共通の画像のパス(EquipTexの順)
	const std::string m_texPath[static_cast<int>(EquipTex::TexSize)] =
	{
		"Asset/Textures/GameUI/Prepare/Equip/Panel/WeaponListPanel.png",
		"Asset/Textures/GameUI/Prepare/Equip/Panel/WeaponDetailPanel.png",
		"Asset/Textures/GameUI/Prepare/Equip/Frame/SelectFrame.png",
		"Asset/Textures/GameUI/Prepare/Equip/Frame/HoverFrame.png",
		"Asset/Textures/GameUI/Prepare/Equip/Text/EquipTitle.png",
		"Asset/Textures/GameUI/Prepare/Equip/Text/Equipped.png",
		"Asset/Textures/GameUI/Prepare/Equip/Text/ClickEquip.png",
		"Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/NotIcon.png",
		"Asset/Textures/GameUI/Prepare/Equip/Scroll/ScrollTrackH.png",
		"Asset/Textures/GameUI/Prepare/Equip/Scroll/ScrollThumbLeftH.png",
		"Asset/Textures/GameUI/Prepare/Equip/Scroll/ScrollThumbMidH.png",
		"Asset/Textures/GameUI/Prepare/Equip/Scroll/ScrollThumbRightH.png",
		"Asset/Textures/GameUI/Prepare/Equip/Scroll/ScrollThumbGripH.png",
		"Asset/Textures/GameUI/Prepare/Equip/Scroll/ScrollThumbLeftH_Hover.png",
		"Asset/Textures/GameUI/Prepare/Equip/Scroll/ScrollThumbMidH_Hover.png",
		"Asset/Textures/GameUI/Prepare/Equip/Scroll/ScrollThumbRightH_Hover.png",
		"Asset/Textures/GameUI/Prepare/Equip/Scroll/ScrollThumbGripH_Hover.png",
	};

	//武器ごとの画像
	std::shared_ptr<KdTexture> m_spWeaponTex[PlayerWeaponInfoSize][static_cast<int>(WeaponTex::WeaponTexSize)];

	//武器ごとの画像のパス(PlayerWeaponInfoの順 × WeaponTexの順)
	const std::string m_weaponTexPath[PlayerWeaponInfoSize][static_cast<int>(WeaponTex::WeaponTexSize)] =
	{
		{	// PlayerWeaponInfo_Dagger
			"Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/Dagger/WeponStrengtheningIcon8.png",
			"Asset/Textures/GameUI/Item/WeaponsStrengthening/Dagger/Icon/SelectNameBase.png",
			"Asset/Textures/GameUI/Item/WeaponsStrengthening/Dagger/Icon/SelectExplanationBase.png",
		},
		{	// PlayerWeaponInfo_Sword
			"Asset/Textures/GameUI/Item/PlayerInventory/WeponStrengtheningIcon/Sword/WeponStrengtheningIcon8.png",
			"Asset/Textures/GameUI/Item/WeaponsStrengthening/Sword/Icon/SelectNameBase.png",
			"Asset/Textures/GameUI/Item/WeaponsStrengthening/Sword/Icon/SelectExplanationBase.png",
		},
	};

	//一番左に見えている武器の番号(スクロール位置)
	//0 〜 (武器の数 - 一度に見える数) の範囲で動く
	int m_scrollIndex = 0;

	//持っている武器の番号(PlayerWeaponInfo)を並べたもの
	//一覧にはこの順で、持っている武器だけを出す
	std::vector<int> m_ownedList;

	//カーソルが乗っている武器の番号(乗っていなければ-1)
	int m_hoverIndex = -1;

	//スクロールバーのつまみの左端のX(PotionInventoryUIの縦を横にしたもの)
	float m_scrollThumbX = 0.0f;

	//つまみの横の長さ
	float m_scrollThumbW = 0.0f;

	//つまみの横の長さの最小(左端＋持ち手＋右端が入る長さ)
	float m_scrollThumbMinW = 0.0f;

	//つまみにマウスが乗っているか(ドラッグ中もtrue)
	bool m_scrollThumbHoverFlg = false;

	//つまみをドラッグしている最中か
	//つまみの上で左クリックを押した瞬間にtrue、離したらfalse
	bool m_scrollDragFlg = false;

	//つかんだ位置と、つまみの左端との横のずれ(マウスのx − つまみの左端のx)
	//つまみの真ん中をつかんでも、左端がマウスにワープしないようにする
	float m_scrollDragOffsetX = 0.0f;

	//---- レイアウト(特に書いていなければ、画像の真ん中の位置) ----

	//画面タイトル(EQUIP)
	const Math::Vector2 m_titlePos = { -22.0f, 289.0f };

	//武器一覧パネル(700×200)
	const Math::Vector2 m_listPos = { 260.0f, 150.0f };

	//武器詳細パネル(700×330)
	const Math::Vector2 m_detailPos = { 260.0f, -130.0f };

	//武器一覧パネルの横幅(ホイールを効かせる範囲)
	const float m_listW = 700.0f;

	//武器一覧パネルの高さ(ホイールを効かせる範囲)
	const float m_listH = 200.0f;

	//一度に見える武器の数
	const int m_visibleNum = 8;

	//アイコンの大きさ
	const float m_slotSiz = 64.0f;

	//アイコン同士の間隔
	const float m_slotSpace = 80.0f;

	//1つ目のアイコンの真ん中
	const Math::Vector2 m_slotPos = { -24.0f, 178.0f };

	//「装備中」(98×50)の真ん中のy(アイコンの下)
	const float m_equippedY = 119.0f;

	//スクロールバーのレールの左端のx
	const float m_trackLeft = -60.0f;

	//レールの真ん中のy
	const float m_trackY = 80.0f;

	//レールの長さ
	const float m_trackW = 640.0f;

	//レールの高さ
	const float m_trackH = 16.0f;

	//レールのふちの太さ(つまみが動けるのはこの分だけ内側)
	const float m_trackInset = 2.0f;

	//武器詳細のアイコン(64px → 2倍)の真ん中
	const Math::Vector2 m_detailIconPos = { 14.0f, -69.0f };

	//武器詳細の名前(左上)
	const Math::Vector2 m_detailNamePos = { -60.0f, -153.0f };

	//武器詳細の説明(左上)
	const Math::Vector2 m_detailExplPos = { 130.0f, -15.0f };

	//「クリックで装備」(216×52)
	const Math::Vector2 m_clickEquipPos = { 28.0f, -331.0f };

	//左上を基準に描く
	const Math::Vector2 m_pivotLeftTop = { 0.0f, 1.0f };

	//左端(上下は真ん中)を基準に描く
	const Math::Vector2 m_pivotLeft = { 0.0f, 0.5f };

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

	//一覧に出す武器の数を返す(= 持っている武器の数)
	//戻り値 ... 武器の数
	int GetWeaponNum() const { return static_cast<int>(m_ownedList.size()); }

	//スクロールバーの更新(ドラッグ・レールのクリック・ホイールでスクロール位置を変える)
	void ScrollBarUpdate();

	//つまみの長さと位置を計算する
	void ScrollBarCalcThumb();

	//一番右までスクロールしたときの番号を返す
	//戻り値 ... 番号
	int GetMaxScrollIndex() const;

	//一覧の見えている枠の真ん中の座標を返す
	// _slot ... 見えている枠の番号
	//戻り値 ... 座標
	Math::Vector2 GetSlotPos(int _slot) const;

	//画像を描く
	// _spTex ... 描く画像
	// _pos   ... _pivotの点の位置
	// _scale ... 元画像に対する倍率
	// _pivot ... 基準点({0.5,0.5}で真ん中、{0,1}で左上)
	void DrawTex(const std::shared_ptr<KdTexture>& _spTex, const Math::Vector2& _pos, float _scale = 1.0f,
		const Math::Vector2& _pivot = { 0.5f, 0.5f });

	//大きさを指定して描く(つまみの中央を伸ばすとき用)
	// _spTex ... 描く画像
	// _pos   ... _pivotの点の位置
	// _w     ... 横幅
	// _h     ... 高さ
	// _pivot ... 基準点({0.5,0.5}で真ん中)
	void DrawTexSize(const std::shared_ptr<KdTexture>& _spTex, const Math::Vector2& _pos, float _w, float _h,
		const Math::Vector2& _pivot = { 0.5f, 0.5f });

	//武器一覧の描画
	void DrawList();

	//スクロールバーの描画
	void DrawScrollBar();

	//武器詳細の描画
	void DrawDetail();
};
