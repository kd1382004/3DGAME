#pragma once

#include"../ScrapBot/ScrapBot.h"

//敵(骸骨兵・黄昏の霊廟)
//錆びた剣と盾、鉄兜を身に着けた骸骨の兵士。動き・攻撃・ステータスはスクラップボットと同じ
class SkeletonWarrior :public ScrapBot
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	SkeletonWarrior() {};

	//デストラクタ
	~SkeletonWarrior()override {};

	//---- 基本の処理 ----

	//初期化
	void Init()override;

protected:

	//---- 取得 ----

	//使うモデルのパス
	std::string GetModelPath() const override { return "Asset/Models/Character/Enemy/Mausoleum/SkeletonWarrior/SkeletonWarrior.gltf"; }
};