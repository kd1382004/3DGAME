#pragma once

class UIBase;
class PlayerBase;
class UIMapManager;

class UIManager:public KdGameObject
{
public:
	UIManager() {};
	~UIManager()override {};

	void Init()override;

	void Update()override;

	void PreDraw()override;
	void DrawSprite()override;
	void SetPlayer(std::shared_ptr<PlayerBase> _spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }


	void AddUIObj(std::shared_ptr<UIBase> _spUI) { m_spUIList.push_back(_spUI); }


	std::shared_ptr<UIMapManager> GetUIMapManager() { return  m_wpUIMapManager.lock(); }
private:

	std::weak_ptr<PlayerBase> m_wpPlayerBase;
	std::weak_ptr<UIMapManager> m_wpUIMapManager;

	std::list<std::shared_ptr<UIBase>> m_spUIList;

};
