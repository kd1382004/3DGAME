#pragma once


struct MineMapInfo
{
	Math::Vector2 m_pos;
	Math::Vector2 m_tileIndex;
	bool m_drawFlg = false;
};

class UIMap_Map
{
public:
	UIMap_Map() {};
	~UIMap_Map() {};

	void Init();
	void DrawSprit();

	void AddPosList(Math::Vector3 _3DPos,float _worldTileSize);

	void AddStairsPos(Math::Vector3 _3DPos,float _worldTileSize);

	void SetBasePos(Math::Vector2 _basePos) { m_basePos = _basePos; }

	void PosListReset();

	void SetBase3DPos(Math::Vector3 _3dPos)
	{
		m_base3DPos = _3dPos;
	}

	void SetMinMapPlayerPos(Math::Vector2 _pos) { m_minMapPlayerPos = _pos; }


	const std::list<MineMapInfo>& GetMineMapInfo() const { return m_posList; }

	Math::Vector2 GetMapTexSiz() { return m_mapTexSiz; }
private:

	float m_siz = 1.0f;

	Math::Vector2 m_basePos;
	Math::Vector3 m_base3DPos;


	std::shared_ptr<KdTexture>m_mapTex;
	std::shared_ptr<KdTexture>m_mapStairsTex;

	Math::Vector2 m_mapTexSiz;

	Math::Vector2 m_minMapPlayerPos;


	int m_discoverTileNum = 3;
	void DiscoverTile();


	std::list<MineMapInfo>m_posList;


	MineMapInfo m_StairsMineMapInfo;
};
