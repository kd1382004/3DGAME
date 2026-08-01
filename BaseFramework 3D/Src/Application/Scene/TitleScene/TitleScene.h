#pragma once

#include"../BaseScene/BaseScene.h"

class TitleSceneEditor;

class TitleScene : public BaseScene
{
public :

	TitleScene()  {}
	~TitleScene() {}

	void ImGUi()override;
	void Init()  override;
private :

	void Event() override;


	std::shared_ptr<TitleSceneEditor>m_spTitleSceneEditor;

	KdGameObjectFactory g_factory;


	void SetObjeList();
};
