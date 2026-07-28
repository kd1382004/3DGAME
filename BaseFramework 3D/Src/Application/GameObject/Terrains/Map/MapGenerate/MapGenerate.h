#pragma once

class MapBase;


struct roomEnd
{
	//右端
	int FarRight;

	//左端
	int FarLeft;

	//上
	int topEnd;

	//下
	int downEnd;
};

struct RoomInfo
{
	//部屋ID
	int m_roomID = 0;

	//中心
	Math::Vector2 m_center;

	roomEnd m_roomEnd;

	//プレイヤーのスポーン部屋
	bool m_playerSpwanRoom = false;

	//階段部屋か
	bool m_stairsRoom = false;
};

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
	//_mapSiz			...マップのサイズ(マップタイルが縦横それぞれ何個ずつか)
	//roomNum			...部屋の最大個数
	//tileSiz			...1タイルのサイズ(正方形)
	//_type				...マップの種類
	//ret				...生成結果を格納する先
	//_playerSpawnPos	...	プレイヤーのスポーン位置
	void Generate(Math::Vector2 _mapSiz, int roomNum, float tileSiz, MapType _type, std::list<std::shared_ptr<MapBase>>* ret, Math::Vector3* _playerSpawnPos);

private:

	//部屋IDの初期値
	static const int m_ionitialRoomID = 1;
	std::vector<RoomInfo> m_roomInfo;

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


	//どの部屋とどの部屋をつなぐかを返す
	std::vector<std::pair<RoomInfo, RoomInfo>> GetRoomConnectionPairs(std::vector<RoomInfo> _roomInfo);

	//通路の座標リストを返す
	std::vector<Math::Vector2>  GenerateCorridorPath(RoomInfo _A, RoomInfo _B);



	int FindRoot(std::vector<int>& _parent, int _x);

	void UnionSet(std::vector<int>& _parent, int _a, int _b);

	// 指定された隣接マスが「範囲外」または「空き地（None）」で壁が必要かを判定する
	bool IsNeedWall(int nx, int ny, const std::vector<std::vector<int>>& map);

	// 壁または階段オブジェクトを生成してリストに追加する
	void CreateWallOrStairs(const Math::Vector3& pos,float rotYDegree,bool isStairs,std::list<std::shared_ptr<MapBase>>* ret);
};
