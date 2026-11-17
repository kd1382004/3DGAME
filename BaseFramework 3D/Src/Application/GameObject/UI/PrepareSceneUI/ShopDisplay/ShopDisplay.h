#pragma once
#include"../../UIBase.h"
#include"../../../../Scene/SceneManager.h"
#include"../../../Potions/PotionsType.h"
#include"../../../../Info/NumDraw/NumDraw.h"

class PrepareScene;

class ShopDisplay :public UIBase
{
public:
	//コンストラクタ
	ShopDisplay() {};
	//デストラクタ(在庫・持ち物・お金を保存する)
	~ShopDisplay() override { SaveShopData(); };

	//初期化
	void Init()override;
	//更新
	void Update()override;
	//2Dの描画
	void DrawSprite()override;

	// 準備シーンをセット
	// _spPrepareScene ... 準備シーン
	void SetPrepareScene(std::shared_ptr<PrepareScene> _spPrepareScene) { m_wpPrepareScene = _spPrepareScene; }

	// 表示する(true)・しない(false)
	// PrepareScene::ChangeTabから呼ばれる
	// _flg ... 表示するか
	void SetActive(bool _flg) { m_activeFlg = _flg; }
	//表示中かを返す
	//戻り値 ... 表示中ならtrue
	bool GetActive() const { return m_activeFlg; }

	// ショップ・持ち物のデータを読み直す
	void LoadShopData();

	//所持金を返す
	//戻り値 ... 所持金
	long GetMoney() const { return m_money; }

	//持っている武器を返す
	//戻り値 ... [PlayerWeaponInfo] = その武器を持っているか
	std::vector<bool> GetOwnedWeapons() const
	{
		// 配列(bool[]) → vector<bool> に入れ直す(EquipDisplay::SetOwnedWeapons が vector で受け取るため)
		return std::vector<bool>(m_weaponOwned, m_weaponOwned + PlayerWeaponInfoSize);
	}

	//持っているポーションの合計を返す
	//戻り値 ... ポーションの合計
	int GetPotionTotal() const
	{
		// 全種類のポーションの数を足す
		int total = 0;
		for (int i = 0; i < PotionsTypeMax; i++)
		{
			total += m_potionHave[i];
		}
		return total;
	}
private:

	// 強化素材の種類数(Crystal1, Crystal2)
	static constexpr int kMaterialNum = 2;

	// 準備シーン(循環参照を防ぐためweak_ptr)
	std::weak_ptr<PrepareScene> m_wpPrepareScene;

	// 表示中かどうか(開いているタブの時だけtrue)
	bool m_activeFlg = false;

	// 座標はフレームワークと同じ(画面の真ん中が0,0・上がプラス)

	//マウスと当たってるかを返す(_pos … 四角の真ん中、_w,_h … 幅と高さ)
	bool MouseHit(const Math::Vector2& _pos, float _w, float _h) const;

	// このフレームで左クリックされたか(Updateの最初に1回だけ取る)
	bool m_clickFlg = false;


	////////////////////////////////////////////////////
	// タブ
	////////////////////////////////////////////////////
	enum class ShopTab
	{
		Consumable,	// 消耗品(ポーション)
		Material,	// 素材(強化素材)
		Weapon,		// 武器
		TabSize
	};

	//今のタブ
	ShopTab m_tab = ShopTab::Consumable;

	// 一覧で選んでいる商品(タブの中での番号)
	int m_selectIndex = 0;

	// 一覧でカーソルが乗っている商品(乗っていなければ-1)
	int m_hoverIndex = -1;

	// 買う・売る数(消耗品・素材だけ)
	int m_count = 1;


	////////////////////////////////////////////////////
	// データ(ShopMasterData.json / ShopStockData.json / PlayerInventoryData.json)
	////////////////////////////////////////////////////
	struct ShopItem
	{
		//買値
		int m_buyPrice = 0;
		//売値
		int m_sellPrice = 0;
		int m_stock = 0;		// 今の在庫
	};

	//ポーションの商品
	ShopItem m_potionItem[PotionsTypeMax];
	//強化素材の商品
	ShopItem m_materialItem[kMaterialNum];
	//武器の商品
	ShopItem m_weaponItem[PlayerWeaponInfoSize];

	int  m_potionHave[PotionsTypeMax] = {};		// 持っているポーションの数
	int  m_materialHave[kMaterialNum] = {};			// 持っている強化素材の数(Crystal1, Crystal2)
	bool m_weaponOwned[PlayerWeaponInfoSize] = {};	// 武器を持っているか

	// 所持金
	// TODO : 所持金の保存場所ができたら差し替える(今はPlayerInventoryData.jsonの"money"を読む。無ければ0)
	long m_money = 0;

	//ショップのマスターデータのパス
	const std::string m_shopMasterPath = "Asset/Data/Shop/ShopMasterData.json";
	//在庫のデータのパス
	const std::string m_shopStockPath = "Asset/Data/PlayerData/ShopStockData.json";
	//持ち物のデータのパス
	const std::string m_inventoryPath = "Asset/Data/PlayerData/PlayerInventoryData.json";

	// 一覧に並ぶ商品の数
	int GetListNum() const;

	// 持ち物の欄に並ぶもの(画像の番号と個数)
	struct HaveItem
	{
		//アイコンの画像
		std::shared_ptr<KdTexture> m_spIcon;
		//個数
		int m_num = 0;
	};
	//持ち物の欄に並ぶものを返す
	//戻り値 ... 持ち物の一覧
	std::vector<HaveItem> GetHaveList() const;


	////////////////////////////////////////////////////
	// 画像
	////////////////////////////////////////////////////
	enum class ShopTex
	{
		Title, MoneyLabel, Gold, Count,
		MoneyPanel, ListPanel, DetailPanel, PriceInfoPanel, InventoryPanel,
		SelectFrame, HoverFrame, LockIcon,
		Owned, NotStocked, SoldOut,
		HaveLabel, Have, NotHave, BuyPriceLabel, SellPriceLabel, CountLabel, TotalLabel, InventoryLabel, Times, Dash,
		ListTrack, InventoryTrack,
		ThumbLeft, ThumbMid, ThumbRight, ThumbGrip,
		ThumbLeftHover, ThumbMidHover, ThumbRightHover, ThumbGripHover,
		StockLabel,		// 「在庫」
		TexSize
	};
	//画面共通の画像
	std::shared_ptr<KdTexture> m_spTex[static_cast<int>(ShopTex::TexSize)];

	// タブ(通常・選択中)
	std::shared_ptr<KdTexture> m_spTabTex[static_cast<int>(ShopTab::TabSize)][2];

	// ボタン(通常・カーソル・押せない)
	enum class ShopButton { Buy, Sell, Plus, Minus, ButtonSize };
	enum class ButtonState { Normal, Hover, Disable, StateSize };
	//ボタンの画像
	std::shared_ptr<KdTexture> m_spButtonTex[static_cast<int>(ShopButton::ButtonSize)][static_cast<int>(ButtonState::StateSize)];

	// 商品ごとの画像(アイコン・名前・説明)
	enum class ItemTex { Icon, Name, Explanation, ItemTexSize };
	//ポーションの画像
	std::shared_ptr<KdTexture> m_spPotionTex[PotionsTypeMax][static_cast<int>(ItemTex::ItemTexSize)];
	//武器の画像
	std::shared_ptr<KdTexture> m_spWeaponTex[PlayerWeaponInfoSize][static_cast<int>(ItemTex::ItemTexSize)];
	//強化素材の画像
	std::shared_ptr<KdTexture> m_spMaterialTex[kMaterialNum][static_cast<int>(ItemTex::ItemTexSize)];

	//画像を全部読み込む
	void LoadTex();
	//画像を1枚読み込む(読み込み済みなら何もしない)
	// _spTex ... 読み込んだ画像を入れる先
	// _path ... 画像のパス
	void LoadOneTex(std::shared_ptr<KdTexture>& _spTex, const std::string& _path);


	////////////////////////////////////////////////////
	// 横スクロール(EquipDisplayと同じ動き)
	////////////////////////////////////////////////////
	struct HScroll
	{
		float m_trackLeft = 0.0f, m_trackY = 0.0f, m_trackW = 0.0f;	// レール(左端のx・真ん中のy・長さ)
		int   m_visibleNum = 1;		// 一度に見える数
		int   m_index = 0;			// 一番左に見えている番号
		bool  m_dragFlg = false;		// ドラッグ中か
		float m_dragOffsetX = 0.0f;	// つかんだ位置とつまみの左端のずれ
		bool  m_hoverFlg = false;		// つまみにマウスが乗っているか
		float m_thumbX = 0.0f, m_thumbW = 0.0f;	// つまみ(左端のx・長さ)
	};
	HScroll m_listScroll;		// 商品一覧
	HScroll m_haveScroll;		// 持ち物

	float m_thumbMinW = 0.0f;	// つまみの最小の長さ

	//つまみの長さと位置を計算する
	// _scroll ... 計算するスクロール
	// _total ... 並ぶものの数
	void ScrollCalc(HScroll& _scroll, int _total);
	// _panelPos,_panelW,_panelH … ホイールが効くパネル(真ん中と大きさ)
	// _scroll ... 更新するスクロール
	// _total ... 並ぶものの数
	void ScrollUpdate(HScroll& _scroll, int _total, const Math::Vector2& _panelPos, float _panelW, float _panelH);
	//スクロールバーを描く
	// _scroll ... 描くスクロール
	// _track ... レールの画像
	void ScrollDraw(const HScroll& _scroll, ShopTex _track);


	////////////////////////////////////////////////////
	// 描画
	////////////////////////////////////////////////////
	// 画像を描く(_pos … _pivotの点の位置。_pivotは{0.5,0.5}で真ん中、{0,1}で左上)
	// _spTex ... 描く画像
	// _scale ... 元画像に対する倍率
	void DrawTex(const std::shared_ptr<KdTexture>& _spTex, const Math::Vector2& _pos, float _scale = 1.0f,
		const Math::Vector2& _pivot = { 0.5f, 0.5f });
	//大きさを指定して描く(つまみの中央を伸ばすとき用)
	// _spTex ... 描く画像
	// _pos ... _pivotの点の位置
	// _w ... 横幅
	// _h ... 高さ
	// _pivot ... 基準点({0.5,0.5}で真ん中)
	void DrawTexSize(const std::shared_ptr<KdTexture>& _spTex, const Math::Vector2& _pos, float _w, float _h,
		const Math::Vector2& _pivot = { 0.5f, 0.5f });

	// 数字を描く(_pos.x … _alignedで揃える側の端、_pos.y … 数字の上下の真ん中)
	// _maxW … 超えてはいけない横幅(0なら制限なし。超えるときは横を縮める)
	// _num ... 描く数字
	// _siz ... 大きさ
	// _separator ... 3桁ごとに「,」を入れるか
	void DrawNum(long _num, Aligned _aligned, const Math::Vector2& _pos, float _siz = 2.0f, bool _separator = false, float _maxW = 0.0f);

	// 数字を描いたときの横幅(_maxWで縮んだ後の幅)
	// _num ... 数字
	// _siz ... 大きさ
	// _separator ... 3桁ごとに「,」を入れるか
	float GetNumWidth(long _num, float _siz = 2.0f, bool _separator = false, float _maxW = 0.0f) const;

	// 右の欄の買値・売値を描く(数字は右揃え、「G」はいつも同じ位置)
	// _rowY … その行の真ん中のy
	// _price ... 値段
	void DrawPrice(long _price, float _rowY);

	// 右の欄の1行目の右側に「在庫 〇」を描く
	// _stock ... 在庫の数
	void DrawStock(int _stock);

	// ボタンを描く(_pos … ボタンの真ん中)。押せるか・カーソルが乗っているかで画像を変える
	// _button ... ボタンの種類
	// _enable ... 押せるか
	void DrawButton(ShopButton _button, const Math::Vector2& _pos, bool _enable);

	void DrawHeader();		// 見出し・所持金・タブ
	void DrawList();		// 商品一覧
	void DrawDetail();		// 商品詳細

	// 消耗品・素材(個数で買う・売るもの)の、選んでいる商品の情報
	bool IsStackTab() const { return m_tab == ShopTab::Consumable || m_tab == ShopTab::Material; }
	//消耗品・素材の商品を返す(今のタブ)
	// _index ... 商品の番号
	//戻り値 ... 商品
	const ShopItem& GetStackItem(int _index) const;
	//消耗品・素材の持っている数を返す(今のタブ)
	// _index ... 商品の番号
	//戻り値 ... 持っている数
	int GetStackHave(int _index) const;
	//消耗品・素材の画像を返す(今のタブ)
	// _index ... 商品の番号
	// _tex ... 画像の種類
	//戻り値 ... 画像
	const std::shared_ptr<KdTexture>& GetStackTex(int _index, ItemTex _tex) const;
	void DrawInventory();	// 持ち物


	////////////////////////////////////////////////////
	// 買う・売る
	////////////////////////////////////////////////////
	// 買う・売るボタンが押されたかを調べて、押されたら下の関数を呼ぶ(Updateから呼ぶ)
	void BuySellUpdate();

	// 消耗品・素材を _count 個買う(今のタブの _index 番)
	// 在庫が足りない・お金が足りないときは何もしないで false
	bool BuyStackItem(int _index, int _count);

	// 消耗品・素材を _count 個売る(今のタブの _index 番)
	// 持っている数が足りないときは何もしないで false
	bool SellStackItem(int _index, int _count);

	// 武器を買う(_id … PlayerWeaponInfo)
	// 持っている・在庫がない・お金が足りないときは何もしないで false
	bool BuyWeapon(int _id);

	// 今の在庫・持ち物・お金をJSONに保存する(買った・売った後に呼ぶ)
	// ShopStockData.json と PlayerInventoryData.json を書き換える
	void SaveShopData();
};
