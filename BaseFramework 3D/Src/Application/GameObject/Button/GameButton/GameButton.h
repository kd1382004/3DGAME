#pragma once
#include"../ButtonBase.h"

//ゲームシーンへ進むボタン
class GameButton:public  ButtonBase
{
public:

	//---- 基本の処理 ----

	//初期化
	void Init()override;

	//更新(押されたらゲームシーンへ)
	void Update()override;

	//---- 設定 ----

	// ボタンの画像を変える(Initより前に呼ぶ。呼ばなければ PlayButton.png)
	// _path ... 画像のパス
	void SetTexPath(const std::string& _path) { m_texPath = _path; }

	// 押されたときに呼ぶ関数をセット
	// _func ... 呼ぶ関数
	void SetOnPush(std::function<void()> _func) { m_onPush = _func; }
private:

	//---- メンバ変数 ----

	//ボタンの画像のパス
	std::string m_texPath = "Asset/Textures/Button/GameButton/PlayButton.png";


	//押されたときに呼ぶ関数
	std::function<void()> m_onPush;
};
