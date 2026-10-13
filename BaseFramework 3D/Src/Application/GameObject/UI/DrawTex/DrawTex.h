#pragma once
#include"../UIBase.h"

class DrawTex :public UIBase
{
public:
	DrawTex() {};
	virtual ~DrawTex() {};

	virtual void Update()override;

	virtual void DrawSprite()override;

	virtual void Set2DPos(const Math::Vector2& pos) { m_2DPos = pos; };

	int GetLayerPriority() { return m_layerPriority; };

	void LoadTex(std::string path);

protected:

	std::shared_ptr<KdTexture>m_tex;

};