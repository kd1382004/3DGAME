#pragma once

class UIBase :public KdGameObject
{
public:
	UIBase() {};
	~UIBase() {};

	virtual void Init()override;

	virtual void Update()override;

	virtual void PreDraw()override;
	virtual void DrawSprite()override;

	virtual void Set2DPos(const Math::Vector2& pos) { m_2DPos=pos; }
protected:
	Math::Vector2 m_2DPos;

};