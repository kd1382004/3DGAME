#pragma once
#include"../MapBase.h"



class StairsBase :public MapBase
{
public:
	StairsBase() {};
	~StairsBase() override;

	void Init()override;
	void Update()override;

	void PostUpdate()override;

	void IsHit()override;

private:

	//プレイヤーが乗ってるかどうか
	bool m_playerHit=false;

	std::weak_ptr<KdEffekseerObject> m_wpAuraEffect;

	Math::Vector3 m_effectLocalPos;
};
