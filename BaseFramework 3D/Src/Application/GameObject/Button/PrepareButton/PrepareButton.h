#pragma once
#include"../ButtonBase.h"

// 準備シーン(ショップ・装備変更)へ移動するボタン
class PrepareButton :public  ButtonBase
{
public:

	//---- 基本の処理 ----

	//初期化
	void Init()override;

	//更新(押されたら準備シーンへ)
	void Update()override;

	//---- 設定 ----

	// 表示する画像を変更する(Initより前に呼ぶ)
	// 呼ばなければSTARTの画像になる
	// _path ... 画像のパス
	void SetTexPath(const std::string& _path) { m_texPath = _path; }

private:

	//---- メンバ変数 ----

	//ボタンの画像のパス
	std::string m_texPath = "Asset/Textures/Button/PrepareButton/StartButton.png";
};
