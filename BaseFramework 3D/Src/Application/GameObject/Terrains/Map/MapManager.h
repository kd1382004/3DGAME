#pragma once
class MapBase;

class MapManager:public KdGameObject
{
public:
	MapManager() {};
	~MapManager() {};


	void Init()override;
	void Update()override;
	void DrawLit()override;
	void GenerateDepthMapFromLight() override;

	//マップと当り判定させたいやつを入れる
	void MapHit(std::shared_ptr< KdGameObject>obj);

private:

	std::list<std::shared_ptr<MapBase>> m_mapObj;

	float m_mapTileSiz = 10;
};
