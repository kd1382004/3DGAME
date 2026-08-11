#pragma once

class UIMap_Player
{
public:
	UIMap_Player() {};
	~UIMap_Player() {};

	void Init();
	void DrawSprit();


	void SetPlayer3DPos(Math::Vector3 _3dPos)
	{
		m_player3DPos = _3dPos;
	}

	void SetBase3DPos(Math::Vector3 _3dPos)
	{
		m_base3DPos = _3dPos;
	}

	void SetBasePos(Math::Vector2 _basePos) { m_basePos= _basePos; }


	void SetTileSiz(float _siz) { m_tileSiz = _siz; }

	void SetAngle(float _angle) { m_angle = _angle; }
private:

	float m_angle;

	float m_tileSiz;

	Math::Vector2 m_basePos;

	std::shared_ptr<KdTexture>m_playerTex;

	Math::Vector3 m_player3DPos;

	Math::Vector2 m_player2DPos;

	Math::Vector3 m_base3DPos;

	void Change2DPos();
};
