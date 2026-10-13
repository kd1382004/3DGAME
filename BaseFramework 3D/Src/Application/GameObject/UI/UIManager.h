#pragma once

class UIBase;
class PlayerBase;
class UIMapManager;
class PlayerInventoryUI;
class GameScene;
class PotionUseController;
class PotionTexInfo;
class UIBase2;

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
	std::shared_ptr<PotionTexInfo> GetPotionTexInfo() { return  m_spPotionTexInfo; }
	std::shared_ptr<UIBase2> GetUIBase2() { return  m_UIBase2; }

	void SetPotionUseController(std::shared_ptr<PotionUseController> _spPotionUseController);


	//マップUIマネージャー
	void AddUIMapManager();

	//プレイヤーのインベントリ
	void AddPlayerInventoryUI();

	//ポーションの画像一覧
	void SetpPotionTexInfo();

	void SetMinimapRevealAll(bool enable);


	std::shared_ptr<KdTexture> GetTresureChestUiTex() { return m_tresureChestUiTex; }
private:

	std::weak_ptr<PlayerBase> m_wpPlayerBase;
	std::weak_ptr<UIMapManager> m_wpUIMapManager;
	std::weak_ptr<GameScene> m_wpGameScene;
	std::shared_ptr<PlayerInventoryUI> m_spPlayerInventoryUI;
	std::shared_ptr<PotionTexInfo> m_spPotionTexInfo;
	std::shared_ptr<UIBase2> m_UIBase2;

	std::list<std::shared_ptr<UIBase>> m_spUIList;
	int m_spUIListOldSiz = 0;



	//宝箱用テクスチャ
	std::shared_ptr<KdTexture>m_tresureChestUiTex;
};
