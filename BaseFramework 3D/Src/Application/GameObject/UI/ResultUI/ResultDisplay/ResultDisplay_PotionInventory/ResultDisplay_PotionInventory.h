#pragma once
#include"../ResultDisplayBase.h"


class PotionTexInfo;
struct Inventory;

//////////////////////////////////////////////////////////////
// リザルト：アイテム（ポーション）画面
// インベントリ（PotionInventoryUI）と同じ見た目・操作
// （空きマス・スクロールバー・ホイール／ドラッグ）。使用ボタンはなし
//////////////////////////////////////////////////////////////
class ResultDisplay_PotionInventory :public ResultDisplayBase
{
public:
	//コンストラクタ
	ResultDisplay_PotionInventory() {}
	//デストラクタ
	~ResultDisplay_PotionInventory() {}


	//初期化
	void Init();

	//更新
	void Update();

	//描画前の処理
	void PreDraw();
	//2Dの描画
	void DrawSprite();


	//あるものを入れてく関数
	// _potions ... 持っているポーションの一覧
	void AddPotionTexInfo(std::vector<Inventory>_potions);
private:

	//アイコンとマウスが当たってるか
	void IconHit();

	//スクロールした段数に合わせてアイコンの位置を決める
	void UpdateIconPos();

	//リザルトに並べるアイテムのアイコン1つ分の情報
	struct ItemIconInfo
	{
		//アイテムの番号
		int m_itemID = 0;
		//表示位置
		Math::Vector2 m_2DPos;
		//所持数
		int m_num = 0;
		//マウスが乗っているか
		bool m_hit = false;

		// 何番目のアイテムか（マスの位置を決めるのに使う）
		int m_index = 0;

		//アイテムの名前
		std::string m_name;
		//アイコンの画像
		std::shared_ptr<KdTexture>m_spIconTex;
		//説明の画像
		std::shared_ptr<KdTexture>m_spExplanationTex;
		//名前の画像
		std::shared_ptr<KdTexture> m_spNameTex;
	};


	//表示するアイテムのアイコン一覧
	std::vector<ItemIconInfo> m_itemIconInfo;
	//アイコンの大きさ
	struct { int m_w; int m_h; } m_iconDimensions = { 40,40 };

	//選ばれてるポーションのID
	int m_selectPotionID = -999;

	//ポーションselect（空きマスにも使う）
	std::shared_ptr<KdTexture>m_spPotionSelectTex;


	//---------- 背景・タイトル ----------
	std::shared_ptr<KdTexture>m_spPotionTitleTex;
	//タイトルの位置
	Math::Vector2 m_potionTitleTexPos;

	std::shared_ptr<KdTexture>m_spPotionBackTex;		// 右の枠（名前・説明）
	std::shared_ptr<KdTexture>m_spPotionBack2Tex;		// 左の枠（アイテム一覧）
	Math::Vector2 m_potionBackTexPos;				// 右の枠の位置
	Math::Vector2 m_potionBack2TexPos;				// 左の枠の位置


	//---------- アイテム一覧のマス（スロット） ----------
	// 横に並べるマスの数（この数を超えたら次の段へ）
	const int m_slotCols = 6;

	// 縦に並べるマスの数（空きマスを何段分描くか）
	const int m_slotRows = 6;

	// 一番左上のマスの位置
	const Math::Vector2 m_slotBasePos = { -285, 55 };

	// マスとマスのすき間（ピクセル）
	const float m_slotSpacing = 10.0f;


	//---------- スクロールバー ----------
	// 溝（レール）
	std::shared_ptr<KdTexture> m_spScrollTrackTex;

	// つまみ（上の端・真ん中・下の端・中央の3本線）
	std::shared_ptr<KdTexture> m_spScrollThumbTopTex;
	std::shared_ptr<KdTexture> m_spScrollThumbMidTex;		// 縦に伸ばして使う
	std::shared_ptr<KdTexture> m_spScrollThumbBottomTex;
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
	Math::Vector2 m_scrollTrackSiz;		// 溝の横幅・縦の長さ（横幅は画像、縦はマスの段数に合わせる）

	// つまみ
	Math::Vector2 m_scrollThumbPos;		// つまみの中心座標（スクロールに合わせて y が動く）
	Math::Vector2 m_scrollThumbSiz;		// つまみの横幅・縦の長さ（横幅は画像、縦はアイテム数で変わる）

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
	int m_scrollRow = 0;

	// つまみをドラッグしている最中か
	bool m_scrollDragFlg = false;

	// つかんだ位置と、つまみの中心との縦のずれ（マウスのy − つまみの中心のy）
	float m_scrollDragOffsetY = 0.0f;

	//その高さのマスが今見えている範囲か
	// _y ... マスの中心のy
	//戻り値 ... 見えているならtrue
	bool IsInSlotView(float _y) const;
};
