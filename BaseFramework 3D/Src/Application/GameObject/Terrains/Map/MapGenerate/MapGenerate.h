#pragma once

class MapBase;

enum MapType
{
	//草原
	MapType_Grassland

};

class MapGenerate
{
public:
	MapGenerate();
	~MapGenerate() {};

	//マップ生成
	//_mapSiz	 ...マップのサイズ(マップタイルが縦横それぞれ何個ずつか)
	//roomNum	 ...部屋の最大個数
	//tileSiz	 ...1タイルのサイズ(正方形)
	//_type		 ...マップの種類
	//ret		 ...生成結果を格納する先
	void Generate(Math::Vector2 _mapSiz, int roomNum, float tileSiz, MapType _type, std::list<std::shared_ptr<MapBase>>* ret);

private:

	enum class TileType
	{
		None = 0,   // 何もない
		Floor,  // 床（通路）
		Room    // 部屋
	};


	//部屋のサイズ
	int roomMax = 0; //最大値
	int roomMine = 0;//最小値

	//部屋のサイズが入ってるファイル名
	std::string m_roomSizPath;

	//_filePath ... ファイルパスを書く
	void LoadRoomSiz(std::string _filePath);
	void SaveRoomSiz(std::string _filePath);
};
