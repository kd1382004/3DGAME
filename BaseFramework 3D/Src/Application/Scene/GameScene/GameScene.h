#pragma once

#include"../BaseScene/BaseScene.h"

class CharacterBase;
class MapManager;
class PlayerBase;
class EnemyManager;
class WeaponBase;

class GameScene : public BaseScene,public std::enable_shared_from_this<GameScene>
{
public:

	GameScene() {}
	~GameScene() {}

	void ImGUi()override;

	void GenerateMap();
	void Init()  override;

private:

	void Event() override;


	std::list<std::shared_ptr<CharacterBase>>m_spCharacterStatus;
	std::shared_ptr<MapManager>m_spMapManager;
	std::shared_ptr<PlayerBase>m_spPlayer;
	std::shared_ptr<EnemyManager>m_spEnemyManager;
	std::shared_ptr<WeaponBase>m_spWeapon;



};
