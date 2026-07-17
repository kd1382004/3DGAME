#pragma once

#include"../BaseScene/BaseScene.h"

class CharacterBase;

class GameScene : public BaseScene
{
public :

	GameScene()  { Init(); }
	~GameScene() {}

	void ImGUi()override;
private:

	void Event() override;
	void Init()  override;

	std::list<std::shared_ptr<CharacterBase>>m_spCharacterStatus;
};
