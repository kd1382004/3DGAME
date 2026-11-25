#pragma once
#include"../UIBase.h"

class FloorTitleUI :public UIBase
{
public:
	FloorTitleUI() {};
	~FloorTitleUI() {};


	void Init()override;
	void Update()override;
	void DrawSprite()override;

	//表示を始める
	// _floor   ... 階層
	// _mapType ... マップの種類(MapType を int にしたもの。マップ名の画像を選ぶ)
	void Show(int _floor, int _mapType)
	{
		m_active = true;
		m_floor = _floor;
		m_mapType = _mapType;
		m_timer = 0;
	}

	bool IsFinished() const { return (m_timer >= kShowTime); }

	void Hide()
	{
		m_active = false;
	}

private:
	bool m_active = false;
	int m_floor = 0;
	float m_timer = 0;
	std::shared_ptr<KdTexture> m_spBlackTex;
	std::shared_ptr<KdTexture> m_spFloorTex;

	//マップ名の画像(MapType の順番に並べる)
	std::vector<std::shared_ptr<KdTexture>> m_spMapNameTexList;
	//今表示しているマップの種類
	int m_mapType = 0;
	//マップ名を出す高さ(画面の中心から上へ)
	const float kMapNamePosY = 150.0f;

	float m_alpha = 0;

	const float kShowTime = 1.6f;
	const float kTextFade = 0.5f;
};
