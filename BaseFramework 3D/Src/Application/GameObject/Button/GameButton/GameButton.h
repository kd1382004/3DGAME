#pragma once
#include"../ButtonBase.h"

class GameButton:public  ButtonBase
{
public:

	void Init()override;
	void Update()override;

	// ボタンの画像を変える(Initより前に呼ぶ。呼ばなければ PlayButton.png)
	void SetTexPath(const std::string& _path) { m_texPath = _path; }

private:

	std::string m_texPath = "Asset/Textures/Button/GameButton/PlayButton.png";
};
