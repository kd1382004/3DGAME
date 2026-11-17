#pragma once
#include"../../UIBase.h"
#include"../../../../Scene/SceneManager.h"

class PrepareScene;

class EquipDisplay :public UIBase
{
public:
	EquipDisplay() {};
	~EquipDisplay() override{};

	void Init()override;
	void Update()override;
	void DrawSprite()override;

	// 準備シーンをセット
	void SetPrepareScene(std::shared_ptr<PrepareScene> _spPrepareScene) { m_wpPrepareScene = _spPrepareScene; }

	// 表示する(true)・しない(false)
	// PrepareScene::ChangeTabから呼ばれる
	void SetActive(bool _flg) { m_activeFlg = _flg; }
	bool GetActive() const { return m_activeFlg; }

	// 持っている武器をセット(PrepareScene::Initから、Initより前に呼ぶ)
	// _owned[PlayerWeaponInfo] = その武器を持っているか
	void SetOwnedWeapons(const std::vector<bool>& _owned);

private:

	// 準備シーン(循環参照を防ぐためweak_ptr)
	std::weak_ptr<PrepareScene> m_wpPrepareScene;

	// 表示中かどうか(開いているタブの時だけtrue)
	bool m_activeFlg = false;

	//マウスと当たってるかを返す
	bool MouseHit(Math::Vector2 _pos,float _w,float _h);


	////////////////////////////////////////////////////
	// 画像
	////////////////////////////////////////////////////

	// 画面共通の画像
	enum class EquipTex
	{
		ListPanel,		// 武器一覧の背景
		DetailPanel,	// 武器詳細の背景
		SelectFrame,	// 装備中の枠(金)
		HoverFrame,		// カーソルが乗っている枠(銀)
		Title,			// 「装備」
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

	std::shared_ptr<KdTexture> m_spTex[static_cast<int>(EquipTex::TexSize)];

	// パス(EquipTexの順)
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

	// 武器ごとの画像
	enum class WeaponTex
	{
		Icon,			// アイコン
		Name,			// 名前(ソード・ダガー)
		Explanation,	// 説明
		WeaponTexSize
	};

	std::shared_ptr<KdTexture> m_spWeaponTex[PlayerWeaponInfoSize][static_cast<int>(WeaponTex::WeaponTexSize)];

	// パス(PlayerWeaponInfoの順 × WeaponTexの順)
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

	// 画像の読み込み
	void LoadTex();
	void LoadOneTex(std::shared_ptr<KdTexture>& _spTex, const std::string& _path);


	////////////////////////////////////////////////////
	// 状態
	////////////////////////////////////////////////////

	// 一番左に見えている武器の番号(スクロール位置)
	// 0 〜 (武器の数 - 一度に見える数) の範囲で動く
	int m_scrollIndex = 0;

	// 持っている武器の番号(PlayerWeaponInfo)を並べたもの
	// 一覧にはこの順で、持っている武器だけを出す
	std::vector<int> m_ownedList;

	// 一覧に出す武器の数(= 持っている武器の数)
	int GetWeaponNum() const { return static_cast<int>(m_ownedList.size()); }

	//---------- スクロールバー(PotionInventoryUIの縦を横にしたもの) ----------
	// つまみの左端のX・横の長さ(画面左上が0,0の座標)
	float m_scrollThumbX = 0.0f;
	float m_scrollThumbW = 0.0f;

	// つまみの横の長さの最小(左端＋持ち手＋右端が入る長さ)
	float m_scrollThumbMinW = 0.0f;

	// つまみにマウスが乗っているか(ドラッグ中もtrue)
	bool m_scrollThumbHoverFlg = false;

	// つまみをドラッグしている最中か
	// つまみの上で左クリックを押した瞬間にtrue、離したらfalse
	bool m_scrollDragFlg = false;

	// つかんだ位置と、つまみの左端との横のずれ(マウスのx − つまみの左端のx)
	// つまみの真ん中をつかんでも、左端がマウスにワープしないようにする
	float m_scrollDragOffsetX = 0.0f;

	//スクロールバーの更新(ドラッグ・レールのクリック・ホイールでスクロール位置を変える)
	void ScrollBarUpdate();

	//つまみの長さと位置を計算する
	void ScrollBarCalcThumb();

	// 一番右までスクロールしたときの番号
	int GetMaxScrollIndex() const;

	// マウスの位置(画面左上が0,0・下がプラスに変換したもの)
	Math::Vector2 GetMouseTL() const;

	// カーソルが乗っている武器の番号(乗っていなければ-1)
	int m_hoverIndex = -1;

	// 一覧のi番目(見えている枠の番号)の左上座標(画面左上が0,0の座標)
	Math::Vector2 GetSlotTopLeft(int _slot) const;


	////////////////////////////////////////////////////
	// 描画
	////////////////////////////////////////////////////

	// 画面左上が0,0・下がプラスの座標(左上基準)で描く
	// _scale … 元画像に対する倍率
	void DrawTL(const std::shared_ptr<KdTexture>& _spTex, float _x, float _y, float _scale = 1.0f, const Math::Color& _color = kWhiteColor);

	// 大きさを指定して描く(つまみの中央を伸ばすとき用)
	void DrawTLSize(const std::shared_ptr<KdTexture>& _spTex, float _x, float _y, float _w, float _h);

	void DrawList();		// 武器一覧
	void DrawScrollBar();	// スクロールバー
	void DrawDetail();		// 武器詳細
};
