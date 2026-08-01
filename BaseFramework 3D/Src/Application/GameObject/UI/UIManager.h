#pragma once

class UIBase;
class PlayerBase;

class UIManager:public KdGameObject
{
public:
	UIManager() {};
	~UIManager()override {};

	void Init()override;

	void Update()override;

	void DrawSprite()override;

	void SetPlayer(std::shared_ptr<PlayerBase> _spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }
private:

	std::weak_ptr<PlayerBase> m_wpPlayerBase;

	std::list<std::shared_ptr<UIBase>> m_spUIList;

};
