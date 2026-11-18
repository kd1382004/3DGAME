#pragma once
#include"../MapBase.h"

//床の種類
enum GroundType
{
	Floor,	//床
	Room	//部屋
};


class FloorBase :public MapBase
{
public:

	//---- 基本の処理 ----

	//初期化
	void Init()override;

	//更新
	void Update()override;

	//---- 取得 ----

	//床の種類を返す
	//戻り値 ... 床の種類
	GroundType GetGroundType() { return m_groundType; }

	//---- 設定 ----

	//床の種類をセット
	// _groundType ... 床の種類
	void SetGroundType(GroundType _groundType) { m_groundType = _groundType; }

private:

	//---- メンバ変数 ----

	//床の種類
	GroundType m_groundType;
};

