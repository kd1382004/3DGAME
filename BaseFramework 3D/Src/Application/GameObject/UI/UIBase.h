#pragma once

class UIBase :public KdGameObject
{
public:
	UIBase() {};
	virtual ~UIBase() {};

	virtual void Init()override;

	virtual void Update()override;

	virtual void PreDraw()override;
	virtual void DrawSprite()override;

	virtual void Set2DPos(const Math::Vector2& pos) { m_2DPos = pos; };

	int GetLayerPriority() { return m_layerPriority; };

protected:
	Math::Vector2 m_2DPos;


	//描画優先度
	//数字が小さいほうが優先度高い
	int m_layerPriority = INT_MAX;
};