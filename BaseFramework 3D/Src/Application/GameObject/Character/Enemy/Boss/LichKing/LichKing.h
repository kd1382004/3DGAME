#pragma once

#include"../Titan/Titan.h"

//ボス(死霊の王・黄昏の霊廟)
//鉄の王冠とぼろぼろの法衣をまとった巨大な骸骨の王。動き・攻撃・ステータスはタイタンと同じ
class LichKing :public Titan
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	LichKing() {};

	//デストラクタ
	~LichKing()override {};

	//---- 基本の処理 ----

	//初期化
	void Init()override;

protected:

	//---- 取得 ----

	//使うモデルのパス
	std::string GetModelPath() const override { return "Asset/Models/Character/Enemy/Mausoleum/LichKing/LichKing.gltf"; }
};