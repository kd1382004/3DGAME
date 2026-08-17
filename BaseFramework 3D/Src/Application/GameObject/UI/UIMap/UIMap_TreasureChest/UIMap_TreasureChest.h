#pragma once

class UIMap_Map;

class UIMap_TreasureChest:public KdGameObject
{
public:
	UIMap_TreasureChest() {};
	~UIMap_TreasureChest()override {};

	void Init()override;
	void DrawSprite()override;

	void SetBase3DPos(Math::Vector3 _3dPos)
	{
		m_base3DPos = _3dPos;
	}

	void SetBasePos(Math::Vector2 _basePos) { m_basePos = _basePos; }
	void SetMinMapPlayerPos(Math::Vector2 _minMapPlayerPos) { m_minMapPlayerPos = _minMapPlayerPos; }

	void SetPos(Math::Vector3 _3DPos, float _worldTileSize);

	void SetTileSiz(float _siz) { m_tileSiz = _siz; }

	Math::Vector2 GetPos();

	void SetMapTexSiz(Math::Vector2 _siz) { m_mapTexSiz = _siz; }

	void SetUIMap_Map(std::shared_ptr<UIMap_Map> _UIMap_Map) { m_wpUIMap_Map = _UIMap_Map; }

	void SetExpired(bool _flg) { m_isExpired = _flg; }
private:

	std::weak_ptr<UIMap_Map>m_wpUIMap_Map;

	Math::Vector2 m_minMapPlayerPos;

	int m_discoverTileNum = 2;

	struct List
	{
		Math::Vector3 m_pos;
		bool m_drawFlg = false;
	};


	List m_posList;

	void DiscoverTile(List* _info);

	Math::Vector2 m_mapTexSiz = { };

	float m_siz = 1.0f;

	float m_tileSiz;

	Math::Vector2 m_basePos;

	std::shared_ptr<KdTexture>m_treasureChestTex;

	Math::Vector3 m_2DPos;

	Math::Vector3 m_base3DPos;

	bool m_drawFlg=false;
};
