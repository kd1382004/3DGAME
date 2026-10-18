#pragma once

#include"../BaseScene/BaseScene.h"


class ResultScene : public BaseScene
{
public:

	ResultScene() {}
	~ResultScene();

	void ImGUi()override;
	void Init()  override;
private:

	void Event() override;

	//音
	std::shared_ptr<KdSoundInstance> m_spPlayerDeadBGM;
	std::shared_ptr<KdSoundInstance> m_spEscapeBGM;

};
