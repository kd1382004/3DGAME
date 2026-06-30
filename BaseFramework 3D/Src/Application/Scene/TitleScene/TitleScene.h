#pragma once

#include"../BaseScene/BaseScene.h"

class TitleSceneEditor;

class TitleScene : public BaseScene
{
public :

	TitleScene()  { Init(); }
	~TitleScene() {}

	void ImGUi()override;

private :

	void Event() override;
	void Init()  override;

	std::shared_ptr<TitleSceneEditor>m_spTitleSceneEditor;
};
