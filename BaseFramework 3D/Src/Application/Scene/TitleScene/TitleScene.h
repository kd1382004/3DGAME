#pragma once

#include"../BaseScene/BaseScene.h"

class TitleSceneEditor;
class MapManager;
class TPSCamera;
class TitelMove;
class MapObjManager;

class TitleScene : public BaseScene
{
public :

	TitleScene()  {}
	~TitleScene() {}

	void ImGUi()override;
	void Init()  override;
private :

	void Event() override;

	std::shared_ptr<TPSCamera> m_spCamera;

	std::shared_ptr<MapManager>m_spMapManager;
	std::shared_ptr<TitleSceneEditor>m_spTitleSceneEditor;
	std::shared_ptr<TitelMove> m_spTitelMove;
	std::shared_ptr<MapObjManager>m_spMapObjManager;
	KdGameObjectFactory g_factory;


	void SetObjeList();


	std::shared_ptr<KdGameObject> m_dummy;
};
