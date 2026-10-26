#pragma once
#include"../UIBase.h"

enum DrawKeyPos
{
	DrawKeyPos_Left,
	DrawKeyPos_Right,
	DrawKeyPos_Top,
	DrawKeyPos_Bot,
};

class UIBase2 :public UIBase
{
public:
	UIBase2() {};
	virtual ~UIBase2() {};

	virtual void Update()override;

	virtual void DrawSprite()override;

	virtual void Set2DPos(const Math::Vector2& pos) { m_2DPos = pos; };

	int GetLayerPriority() { return m_layerPriority; };

	void SetDrawKey(int _key) { m_DrawKey = _key; }

	void SetTex(std::shared_ptr<KdTexture>_tex) { m_tex = _tex; }

	void SetDrawFlg(bool _flg) { m_drwaFlg = _flg; }

	void SetDrawKeyPos(DrawKeyPos _drawKeyPos) {m_DrawKeyPos = _drawKeyPos;}
protected:


	bool m_drwaFlg = false;
	int m_DrawKey = 0;
	std::shared_ptr<KdTexture>m_tex;

	Math::Vector2 m_2DPos;

	DrawKeyPos m_DrawKeyPos = DrawKeyPos_Left;

	//描画優先度
	//数字が小さいほうが優先度高い
	int m_layerPriority = INT_MAX;

	//アルファ
	float m_alpha = 1;
	float m_addAlpha = 0.5;
};