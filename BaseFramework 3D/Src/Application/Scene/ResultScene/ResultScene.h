#pragma once

#include"../BaseScene/BaseScene.h"


class ResultScene : public BaseScene
{
public:

	//コンストラクタ
	ResultScene() {}
	//デストラクタ
	~ResultScene();

	//ImGuiの表示
	void ImGUI()override;
	//シーンの初期化
	void Init()  override;
private:

	//シーンのイベント(毎フレーム)
	void Event() override;

	//音
	std::shared_ptr<KdSoundInstance> m_spPlayerDeadBGM;
	//脱出したときのBGM
	std::shared_ptr<KdSoundInstance> m_spEscapeBGM;

};
