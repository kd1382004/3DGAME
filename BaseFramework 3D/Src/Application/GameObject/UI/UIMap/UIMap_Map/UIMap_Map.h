#pragma once


struct MineMapInfo
{
	Math::Vector2 m_pos;
	Math::Vector2 m_tileIndex;
	bool m_drawFlg = false;

	int m_LoomNum = -1;
};

class UIMap_Map
{
public:
	UIMap_Map() {};
	~UIMap_Map() {};

	void Init();
	void DrawSprit();
	void PreDraw();

	void AddPosList(Math::Vector3 _3DPos, float _worldTileSize,int LoomNum= m_NOTLoom);

	void AddStairsPos(Math::Vector3 _3DPos, float _worldTileSize, int LoomNum);

	void SetBasePos(Math::Vector2 _basePos) { m_basePos = _basePos; }

	void PosListReset();

	void SetBase3DPos(Math::Vector3 _3dPos)
	{
		m_base3DPos = _3dPos;
	}

	void SetMinMapPlayerPos(Math::Vector2 _pos) { m_minMapPlayerPos = _pos; }


	const std::list<MineMapInfo>& GetMineMapInfo() const { return m_posList; }

	Math::Vector2 GetMapTexSiz() { return m_mapTexSiz; }

	void SetIsStairsMine(bool _flg) { m_isStairsMine = _flg; }
private:

	float m_siz = 1.0f;

	Math::Vector2 m_basePos;
	Math::Vector3 m_base3DPos;


	std::shared_ptr<KdTexture>m_mapTex;
	std::shared_ptr<KdTexture>m_mapStairsTex;

	Math::Vector2 m_mapTexSiz;

	Math::Vector2 m_minMapPlayerPos;


	int m_discoverTileNum = 1;
	void DiscoverTile();


	std::list<MineMapInfo>m_posList;

	//部屋の床リスト
	std::vector< std::list<MineMapInfo>>m_posLoomFloorList;

	static const int m_NOTLoom = -999;

	MineMapInfo m_StairsMineMapInfo;
	bool m_isStairsMine = false;
};
