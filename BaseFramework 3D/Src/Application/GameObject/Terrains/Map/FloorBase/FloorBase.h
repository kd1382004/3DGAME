#pragma once
#include"../MapBase.h"

enum GroundType
{
	Floor,	//床
	Room	//部屋
};


class FloorBase :public MapBase
{
public:

	void Init()override;
	void Update()override;

	void SetGroundType(GroundType _groundType) { m_groundType = _groundType; }

	GroundType GetGroundType() { return m_groundType; }






private:

	GroundType m_groundType;




};

