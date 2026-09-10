#pragma once

#pragma once


class TreasureChest;
class PlayerBase;
class CameraBase;
class UIManager;
class UIMap_TreasureChest;
class LootTableManager;

class TreasureChestManager :public KdGameObject
{
public:
	TreasureChestManager() {};
	~TreasureChestManager() override {};

	void Init()override;
	void PreUpdate()override;
	void Update()override;
	void PostUpdate()override;
	void GenerateDepthMapFromLight()override;
	void DrawLit()override;
	void PreDraw()override;

	void GenerateTreasureChest(std::list<Math::Vector3> _treasureChestPosList);


	void SetPlayer(std::shared_ptr<PlayerBase> _spPlayer) { m_wpPlayer = _spPlayer; };
	void SetCamera(std::shared_ptr<CameraBase> _spCamera) { m_wpCamera = _spCamera; };
	void SetUIManager(std::shared_ptr<UIManager> _spUIManager) { m_wpUIManager = _spUIManager; };

	//宝箱とあたり判定したい対象
	void TreasureChestHit(const std::shared_ptr<KdGameObject>& obj);

	void TreasureChestReset() { m_treasureChestList.clear(); }
private:
	std::weak_ptr<PlayerBase>m_wpPlayer;
	std::weak_ptr<CameraBase>m_wpCamera;
	std::weak_ptr<UIManager>m_wpUIManager;
	std::weak_ptr<UIMap_TreasureChest>m_wpUIMap_TreasureChest;

	std::shared_ptr<LootTableManager>m_spLootTableManager;

	std::list<std::shared_ptr<TreasureChest>> m_treasureChestList;
	};
