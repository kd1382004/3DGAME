#pragma once

#include"../PressBot/PressBot.h"

//敵(溶岩の剛鬼・火山帯)
//背中に小さな火口を背負った岩の巨体。動き・攻撃・ステータスはプレスボットと同じ
class MagmaBrute :public PressBot
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	MagmaBrute() {};

	//デストラクタ
	~MagmaBrute()override {};

	//---- 基本の処理 ----

	//初期化
	void Init()override;

protected:

	//---- 取得 ----

	//使うモデルのパス
	std::string GetModelPath() const override { return "Asset/Models/Character/Enemy/Volcano/MagmaBrute/MagmaBrute.gltf"; }
};
