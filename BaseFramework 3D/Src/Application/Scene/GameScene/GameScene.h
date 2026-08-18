#pragma once

#include"../BaseScene/BaseScene.h"

class CharacterBase;
class MapManager;
class PlayerBase;
class EnemyManager;
class WeaponBase;
class TreasureChestManager;
class PotionUseController;

class GameScene : public BaseScene,public std::enable_shared_from_this<GameScene>
{
public:

	GameScene() {}
	~GameScene() {}

	void ImGUi()override;

	void GenerateMap();
	void Init()  override;


	std::shared_ptr<PotionUseController> GetPotionUseController() { return m_spPotionUseController; };
private:

	void Event() override;


	std::list<std::shared_ptr<CharacterBase>>m_spCharacterStatus;
	std::shared_ptr<MapManager>m_spMapManager;
	std::shared_ptr<PlayerBase>m_spPlayer;
	std::shared_ptr<EnemyManager>m_spEnemyManager;
	std::shared_ptr<WeaponBase>m_spWeapon;
	std::shared_ptr<TreasureChestManager>m_spTreasureChestManager;
	std::shared_ptr<PotionUseController>m_spPotionUseController;



	//階層
	int m_displayFloor;

	//ボス戦になる倍数(割り切れたらボス戦)
	int m_bossInterval = 3;

	//マップ成長率
	int m_mapLinearGrowthPerFloor;
	float m_mapLinearGrowthPerFloorX;
	float m_mapLinearGrowthPerFloorY;
};
