#pragma once
#include"../MapBase.h"



class StairsBase :public MapBase
{
public:
	StairsBase() {};
	~StairsBase() override{};

	void Init()override;
	void Update()override;

	void IsHit()override;

private:

	//プレイヤーが乗ってるかどうか
	bool m_playerHit=false;

};
