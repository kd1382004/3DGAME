#pragma once
#include"../ButtonBase.h"

// 準備シーン(ショップ・装備変更)へ移動するボタン
class PrepareButton :public  ButtonBase
{
public:

	void Init()override;
	void Update()override;

	// 表示する画像を変更する(Initより前に呼ぶ)
	// 呼ばなければSTARTの画像になる
	void SetTexPath(const std::string& _path) { m_texPath = _path; }

private:

	std::string m_texPath = "Asset/Textures/Button/PrepareButton/StartButton.png";
};
