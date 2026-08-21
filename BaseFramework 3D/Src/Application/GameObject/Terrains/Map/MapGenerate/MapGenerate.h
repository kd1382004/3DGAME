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




struct RoomI
{
	//部屋ID
	int m_roomID;

	//部屋の床の座標
	Math::Vector3 m_pos;

	//部屋のType
	int m_roomType;

	//部屋あたりの敵の数
	int m_roomEnemyNum = 0;

	//床番号
	Math::Vector2 m_xy;

	//部屋あたりの宝箱の数
	int m_roomTreasuerChestNum = 0;

	//この床に何か置いたか
	bool m_Installation = false;
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
	//_basePos	...	プレイヤーのスポーン位置
	//戻り値...ノード用のマップデータ(歩けるか歩けないか)
	std::vector<std::vector<bool>> Generate(Math::Vector2 _mapSiz, int roomNum, float tileSiz, int _type, std::list<std::shared_ptr<MapBase>>* ret, Math::Vector3* _playerSpawnPos, Math::Vector3* _basePos);
	
	
	std::vector<std::vector<bool>> GenerateBoss(Math::Vector2 _mapSiz,float tileSiz, int _type, std::list<std::shared_ptr<MapBase>>* ret, Math::Vector3* _playerSpawnPos, Math::Vector3* _basePos);


	// 部屋ごとの情報リストを取得
	const std::vector<std::vector<RoomI>>& GetRoomInfoList() const { return m_roomInfoList; }


	enum class TileType
	{
		None = 0,   // 何もない
		Floor,  // 床（通路）
		Room    // 部屋
	};

	Math::Vector3 GetBossSpawnPos() { return m_bossSpawnPos; }

private:


	//部屋IDの初期値
	static const int m_ionitialRoomID = 1;
	std::vector<RoomInfo> m_roomInfo;

	//部屋の数
	int m_roomNum = 0;

	//部屋の一覧情報が入る
	std::vector<std::vector<RoomI>> m_roomInfoList;

	//部屋のTypeごとの敵の割合(部屋のタイル数にかける)
	struct RoomEnemyPercent
	{
		float m_EnemyRoom = 0.15f;
		float m_NotEnemyRoom = 0.1f;
		float m_SafeRoom = 0.0f;
	};

	RoomEnemyPercent m_roomEnemyPercent;



	//部屋のTypeごとの宝箱の数
	struct RoomTreasuerChestNum
	{
		float m_TreasuerChestRoomMaxNum = 5;
		float m_TreasuerChestRoomMineNum = 3;

		float m_NotTreasuerChestRoomMaxNum = 2;
		float m_NotTreasuerChestRoomMineNum = 0;
	};

	//部屋のTypeごとの宝箱の割合(部屋のタイル数にかける)
	struct RoomreasuerChestPercent
	{
		float m_TreasuerChestRoom = 0.15f;
		float m_NotTreasuerChestRoom = 0.1f;
	};

	RoomTreasuerChestNum m_roomTreasuerChestNum;
	RoomreasuerChestPercent m_roomTreasuerChestPercent;


	//部屋のサイズ
	int roomMax = 0; //最大値
	int roomMine = 0;//最小値

	//部屋のサイズが入ってるファイル名
	std::string m_roomSizPath;

	//_filePath ... ファイルパスを書く
	void LoadRoomSiz(std::string _filePath);
	void SaveRoomSiz(std::string _filePath);


	//どの部屋とどの部屋をつなぐかを返す
	std::vector<std::pair<RoomInfo, RoomInfo>> GetRoomConnectionPairs(const std::vector<RoomInfo>& _roomInfo);

	//通路の座標リストを返す
	std::vector<Math::Vector2>  GenerateCorridorPath(const RoomInfo& _A, const RoomInfo& _B);



	int FindRoot(std::vector<int>& _parent, int _x);

	void UnionSet(std::vector<int>& _parent, int _a, int _b);

	// 指定された隣接マスが「範囲外」または「空き地（None）」で壁が必要かを判定する
	bool IsNeedWall(int nx, int ny, const std::vector<std::vector<int>>& map);

	// 壁または階段オブジェクトを生成してリストに追加する
	void CreateWallOrStairs(const Math::Vector3& pos, float rotYDegree, bool isStairs, std::list<std::shared_ptr<MapBase>>* ret);


	std::vector<Math::Vector3>m_enemySpawnList;

	//敵のスポーン位置を決める
	void EnemySpawnListDecision(std::vector<std::vector<int>> map);

	//ボスの出現位置
	Math::Vector3 m_bossSpawnPos;
};
