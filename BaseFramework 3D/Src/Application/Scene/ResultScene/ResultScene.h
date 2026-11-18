#pragma once

#include"../BaseScene/BaseScene.h"


class ResultScene : public BaseScene
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	ResultScene() {}

	//デストラクタ
	~ResultScene();

	//---- 基本の処理 ----

	//ImGuiの表示
	void ImGUI()override;

	//シーンの初期化
	void Init()  override;

private:

	//---- メンバ変数 ----

	//音
	std::shared_ptr<KdSoundInstance> m_spPlayerDeadBGM;

	//脱出したときのBGM
	std::shared_ptr<KdSoundInstance> m_spEscapeBGM;

	//---- その他の処理 ----

	//シーンのイベント(毎フレーム)
	void Event() override;
};
