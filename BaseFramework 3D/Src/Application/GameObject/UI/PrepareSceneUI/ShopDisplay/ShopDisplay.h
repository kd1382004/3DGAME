#pragma once
#include"../../UIBase.h"
#include"../../../../Scene/SceneManager.h"
#include"../../../Potions/PotionsType.h"
#include"../../../../Info/NumDraw/NumDraw.h"

class PrepareScene;

//座標はフレームワークと同じ(画面の真ん中が0,0・右と上がプラス)
class ShopDisplay :public UIBase
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	ShopDisplay() {};

	//デストラクタ(在庫・持ち物・お金を保存する)
	~ShopDisplay() override { SaveShopData(); };

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

	//所持金を返す
	//戻り値 ... 所持金
	long GetMoney() const { return m_money; }

	//持っている武器を返す
	//戻り値 ... [PlayerWeaponInfo] = その武器を持っているか
	std::vector<bool> GetOwnedWeapons() const
	{
		//配列(bool[]) → vector<bool> に入れ直す(EquipDisplay::SetOwnedWeapons が vector で受け取るため)
		return std::vector<bool>(m_weaponOwned, m_weaponOwned + PlayerWeaponInfoSize);
	}

	//持っているポーションの合計を返す
	//戻り値 ... ポーションの合計
	int GetPotionTotal() const
	{
		//全種類のポーションの数を足す
		int total = 0;
		for (int i = 0; i < PotionsTypeMax; i++)
		{
			total += m_potionHave[i];
		}
		return total;
	}

	//---- 設定 ----

	//準備シーンをセット
	// _spPrepareScene ... 準備シーン
	void SetPrepareScene(std::shared_ptr<PrepareScene> _spPrepareScene) { m_wpPrepareScene = _spPrepareScene; }

	//表示する(true)・しない(false)
	//PrepareScene::ChangeTabから呼ばれる
	// _flg ... 表示するか
	void SetActive(bool _flg) { m_activeFlg = _flg; }

	//---- その他の処理 ----

	//ショップ・持ち物のデータを読み直す
	void LoadShopData();

private:

	//---- 型 ----

	//強化素材の種類
	enum MaterialType
	{
		MaterialType_Crystal1,	// 強化素材1
		MaterialType_Crystal2,	// 強化素材2
		MaterialTypeMax
	};

	//タブ
	enum class ShopTab
	{
		Consumable,	// 消耗品(ポーション)
		Material,	// 素材(強化素材)
		Weapon,		// 武器
		TabSize
	};

	//商品のデータ(ShopMasterData.json / ShopStockData.json)
	struct ShopItem
	{
		//買値
		int m_buyPrice = 0;

		//売値
		int m_sellPrice = 0;

		//今の在庫
		int m_stock = 0;
	};

	//持ち物の欄に並ぶもの(画像と個数)
	struct HaveItem
	{
		//アイコンの画像
		std::shared_ptr<KdTexture> m_spIcon;

		//個数
		int m_num = 0;
	};

	//画面共通の画像
	enum class ShopTex
	{
		Title,				// 画面タイトル(SHOP)
		MoneyLabel,			// 「所持金」
		Gold,				// 「G」
		Count,				// 「個」
		MoneyPanel,			// 所持金パネル
		ListPanel,			// 商品一覧パネル
		DetailPanel,		// 商品詳細パネル
		PriceInfoPanel,		// 所持・値段の欄
		InventoryPanel,		// 持ち物パネル
		SelectFrame,		// 選んでいる枠(金)
		HoverFrame,			// カーソルが乗っている枠(銀)
		LockIcon,			// 未所持の「？」
		Owned,				// 「所持済み」
		NotStocked,			// 「未入荷」
		SoldOut,			// 「売切れ」
		HaveLabel,			// 「所持」
		Have,				// 「あり」
		NotHave,			// 「なし」
		BuyPriceLabel,		// 「買値」
		SellPriceLabel,		// 「売値」
		CountLabel,			// 「数量」
		TotalLabel,			// 「合計」
		InventoryLabel,		// 「持ち物」
		Times,				// 「×」
		Dash,				// 「ー」
		ListTrack,			// 商品一覧のスクロールのレール
		InventoryTrack,		// 持ち物のスクロールのレール
		ThumbLeft,			// つまみの左端
		ThumbMid,			// つまみの中央(伸ばす)
		ThumbRight,			// つまみの右端
		ThumbGrip,			// つまみの持ち手
		ThumbLeftHover,		// マウスを乗せたときのつまみ(左端)
		ThumbMidHover,		// 〃(中央)
		ThumbRightHover,	// 〃(右端)
		ThumbGripHover,		// 〃(持ち手)
		StockLabel,			// 「在庫」
		TexSize
	};

	//ショップのボタンの種類(ButtonSize はボタンの数)
	enum class ShopButton
	{
		Buy,	// 買う
		Sell,	// 売る
		Plus,	// ＋
		Minus,	// －
		ButtonSize
	};

	//ボタンの見た目の状態(StateSize は状態の数)
	enum class ButtonState
	{
		Normal,		// 通常
		Hover,		// カーソルが乗っている
		Disable,	// 押せない
		StateSize
	};

	//商品ごとの画像
	enum class ItemTex
	{
		Icon,			// アイコン
		Name,			// 名前
		Explanation,	// 説明
		ItemTexSize
	};

	//横スクロール(EquipDisplayと同じ動き)
	struct HScroll
	{
		//レールの左端のx
		float m_trackLeft = 0.0f;

		//レールの真ん中のy
		float m_trackY = 0.0f;

		//レールの長さ
		float m_trackW = 0.0f;

		//一度に見える数
		int m_visibleNum = 1;

		//一番左に見えている番号
		int m_index = 0;

		//ドラッグ中か
		bool m_dragFlg = false;

		//つかんだ位置とつまみの左端のずれ
		float m_dragOffsetX = 0.0f;

		//つまみにマウスが乗っているか
		bool m_hoverFlg = false;

		//つまみの左端のx
		float m_thumbX = 0.0f;

		//つまみの長さ
		float m_thumbW = 0.0f;
	};

	//---- メンバ変数 ----

	//準備シーン(循環参照を防ぐためweak_ptr)
	std::weak_ptr<PrepareScene> m_wpPrepareScene;

	//表示中かどうか(開いているタブの時だけtrue)
	bool m_activeFlg = false;

	//このフレームで左クリックされたか(Updateの最初に1回だけ取る)
	bool m_clickFlg = false;

	//今のタブ
	ShopTab m_tab = ShopTab::Consumable;

	//一覧で選んでいる商品(タブの中での番号)
	int m_selectIndex = 0;

	//一覧でカーソルが乗っている商品(乗っていなければ-1)
	int m_hoverIndex = -1;

	//買う・売る数(消耗品・素材だけ)
	int m_count = 1;

	//ポーションの商品
	ShopItem m_potionItem[PotionsTypeMax];

	//強化素材の商品
	ShopItem m_materialItem[MaterialTypeMax];

	//武器の商品
	ShopItem m_weaponItem[PlayerWeaponInfoSize];

	//持っているポーションの数
	int m_potionHave[PotionsTypeMax] = {};

	//持っている強化素材の数(Crystal1, Crystal2)
	int m_materialHave[MaterialTypeMax] = {};

	//武器を持っているか
	bool m_weaponOwned[PlayerWeaponInfoSize] = {};

	//所持金
	//TODO : 所持金の保存場所ができたら差し替える(今はPlayerInventoryData.jsonの"money"を読む。無ければ0)
	long m_money = 0;

	//ショップのマスターデータのパス
	const std::string m_shopMasterPath = "Asset/Data/Shop/ShopMasterData.json";

	//在庫のデータのパス
	const std::string m_shopStockPath = "Asset/Data/PlayerData/ShopStockData.json";

	//持ち物のデータのパス
	const std::string m_inventoryPath = "Asset/Data/PlayerData/PlayerInventoryData.json";

	//武器の画像フォルダ名(PlayerWeaponInfoの順)
	const std::string m_weaponName[PlayerWeaponInfoSize] =
	{
		"Dagger",
		"Sword",
	};

	//画面共通の画像
	std::shared_ptr<KdTexture> m_spTex[static_cast<int>(ShopTex::TexSize)];

	//タブの画像(通常・選択中)
	std::shared_ptr<KdTexture> m_spTabTex[static_cast<int>(ShopTab::TabSize)][2];

	//ボタンの画像
	std::shared_ptr<KdTexture> m_spButtonTex[static_cast<int>(ShopButton::ButtonSize)][static_cast<int>(ButtonState::StateSize)];

	//ポーションの画像
	std::shared_ptr<KdTexture> m_spPotionTex[PotionsTypeMax][static_cast<int>(ItemTex::ItemTexSize)];

	//武器の画像
	std::shared_ptr<KdTexture> m_spWeaponTex[PlayerWeaponInfoSize][static_cast<int>(ItemTex::ItemTexSize)];

	//強化素材の画像
	std::shared_ptr<KdTexture> m_spMaterialTex[MaterialTypeMax][static_cast<int>(ItemTex::ItemTexSize)];

	//商品一覧のスクロール
	HScroll m_listScroll;

	//持ち物のスクロール
	HScroll m_haveScroll;

	//つまみの最小の長さ
	float m_thumbMinW = 0.0f;

	//---- レイアウト(特に書いていなければ、画像の真ん中の位置) ----

	//画面タイトル(SHOP)
	const Math::Vector2 m_titlePos = { 38.0f, 314.0f };

	//所持金パネル(250×64)
	const Math::Vector2 m_moneyPanelPos = { 485.0f, 314.0f };

	//「所持金」(116×56)
	const Math::Vector2 m_moneyLabelPos = { 430.0f, 314.0f };

	//所持金の数字(左端)。「G」はこの右
	const Math::Vector2 m_moneyNumPos = { 490.0f, 314.0f };

	//1つ目のタブ(150×58)
	const Math::Vector2 m_tabPos = { -15.0f, 245.0f };

	//タブを右へずらす量(150 + すき間8)
	const float m_tabSpace = 158.0f;

	//タブの横幅
	const float m_tabW = 150.0f;

	//タブの高さ
	const float m_tabH = 58.0f;

	//商品一覧パネル(700×200)
	const Math::Vector2 m_listPos = { 260.0f, 110.0f };

	//商品一覧パネルの横幅
	const float m_listW = 700.0f;

	//商品一覧パネルの高さ
	const float m_listH = 200.0f;

	//1つ目の枠(64×64)
	const Math::Vector2 m_slotPos = { -24.0f, 138.0f };

	//枠の大きさ
	const float m_slotSiz = 64.0f;

	//枠同士の間隔
	const float m_slotSpace = 96.0f;

	//一覧で一度に見える数
	const int m_listVisibleNum = 7;

	//枠の下の文字(売切れ・所持済み・未入荷)の真ん中のy
	const float m_slotUnderY = 84.0f;

	//枠の下の値段の真ん中のy
	const float m_slotPriceY = 82.0f;

	//商品詳細パネル(700×240)
	const Math::Vector2 m_detailPos = { 260.0f, -118.0f };

	//商品詳細の大きいアイコン(真ん中)
	const Math::Vector2 m_detailIconPos = { 4.0f, -88.0f };

	//商品詳細の名前(左上)
	const Math::Vector2 m_detailNamePos = { -70.0f, -158.0f };

	//商品詳細の説明(左上)
	const Math::Vector2 m_detailExplPos = { 90.0f, -34.0f };

	//左上を基準に描く
	const Math::Vector2 m_pivotLeftTop = { 0.0f, 1.0f };

	//所持・値段の欄(334×206)
	const Math::Vector2 m_infoPos = { 433.0f, -118.0f };

	//項目(「所持」など 76×52)の真ん中のx
	const float m_infoLabelX = 318.0f;

	//値の左端のx
	const float m_infoValueX = 362.0f;

	//買う・売るボタン(92×38)の真ん中のx
	const float m_infoButtonX = 536.0f;

	//1行目の真ん中のy
	const float m_infoRow0Y = -45.0f;

	//下の行へずらす量
	const float m_infoRowSpace = 37.0f;

	//買う・売るボタンの横幅
	const float m_buttonW = 92.0f;

	//買う・売るボタンの高さ
	const float m_buttonH = 38.0f;

	//＋・－ボタンの大きさ
	const float m_smallButtonSize = 36.0f;

	//「あり」「なし」(76×48)の真ん中のx
	const float m_haveTextX = 400.0f;

	//「ー」(48×34)の真ん中のx
	const float m_dashX = 394.0f;

	//1行目の右側の「在庫」(76×52)の真ん中のx
	const float m_stockLabelX = 494.0f;

	//在庫の数の右端(買う・売るボタンの右端にそろえる)
	const float m_stockNumRightX = 582.0f;

	//在庫の数の最大幅(「在庫」の右 〜 欄の右端)
	const float m_stockNumMaxW = 50.0f;

	//1行目の所持の数の最大幅(「個」が「在庫」にかぶらないように。3桁まで)
	const float m_infoHaveNumMaxW = 54.0f;

	//買値・売値の数字の右端(4桁 + カンマ「2,900」の右端にそろえる)。「G」はこのすぐ右
	const float m_infoPriceRightX = 452.0f;

	//「G」(40×42)の真ん中のx
	const float m_infoGoldX = 476.0f;

	//数量の行の－ボタン(36×36)の真ん中のx
	const float m_countMinusX = 374.0f;

	//数量の行の＋ボタン(36×36)の真ん中のx
	const float m_countPlusX = 464.0f;

	//数量の数字の真ん中のx(－と＋の真ん中)
	const float m_countNumX = 419.0f;

	//持ち物パネル(700×104)
	const Math::Vector2 m_havePos = { 260.0f, -298.0f };

	//持ち物パネルの横幅
	const float m_haveW = 700.0f;

	//持ち物パネルの高さ
	const float m_haveH = 104.0f;

	//「持ち物」(104×52)
	const Math::Vector2 m_haveLabelPos = { -22.0f, -286.0f };

	//1つ目の持ち物のアイコン(40×40)の真ん中のx
	const float m_haveItemX = 60.0f;

	//1つ目の持ち物のアイコン(40×40)の真ん中のy
	const float m_haveItemY = -284.0f;

	//持ち物同士の間隔
	const float m_haveSpace = 130.0f;

	//持ち物で一度に見える数
	const int m_haveVisibleNum = 4;

	//スクロールのレールの高さ
	const float m_trackH = 16.0f;

	//レールのふちの太さ(つまみが動けるのはこの分だけ内側)
	const float m_trackInset = 2.0f;

	//所持金の数字の最大幅(「所持金」と「G」の間)
	const float m_moneyNumMaxW = 72.0f;

	//一覧のアイコンの下の値段の最大幅(枠の間隔96 - 「G」)
	const float m_listPriceMaxW = 70.0f;

	//右の欄の値の最大幅
	const float m_infoValueMaxW = 96.0f;

	//数量の最大幅(－と＋の間)
	const float m_countNumMaxW = 40.0f;

	//持ち物の個数の最大幅(「×」の右 〜 次の持ち物)
	const float m_haveNumMaxW = 44.0f;

	//数字1文字の横幅(NumDrawの画像)
	const float m_numGlyphW = 9.0f;

	//所持金の数字の右端(数字の左端 490 + 最大幅 72)。「G」はこのすぐ右
	const float m_moneyNumRightX = 562.0f;

	//所持金の「G」(40×42)の真ん中のx
	const float m_moneyGoldX = 586.0f;

	//---- 購入価格・売却価格(左右2列) ----

	//左の列(購入)の真ん中のx
	const float m_buyColCenterX = 355.0f;

	//右の列(売却)の真ん中のx
	const float m_sellColCenterX = 512.0f;

	//ラベル(購入価格・売却価格)の真ん中のy
	const float m_priceLabelY = -114.0f;

	//値段(数字と G)の真ん中のy
	const float m_priceNumY = -144.0f;

	//ボタン(購入・売却)の真ん中のy
	const float m_priceButtonY = -181.0f;

	//列の真ん中から、数字の右端まで
	const float m_priceNumOffsetX = 20.0f;

	//列の真ん中から、「G」の真ん中まで
	const float m_priceGoldOffsetX = 44.0f;

	//値段の数字の最大幅
	const float m_priceNumMaxW = 80.0f;


	//---- 描画 ----

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

	//数字を描く
	// _num       ... 描く数字
	// _aligned   ... どちらの端で揃えるか
	// _pos       ... x は _aligned で揃える側の端、y は数字の上下の真ん中
	// _size       ... 大きさ
	// _separator ... 3桁ごとに「,」を入れるか
	// _maxW      ... 超えてはいけない横幅(0なら制限なし。超えるときは横を縮める)
	void DrawNum(long _num, Aligned _aligned, const Math::Vector2& _pos, float _size = 2.0f, bool _separator = false, float _maxW = 0.0f);

	//値段の列(ラベル・数字・「G」)を描く
	// _label   ... ラベルの画像(購入価格・売却価格)
	// _price   ... 値段
	// _centerX ... 列の真ん中のx
	void DrawPrice(ShopTex _label, long _price, float _centerX);

	//右の欄の1行目の右側に「在庫 〇」を描く
	// _stock ... 在庫の数
	void DrawStock(int _stock);

	//ボタンを描く(押せるか・カーソルが乗っているかで画像を変える)
	// _button ... ボタンの種類
	// _pos    ... ボタンの真ん中
	// _enable ... 押せるか
	void DrawButton(ShopButton _button, const Math::Vector2& _pos, bool _enable);

	//一覧の枠の下に、値段 + G(半分の大きさ)を枠の真ん中にそろえて描く
	// _price ... 値段
	// _slotX ... 枠の真ん中のx
	void DrawListPrice(int _price, float _slotX);

	//右の欄の項目(「所持」など)を描く
	// _tex ... 項目の画像
	// _row ... 1行目から何行下か
	void DrawInfoLabel(ShopTex _tex, int _row);

	//見出し・所持金・タブの描画
	void DrawHeader();

	//商品一覧の描画
	void DrawList();

	//商品詳細の描画
	void DrawDetail();

	//持ち物の描画
	void DrawInventory();

	//---- 取得 ----

	//一覧に並ぶ商品の数を返す
	//戻り値 ... 商品の数
	int GetListNum() const;

	//持ち物の欄に並ぶものを返す
	//戻り値 ... 持ち物の一覧
	std::vector<HaveItem> GetHaveList() const;

	//数字を描いたときの横幅を返す(_maxWで縮んだ後の幅)
	// _num       ... 数字
	// _size       ... 大きさ
	// _separator ... 3桁ごとに「,」を入れるか
	// _maxW      ... 超えてはいけない横幅(0なら制限なし)
	//戻り値 ... 横幅
	float GetNumWidth(long _num, float _size = 2.0f, bool _separator = false, float _maxW = 0.0f) const;

	//右の欄の行の真ん中のyを返す
	// _row ... 1行目から何行下か
	//戻り値 ... y座標
	float GetInfoRowY(int _row) const { return m_infoRow0Y - m_infoRowSpace * _row; }

	//消耗品・素材(個数で買う・売るもの)のタブか
	//戻り値 ... 消耗品・素材のタブならtrue
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
	// _tex   ... 画像の種類
	//戻り値 ... 画像
	const std::shared_ptr<KdTexture>& GetStackTex(int _index, ItemTex _tex) const;

	//---- その他の処理 ----

	//マウスと当たってるかを返す
	// _pos ... 四角の真ん中
	// _w   ... 四角の横幅
	// _h   ... 四角の高さ
	//戻り値 ... 当たっていたらtrue
	bool MouseHit(const Math::Vector2& _pos, float _w, float _h) const;

	//画像を全部読み込む
	void LoadTex();

	//画像を1枚読み込む(読み込み済みなら何もしない)
	// _spTex ... 読み込んだ画像を入れる先
	// _path  ... 画像のパス
	void LoadOneTex(std::shared_ptr<KdTexture>& _spTex, const std::string& _path);

	//JSONファイルを読む(無い・壊れているときは空のオブジェクト)
	// _path  ... ファイルのパス
	// _isLog ... ファイルが無いときにログを出すか
	//戻り値 ... 読んだデータ
	nlohmann::json ReadJson(const std::string& _path, bool _isLog);

	//JSONファイルに書く
	// _path ... ファイルのパス
	// _data ... 書くデータ
	void WriteJson(const std::string& _path, const nlohmann::json& _data);

	//マスターデータから値段を読む
	// _master ... マスターデータ
	// _key    ... 配列の名前
	// _items  ... 値段を入れる商品の配列
	// _num    ... 商品の種類数
	void ReadPrice(const nlohmann::json& _master, const char* _key, ShopItem* _items, int _num);

	//在庫のデータから在庫を読む
	// _stock ... 在庫のデータ
	// _key   ... 配列の名前
	// _items ... 在庫を入れる商品の配列
	// _num   ... 商品の種類数
	void ReadStock(const nlohmann::json& _stock, const char* _key, ShopItem* _items, int _num);

	//持ち物のデータから持っている数を読む
	// _inventory ... 持ち物のデータ
	// _key       ... 配列の名前
	// _have      ... 持っている数を入れる配列
	// _num       ... 種類数
	void ReadHave(const nlohmann::json& _inventory, const char* _key, int* _have, int _num);

	//配列 _key の中で "id" が合う要素の "stock" に在庫を書く
	//(ファイルに無いidは書かない。順番が違っても id で合わせる)
	// _data  ... 書き換えるデータ
	// _key   ... 配列の名前
	// _items ... 商品の配列
	// _num   ... 商品の種類数
	void WriteStock(nlohmann::json& _data, const char* _key, const ShopItem* _items, int _num);

	//配列 _key の中で "id" が合う要素の "num" に持っている数を書く
	// _data ... 書き換えるデータ
	// _key  ... 配列の名前
	// _have ... 持っている数の配列
	// _num  ... 種類数
	void WriteHave(nlohmann::json& _data, const char* _key, const int* _have, int _num);

	//配列 _key の中で "id" が合う要素の "owned" に持っているかを書く
	// _data  ... 書き換えるデータ
	// _key   ... 配列の名前
	// _owned ... 持っているかの配列
	// _num   ... 種類数
	void WriteOwned(nlohmann::json& _data, const char* _key, const bool* _owned, int _num);

	//つまみの長さと位置を計算する
	// _scroll ... 計算するスクロール
	// _total  ... 並ぶものの数
	void ScrollCalc(HScroll& _scroll, int _total);

	//スクロールの更新(ドラッグ・レールのクリック・ホイール)
	// _scroll   ... 更新するスクロール
	// _total    ... 並ぶものの数
	// _panelPos ... ホイールが効くパネルの真ん中
	// _panelW   ... ホイールが効くパネルの横幅
	// _panelH   ... ホイールが効くパネルの高さ
	void ScrollUpdate(HScroll& _scroll, int _total, const Math::Vector2& _panelPos, float _panelW, float _panelH);

	//スクロールバーを描く
	// _scroll ... 描くスクロール
	// _track  ... レールの画像
	void ScrollDraw(const HScroll& _scroll, ShopTex _track);

	//買う・売るボタンが押されたかを調べて、押されたら下の関数を呼ぶ(Updateから呼ぶ)
	void BuySellUpdate();

	//消耗品・素材を買う(今のタブ)
	//在庫が足りない・お金が足りないときは何もしないで false
	// _index ... 商品の番号
	// _count ... 買う数
	//戻り値 ... 買えたらtrue
	bool BuyStackItem(int _index, int _count);

	//消耗品・素材を売る(今のタブ)
	//持っている数が足りないときは何もしないで false
	// _index ... 商品の番号
	// _count ... 売る数
	//戻り値 ... 売れたらtrue
	bool SellStackItem(int _index, int _count);

	//武器を買う
	//持っている・在庫がない・お金が足りないときは何もしないで false
	// _id ... PlayerWeaponInfo
	//戻り値 ... 買えたらtrue
	bool BuyWeapon(int _id);

	//今の在庫・持ち物・お金をJSONに保存する(買った・売った後に呼ぶ)
	//ShopStockData.json と PlayerInventoryData.json を書き換える
	void SaveShopData();


	//「購入」ボタンの真ん中を返す
	//戻り値 ... 座標
	Math::Vector2 GetBuyButtonPos() const { return { m_buyColCenterX, m_priceButtonY }; }

	//「売却」ボタンの真ん中を返す
	//戻り値 ... 座標
	Math::Vector2 GetSellButtonPos() const { return { m_sellColCenterX, m_priceButtonY }; }
};
