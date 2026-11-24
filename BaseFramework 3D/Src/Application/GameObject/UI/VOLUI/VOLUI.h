#pragma once
#include"../UIBase.h"

//音量設定画面
class  VOLUI :public UIBase
{
public:
	//コンストラクタ
	VOLUI() {};
	//デストラクタ
	~VOLUI() {};

	//初期化
	void Init()override;
	//更新
	void Update()override;
	//2Dの描画
	void DrawSprite()override;

	//音量UIの位置をセット
	// _pos ... 位置(画面)
	void SetVolPos(Math::Vector2 _pos);

private:

	//音量アイコンの画像
	std::shared_ptr<KdTexture>m_spVolTex;
	//音量アイコンの位置
	Math::Vector2 m_volPos;
	//音量アイコンの大きさ
	Math::Vector2 m_volSize;


	//音量の設定を開いているか
	bool m_volChangeOpenFlg = false;

	//背景の画像
	std::shared_ptr<KdTexture>m_spVolBackTex;
	//背景の位置
	Math::Vector2 m_volBackPos;
	//背景の大きさ
	Math::Vector2 m_volBackSize;


	//音量設定の項目1つ分(項目の名前・音量バー)の表示情報
	struct VolInfo
	{
		//項目の名前の画像
		std::shared_ptr<KdTexture>m_spVolTex;
		//項目の名前の位置
		Math::Vector2 m_volPos;
		//項目の名前の大きさ
		Math::Vector2 m_volSize;

		//音量バーの画像
		std::shared_ptr<KdTexture>m_spVolValTex;
		//音量バーの位置
		Math::Vector2 m_volValPos;
		//音量バーの大きさ
		Math::Vector2 m_volValSize;

		//音量(0〜1)
		float m_vol = 0;

		// つまみにマウスが乗っているか（明るい画像にする）
		bool m_thumbHoverFlg = false;
	};



	//音量の項目の数
	static const int kVolInfoSize = 3;
	//音量の項目
	VolInfo m_volInfo[kVolInfoSize];

	//---------- つまみ（インベントリのスクロールバーと同じ操作） ----------
	// つまみの画像（通常・マウスを乗せたとき）
	std::shared_ptr<KdTexture> m_spVolThumbTex;
	//つまみの画像(マウスを乗せたとき)
	std::shared_ptr<KdTexture> m_spVolThumbHoverTex;
	//つまみの大きさ
	Math::Vector2 m_volThumbSize;

	// ドラッグしているバーの番号（-1 = ドラッグしていない）
	// 押した瞬間に決めて、離すまで変えない（ドラッグ中にバーから外れても動かせるように）
	int m_volDragIndex = -1;

	// つかんだ位置と、つまみの中心との縦のずれ（マウスのy − つまみの中心のy）
	// つまみの端をつかんでも中心にワープしないようにする
	float m_volDragOffsetY = 0.0f;

	// バーのふちの太さ（つまみが動けるのは、バーからこの分だけ内側）
	const float m_volBarInset = 4.0f;

	// ホイール1回で変わる音量
	const float m_volWheelStep = 0.05f;

	// 音量 → つまみの中心のy
	// _index ... 音量の項目の番号
	float VolToThumbY(int _index) const;

	// つまみの中心のy → 音量（0〜1）
	// _index ... 音量の項目の番号
	// _y ... つまみの中心のy
	float ThumbYToVol(int _index, float _y) const;

	// つまみの更新（ドラッグ・バーのクリック・ホイール）
	void VolThumbUpdate();

};
