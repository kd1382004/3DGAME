#pragma once

#include"../ScrapBot/ScrapBot.h"

//敵(燼鬼・火山帯)
//冷えた溶岩の体に黒曜石の角と爪を持つ小鬼。動き・攻撃・ステータスはスクラップボットと同じ
class CinderImp :public ScrapBot
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	CinderImp() {};

	//デストラクタ
	~CinderImp()override {};

	//---- 基本の処理 ----

	//初期化
	void Init()override;

protected:

	//---- 取得 ----

	//使うモデルのパス
	std::string GetModelPath() const override { return "Asset/Models/Character/Enemy/Volcano/CinderImp/CinderImp.gltf"; }
};
