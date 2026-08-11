#pragma once

#include"../UIBase.h"

class UIMap_Map;
class UIMap_Player;
class UIMap_EnemyManage;
class UIMapManager:public UIBase
{
public:
	UIMapManager() {};
	~UIMapManager()override {};

	virtual void Init()override;

	virtual void Update()override;

	virtual void PreDraw()override;
	virtual void DrawSprite()override;

	void SetBase3DPos(Math::Vector3 _base3DPos);


	void SetTileSiz(float _siz);

	std::shared_ptr<UIMap_Map> GetUIMap_Map() {return m_UIMap_Map;}
	std::shared_ptr<UIMap_Player> GetUIMap_Player() {return m_UIMap_Player;}
	std::shared_ptr<UIMap_EnemyManage> GetUIMap_Enemy() {return m_UIMap_Enemy;}
private:

	std::shared_ptr<UIMap_Map>m_UIMap_Map;
	std::shared_ptr<UIMap_Player>m_UIMap_Player;
	std::shared_ptr<UIMap_EnemyManage>m_UIMap_Enemy;


	Math::Vector2 m_basePos;
	Math::Vector3 m_base3DPos;

	float m_tileSiz;
};
