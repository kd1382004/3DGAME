#pragma once
#include"../UIBase.h"

class  VOLUI :public UIBase
{
public:
	VOLUI() {};
	~VOLUI() {};

	void Init()override;
	void Update()override;
	void DrawSprite()override;

	void SetVolPos(Math::Vector2 _pos);

private:

	std::shared_ptr<KdTexture>m_volTex;
	Math::Vector2 m_volPos;
	Math::Vector2 m_volSiz;


	bool m_VolChangeOpenFlg = false;

	std::shared_ptr<KdTexture>m_volBackTex;
	Math::Vector2 m_volBackPos;
	Math::Vector2 m_volBackSiz;


	struct VolInfo
	{
		std::shared_ptr<KdTexture>m_volTex;
		Math::Vector2 m_volPos;
		Math::Vector2 m_volSiz;

		std::shared_ptr<KdTexture>m_volValTex;
		Math::Vector2 m_volValPos;
		Math::Vector2 m_volVaSiz;

		float vol = 0;

		// つまみにマウスが乗っているか（明るい画像にする）
		bool thumbHoverFlg = false;
	};



	static const int m_volInfoSiz = 3;
	VolInfo m_volInfo[m_volInfoSiz];

	//---------- つまみ（インベントリのスクロールバーと同じ操作） ----------
	// つまみの画像（通常・マウスを乗せたとき）
	std::shared_ptr<KdTexture> m_volThumbTex;
	std::shared_ptr<KdTexture> m_volThumbHoverTex;
	Math::Vector2 m_volThumbSiz;

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
	float VolToThumbY(int _index) const;

	// つまみの中心のy → 音量（0〜1）
	float ThumbYToVol(int _index, float _y) const;

	// つまみの更新（ドラッグ・バーのクリック・ホイール）
	void VolThumbUpdate();

};
