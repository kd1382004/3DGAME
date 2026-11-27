#pragma once

#include"../Titan/Titan.h"

//ボス(火山の巨神・火山帯)
//黒曜石の鎧と両肩の火口を持つ巨神。動き・攻撃・ステータスはタイタンと同じ
class VolcanoColossus :public Titan
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	VolcanoColossus() {};

	//デストラクタ
	~VolcanoColossus()override {};

	//---- 基本の処理 ----

	//初期化
	void Init()override;

protected:

	//---- 取得 ----

	//使うモデルのパス
	std::string GetModelPath() const override { return "Asset/Models/Character/Enemy/Volcano/VolcanoColossus/VolcanoColossus.gltf"; }
};
