#pragma once

#include"../BaseScene/BaseScene.h"

class CharacterBase;
class MapManager;
class PlayerBase;

class GameScene : public BaseScene
{
public:

	GameScene() { Init(); }
	~GameScene() {}

	void ImGUi()override;
private:

	void Event() override;
	void Init()  override;

	std::list<std::shared_ptr<CharacterBase>>m_spCharacterStatus;
	std::shared_ptr<MapManager>m_spMapManager;
	std::shared_ptr<PlayerBase>m_spPlayer;


	void GenerateMap();
};
