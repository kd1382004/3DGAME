#pragma once

#include"../PressBot/PressBot.h"

//敵(骨の巨躯・黄昏の霊廟)
//無数の骨が寄り集まった獣の頭の巨躯。動き・攻撃・ステータスはプレスボットと同じ
class BoneBehemoth :public PressBot
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	BoneBehemoth() {};

	//デストラクタ
	~BoneBehemoth()override {};

	//---- 基本の処理 ----

	//初期化
	void Init()override;

protected:

	//---- 取得 ----

	//使うモデルのパス
	std::string GetModelPath() const override { return "Asset/Models/Character/Enemy/Mausoleum/BoneBehemoth/BoneBehemoth.gltf"; }
};