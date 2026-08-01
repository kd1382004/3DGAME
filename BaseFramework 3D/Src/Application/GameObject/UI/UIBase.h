#pragma once

class UIBase :public KdGameObject
{
public:
	UIBase() {};
	~UIBase() {};

	virtual void Init()override;

	virtual void Update()override;

	virtual void DrawSprite()override;

protected:
	Math::Vector2 m_2DPos;

};