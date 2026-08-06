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

	void SerRoomID(int _ID) { m_roomID = _ID; }

	int GetRoomID() { return m_roomID; }


private:

	GroundType m_groundType;

	//部屋なら部屋番号を保存
	int m_roomID = -999;



};

