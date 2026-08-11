#pragma once

class UIMap_Map
{
public:
	UIMap_Map() {};
	~UIMap_Map() {};

	void Init();
	void DrawSprit();

	void AddPosList(Math::Vector3 _3DPos,float _worldTileSize);

	void SetBasePos(Math::Vector2 _basePos) { m_basePos = _basePos; }

	void PosListReset();

	void CreateNavMap(std::vector<std::vector<int>> mapDate);

	void SetBase3DPos(Math::Vector3 _3dPos)
	{
		m_base3DPos = _3dPos;
	}
private:

	Math::Vector2 m_basePos;
	Math::Vector3 m_base3DPos;


	std::shared_ptr<KdTexture>m_mapTex;

	Math::Vector2 m_mapTexSiz;

	std::list<Math::Vector2>m_posList;

};
