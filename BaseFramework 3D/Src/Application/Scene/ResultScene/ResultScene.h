#pragma once

#include"../BaseScene/BaseScene.h"


//リザルトシーン
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

	//BGMの音量を今の設定に合わせる(ポーズ中も呼ばれる)
	void ApplyBGMVolume() override;

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
