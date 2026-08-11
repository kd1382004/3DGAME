#pragma once

class UIMap_Map;

class UIMap_EnemyManage
{
public:
	UIMap_EnemyManage() {};
	~UIMap_EnemyManage() {};

	void Init();
	void DrawSprit();


	void SetEnemy3DPos(Math::Vector3 _3dPos)
	{
		m_enemy3DPos = _3dPos;
	}

	void SetBase3DPos(Math::Vector3 _3dPos)
	{
		m_base3DPos = _3dPos;
	}

	void PosListRiseto();

	void SetBasePos(Math::Vector2 _basePos) { m_basePos= _basePos; }


	void SetTileSiz(float _siz) { m_tileSiz = _siz; }

	void SetAngle(float _angle) { m_angle = _angle; }

	void SetEnemyPosAngleList(Math::Vector3 _pos,float _angle);

	void SetUIMap_Map(std::shared_ptr<UIMap_Map>_spUIMap_Map) { m_wpUIMap_Map = _spUIMap_Map; }

	void SetMapTexSiz(Math::Vector2 _siz) { m_mapTexSiz = _siz; }
private:

	Math::Vector2 m_mapTexSiz = { };

	float m_angle;

	float m_tileSiz;

	Math::Vector2 m_basePos;

	std::shared_ptr<KdTexture>m_enemyTex;

	Math::Vector3 m_enemy3DPos;

	Math::Vector2 m_enemy2DPos;

	Math::Vector3 m_base3DPos;

	Math::Vector2 Change2DPos(Math::Vector3 _pos);

	struct List
	{
		Math::Vector3 m_pos;
		float m_angle;
		bool m_drawFlg = false;
	};
	

	std::list<List> m_posList;

	std::weak_ptr<UIMap_Map>m_wpUIMap_Map;

	void DiscoverEnemy(List* _info);


	float m_siz = 1.0f;
};
