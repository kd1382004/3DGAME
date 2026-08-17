#pragma once
#include"../UIBase.h"

class GameScene;

class PlayerInventoryUI:public UIBase
{
public:
	PlayerInventoryUI() {};
	~PlayerInventoryUI()override {};

	virtual void Init()override;

	virtual void Update()override;

	virtual void PreDraw()override;
	virtual void DrawSprite()override;
	void SetGameScene(std::shared_ptr<GameScene> _spGameScene) { m_wpGameScene = _spGameScene; }

private:

	std::weak_ptr<GameScene> m_wpGameScene;

	//インベントリが開いてるかどうか
	bool m_playerInventoryUIFlg;


	std::shared_ptr<KdTexture>m_back1Tex;
	Math::Vector2 m_back1Tex2DPos;


	std::shared_ptr<KdTexture>m_back2Tex;
	Math::Vector2 m_back2Tex2DPos;

};
