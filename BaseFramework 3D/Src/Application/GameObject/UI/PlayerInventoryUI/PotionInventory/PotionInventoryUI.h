#pragma once
#include"../InventoryUIContext.h"

//////////////////////////////////////////////////////////////
// インベントリ：アイテム（ポーション）画面
//////////////////////////////////////////////////////////////
class PotionInventoryUI :public InventoryTabBase
{
public:
	//コンストラクタ
	PotionInventoryUI() {}
	//デストラクタ
	~PotionInventoryUI()override {}

	//初期化
	void Init()override;
	void OnSelect()override;		// = 旧 PotionInfoInit
	void Update()override;			// = 旧 PotionUpdate
	void DrawSprite()override;		// = 旧 PotionDraw ＋ 使用ボタン

	// インベントリを開いたとき（アイテムがあれば true）
	bool OnInventoryOpen();

private:
	//あるものを入れてく関数
	void AddPotionTexInfo();

	//アイコンとマウスが当たってるか
	void IconHit();

	//使用
	void PotionUse();

	using ItemIconInfo = InventoryItemIconInfo;
	//表示するアイテムのアイコン一覧
	std::vector<ItemIconInfo> m_itemIconInfo;
	//アイコンの大きさ
	struct { int m_w; int m_h; } m_iconDimensions = { 40,40 };

	//選ばれてるポーションのID
	int m_selectPotionID = -999;

	//---------- アイテム一覧のマス（スロット） ----------
	// 横に並べるマスの数（この数を超えたら次の段へ）
	const int m_slotCols = 14;

	// 縦に並べるマスの数（空きマスを何段分描くか）
	const int m_slotRows = 6;

	// 一番左上のマスの位置
	const Math::Vector2 m_slotBasePos = { -450, 60 };

	// マスとマスのすき間（ピクセル）
	const float m_slotSpacing = 10.0f;


	int  m_SlotSetKey = 'F';
	std::shared_ptr<KdTexture> m_SlotRegisterTex;
	std::shared_ptr<KdTexture> m_SlotUnregisterTex;
	std::shared_ptr<KdTexture> m_SlotFullTex;
	std::shared_ptr<KdTexture> m_spNumSlashTex;
	Math::Vector2 m_SlotTexPos;

	//---------- 種類の切り替えボタン（消耗品／素材） ----------
	// 通常（選ばれていないとき）の画像
	std::shared_ptr<KdTexture> m_spCategoryTex[ItemCategory_Num];

	// 選ばれているときの画像（明るい）
	std::shared_ptr<KdTexture> m_spCategorySelectTex[ItemCategory_Num];


	// 今選ばれている種類（開いたときは消耗品）
	ItemCategory m_nowCategory = ItemCategory_Consumable;

	// ボタンの中心座標（種類ごと）
	Math::Vector2 m_categoryPos[ItemCategory_Num];

	// ボタンの大きさ（画像から。全部同じ大きさ 96×34）
	Math::Vector2 m_categorySize;

	// ボタンとボタンのすき間（ピクセル）
	const float m_categorySpacing = 8.0f;

	// 枠の右端から一番右のボタンまでの余白（ピクセル）
	const float m_categoryRightMargin = 30.0f;

	//種類の切り替えボタンの更新（クリックで m_nowCategory を変える）
	void CategoryUpdate();

	//種類の切り替えボタンの描画
	void CategoryDraw();

	//種類を変えたときの後始末（スクロールを戻す・先頭を選ぶ）
	void OnCategoryChange();

	//---------- スクロールバー ----------
	// 溝（レール）
	std::shared_ptr<KdTexture> m_spScrollTrackTex;

	// つまみ（上の端・真ん中・下の端・中央の3本線）
	std::shared_ptr<KdTexture> m_spScrollThumbTopTex;		// 上の端
	std::shared_ptr<KdTexture> m_spScrollThumbMidTex;		// 縦に伸ばして使う
	std::shared_ptr<KdTexture> m_spScrollThumbBottomTex;	// 下の端
	//つまみの中央の3本線の画像
	std::shared_ptr<KdTexture> m_spScrollThumbGripTex;

	// マウスを乗せたときのつまみ
	std::shared_ptr<KdTexture> m_spScrollThumbTopHoverTex;
	//マウスを乗せたときのつまみの真ん中の画像
	std::shared_ptr<KdTexture> m_spScrollThumbMidHoverTex;
	//マウスを乗せたときのつまみの下の端の画像
	std::shared_ptr<KdTexture> m_spScrollThumbBottomHoverTex;
	//マウスを乗せたときのつまみの3本線の画像
	std::shared_ptr<KdTexture> m_spScrollThumbGripHoverTex;



	// 溝（レール）
	Math::Vector2 m_scrollTrackPos;		// 溝の中心座標（マスの右側から計算）
	Math::Vector2 m_scrollTrackSize;		// 溝の横幅・縦の長さ（横幅は画像、縦はマスの段数に合わせる）

	// つまみ（上の端・真ん中・下の端）
	Math::Vector2 m_scrollThumbPos;		// つまみの中心座標（スクロールに合わせて y が動く）
	Math::Vector2 m_scrollThumbSize;		// つまみの横幅・縦の長さ（横幅は画像、縦はアイテム数で変わる）

	float m_scrollThumbCapH = 0.0f;		// つまみの上の端・下の端の画像の高さ（画像から）
	float m_scrollThumbMinH = 0.0f;		// つまみの縦の長さの最小（上の端＋3本線＋下の端が入る長さ）

	bool m_scrollThumbHoverFlg = false;	// つまみにマウスが乗っているか（ScrollBarUpdate で更新）

	// 溝のふちの太さ（つまみが動けるのは、溝からこの分だけ内側）
	const float m_scrollTrackInset = 2.0f;

	//スクロールバーの描画（溝 → つまみ）
	void ScrollBarDraw();

	//スクロールバーの更新（ホイール・ドラッグでスクロール位置を変える）
	void ScrollBarUpdate();

	//つまみの長さと位置を計算する
	void ScrollBarCalcThumb();

	//---------- スクロールの状態 ----------
	// 今いちばん上に表示している段（0 = 一番上までスクロールしている）
	// 0 〜 (全部の段数 - m_slotRows) の範囲で動く
	int m_scrollRow = 0;

	// つまみをドラッグしている最中か
	// つまみの上で左クリックを押した瞬間に true、離したら false
	// （ドラッグ中にマウスがつまみから外れても、続けて動かせるようにするため）
	bool m_scrollDragFlg = false;

	// つかんだ位置と、つまみの中心との縦のずれ（マウスのy − つまみの中心のy）
	// ドラッグ開始時に記録して、つまみの端をつかんでも中心にワープしないようにする
	float m_scrollDragOffsetY = 0.0f;

	//その高さのマスが今見えている範囲か
	// _y ... マスの中心のy
	//戻り値 ... 見えているならtrue
	bool IsInSlotView(float _y) const;
};
