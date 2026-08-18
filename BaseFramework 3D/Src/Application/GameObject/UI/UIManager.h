#pragma once

class UIBase;
class PlayerBase;
class UIMapManager;
class PlayerInventoryUI;
class GameScene;
class PotionUseController;

class UIManager :public KdGameObject
{
public:
	UIManager() {};
	~UIManager()override {};

	void Init()override;

	void PreUpdate()override;
	void Update()override;

	void PreDraw()override;
	void DrawSprite()override;
	void SetPlayer(std::shared_ptr<PlayerBase> _spPlayerBase);
	void SetGameScene(std::shared_ptr<GameScene> _spGameScene);

	void AddUIObj(std::shared_ptr<UIBase> _spUI) { m_spUIList.push_back(_spUI); }


	std::shared_ptr<UIMapManager> GetUIMapManager() { return  m_wpUIMapManager.lock(); }

	void SetPotionUseController(std::shared_ptr<PotionUseController> _spPotionUseController);

private:

	std::weak_ptr<PlayerBase> m_wpPlayerBase;
	std::weak_ptr<UIMapManager> m_wpUIMapManager;
	std::weak_ptr<GameScene> m_wpGameScene;
	std::shared_ptr<PlayerInventoryUI> m_spPlayerInventoryUI;

	std::list<std::shared_ptr<UIBase>> m_spUIList;
	int m_spUIListOldSiz = 0;
};
