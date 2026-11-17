#pragma once

class MapBase;
class MapObjManager;
class CameraBase;

enum class TileType
{
	None = 0,   // 何もない
	Floor,  // 床（通路）
	Room,    // 部屋
	Slope	//階段(床にあるやつ)
};


struct FloorInfo
{
	//タイルの種類
	TileType m_tileType = TileType::None;

	//階層
	int m_heightLevel = 0;

	//回転
	int m_angle = 0;

	//松明を置いたかどうか
	bool m_setTorchFlg = false;
};


struct RoomEnd
{
	//右端
	int m_farRight;

	//左端
	int m_farLeft;

	//上
	int m_topEnd;

	//下
	int m_downEnd;
};

struct RoomInfo
{
	//部屋ID
	int m_roomID = 0;

	//中心
	Math::Vector2 m_center;

	//部屋の端
	RoomEnd m_roomEnd;

	//プレイヤーのスポーン部屋
	bool m_playerSpawnRoom = false;

	//階段部屋か
	bool m_stairsRoom = false;

	//部屋の階層
	int m_heightLevel;
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
	int m_roomTreasureChestNum = 0;

	//この床に何か置いたか
	bool m_installation = false;


	//チャンク番号
	Math::Vector2 m_chunk;


	//この床のマップオブジェクト
	std::shared_ptr<MapBase> m_spMap;
};




class MapGenerate
{
public:
	//コンストラクタ
	MapGenerate();
	//デストラクタ
	~MapGenerate() {};

	//マップ生成
	//_mapSiz			...マップのサイズ(マップタイルが縦横それぞれ何個ずつか)
	//_roomNum			...部屋の最大個数
	//_tileSiz			...1タイルのサイズ(正方形)
	//_type				...マップの種類
	//_ret				...生成結果を格納する先
	//_playerSpawnPos	...	プレイヤーのスポーン位置
	//_basePos	...	マップの基準位置
	//戻り値...ノード用のマップデータ(歩けるか歩けないか)
	std::vector<std::vector<bool>> Generate(Math::Vector2 _mapSiz, int _roomNum, float _tileSiz, int _type, std::list<std::shared_ptr<MapBase>>* _ret, Math::Vector3* _playerSpawnPos, Math::Vector3* _basePos);


	//ボス部屋のマップ生成
	// _mapSiz ... マップのサイズ(マップタイルが縦横それぞれ何個ずつか)
	// _tileSiz ... 1タイルのサイズ(正方形)
	// _type ... マップの種類
	// _ret ... 生成結果を格納する先
	// _playerSpawnPos ... プレイヤーのスポーン位置を入れる先
	// _basePos ... マップの基準位置を入れる先
	//戻り値 ... ノード用のマップデータ(歩けるか歩けないか)
	std::vector<std::vector<bool>> GenerateBoss(Math::Vector2 _mapSiz, float _tileSiz, int _type, std::list<std::shared_ptr<MapBase>>* _ret, Math::Vector3* _playerSpawnPos, Math::Vector3* _basePos);


	// 部屋ごとの情報リストを取得
	const std::vector<std::vector<RoomI>>& GetRoomInfoList() const { return m_roomInfoList; }

	//ボスの出現位置を返す
	//戻り値 ... ボスの出現位置
	Math::Vector3 GetBossSpawnPos() { return m_bossSpawnPos; }

	//次の階へのワープゲートの位置を返す
	//戻り値 ... ワープゲートの位置
	Math::Vector3 GetNextWarpGatePos() const { return m_nextWarpGatePos; }
	//脱出用ワープゲートの位置を返す
	//戻り値 ... ワープゲートの位置
	Math::Vector3 GetEscapeWarpGatePos() const { return m_escapeWarpGatePos; }


	//マップオブジェクトの管理をセット
	// _spObj ... マップオブジェクトの管理
	void SetMapObjManager(std::shared_ptr<MapObjManager>_spObj) { m_wpMapObjManager = _spObj; }



	//チャンクごとのマップオブジェクトを返す
	//戻り値 ... チャンク[x][y]ごとのマップの一覧
	std::vector<std::vector<std::vector<std::weak_ptr<MapBase>>>> GetChunks() { return m_chunks; }

	//1チャンクのタイル数を返す
	//戻り値 ... 1チャンクのタイル数(縦横)
	int GetChunkSize() { return kChunkSize; }


	//カメラをセット
	// _spCamera ... カメラ
	void SetCamera(std::shared_ptr<CameraBase> _spCamera) { m_wpCamera = _spCamera; }

	//階層の最大数を返す
	//戻り値 ... 階層の最大数
	const int GetHeightLevelMax() { return kHeightLevelMax; }

private:


	//マップに異常がないかどうか
	//true...異常なし
	//false...異常あり
	// _map ... 調べるマップ
	bool MapCheck(std::vector<std::vector<FloorInfo>>& _map);


	//部屋IDの初期値
	static const int kInitialRoomID = 1;
	//部屋ごとの情報
	std::vector<RoomInfo> m_roomInfo;

	//部屋の数
	int m_roomNum = 0;

	//部屋の一覧情報が入る
	std::vector<std::vector<RoomI>> m_roomInfoList;

	//部屋のTypeごとの敵の割合(部屋のタイル数にかける)
	struct RoomEnemyPercent
	{
		//敵の部屋
		float m_enemyRoom = 0.2f;
		//敵の部屋以外
		float m_notEnemyRoom = 0.15f;
		//安全な部屋
		float m_safeRoom = 0.0f;
	};

	//部屋の種類ごとの敵の割合
	RoomEnemyPercent m_roomEnemyPercent;



	//部屋のTypeごとの宝箱の数
	struct RoomTreasureChestNum
	{
		//宝箱部屋の宝箱の最大数
		float m_treasureChestRoomMaxNum = 7;
		//宝箱部屋の宝箱の最小数
		float m_treasureChestRoomMinNum = 4;

		//宝箱部屋以外の宝箱の最大数
		float m_notTreasureChestRoomMaxNum = 3;
		//宝箱部屋以外の宝箱の最小数
		float m_notTreasureChestRoomMinNum = 1;
	};

	//部屋のTypeごとの宝箱の割合(部屋のタイル数にかける)
	struct RoomTreasureChestPercent
	{
		//宝箱部屋
		float m_treasureChestRoom = 0.2f;
		//宝箱部屋以外
		float m_notTreasureChestRoom = 0.1f;
	};

	//部屋の種類ごとの宝箱の数
	RoomTreasureChestNum m_roomTreasureChestNum;
	//部屋の種類ごとの宝箱の割合
	RoomTreasureChestPercent m_roomTreasureChestPercent;


	//部屋のサイズ
	int m_roomMax = 0; //最大値
	int m_roomMin = 0;//最小値

	//部屋のサイズが入ってるファイル名
	std::string m_roomSizPath;

	//_filePath ... ファイルパスを書く
	void LoadRoomSiz(std::string _filePath);
	//部屋のサイズをファイルに保存する
	// _filePath ... 保存するファイルのパス
	void SaveRoomSiz(std::string _filePath);


	//どの部屋とどの部屋をつなぐかを返す
	// _roomInfo ... 部屋の一覧
	std::vector<std::pair<RoomInfo, RoomInfo>> GetRoomConnectionPairs(const std::vector<RoomInfo>& _roomInfo);

	//通路の座標リストを返す
	// _a ... 通路の始まりの部屋
	// _b ... 通路の終わりの部屋
	// _map ... 今のマップ
	// _stopAtExisting ... すでにある床に着いたら止めるか
	std::vector<Math::Vector3> GenerateCorridorPath(const RoomInfo& _a, const RoomInfo& _b,const std::vector<std::vector<FloorInfo>>& _map, bool _stopAtExisting = true);

	// 全部屋が連結になるまで通路を追加する
	// _map ... マップ(通路を書き込む)
	void ConnectAllRegions(std::vector<std::vector<FloorInfo>>& _map);


	//Union-Findの根を探す
	// _parent ... 親の番号の配列
	// _x ... 調べる番号
	//戻り値 ... 根の番号
	int FindRoot(std::vector<int>& _parent, int _x);

	//Union-Findで2つをつなげる
	// _parent ... 親の番号の配列
	// _a ... つなげる番号1
	// _b ... つなげる番号2
	void UnionSet(std::vector<int>& _parent, int _a, int _b);

	// 指定された隣接マスが「範囲外」または「空き地（None）」で壁が必要かを判定する
	// _nx ... 隣のマスのx
	// _ny ... 隣のマスのy
	// _map ... マップ
	// _heightLevel ... 今の階層
	// _x ... 今のマスのx
	// _y ... 今のマスのy
	bool IsNeedWall(int _nx, int _ny, const std::vector<std::vector<FloorInfo>>& _map, int _heightLevel, int _x, int _y);

	// 壁または階段オブジェクトを生成してリストに追加する
	// _pos ... 置く座標
	// _rotYDegree ... Y軸の回転(度)
	// _isStairs ... 階段にするか
	// _ret ... 追加するリスト
	// _roomID ... 部屋ID
	// _x ... マスのx
	// _y ... マスのy
	// _map ... マップ
	// _chunkNum ... チャンク番号
	// _torchFlg ... 松明を置いたかを入れる先(nullptrなら置かない)
	std::shared_ptr<MapBase> CreateWallOrStairs(const Math::Vector3& _pos, float _rotYDegree, bool _isStairs, std::list<std::shared_ptr<MapBase>>* _ret, int _roomID, int _x, int _y, std::vector<std::vector<FloorInfo>>& _map, Math::Vector2 _chunkNum, bool* _torchFlg = nullptr);


	//敵のスポーン位置の一覧
	std::vector<Math::Vector3>m_enemySpawnList;

	//敵のスポーン位置を決める
	// _map ... マップ
	void EnemySpawnListDecision(std::vector<std::vector<int>> _map);

	//ボスの出現位置
	Math::Vector3 m_bossSpawnPos;

	//ワープゲートの出現位置
	Math::Vector3 m_nextWarpGatePos;
	//脱出用ワープゲートの出現位置
	Math::Vector3 m_escapeWarpGatePos;


	//////////////////////////////////
	//マップオブジェクト
	std::weak_ptr<MapObjManager>m_wpMapObjManager;

	//松明を置く
	// _rotYDegree ... Y軸の回転(度)
	// _pos ... 置く座標
	// _spObj ... くっつける壁
	// _chunk ... チャンク番号
	void SetTorch(float _rotYDegree, Math::Vector3 _pos, std::shared_ptr<KdGameObject>_spObj, Math::Vector2 _chunk);

	//角なら設置
	// _x ... マスのx
	// _y ... マスのy
	// _map ... マップ
	bool IsCornerWall(int _x, int _y, const std::vector<std::vector<FloorInfo>>& _map);

	//T字路なら設置
	// _x ... マスのx
	// _y ... マスのy
	// _map ... マップ
	bool IsTIntersection(int _x, int _y, const std::vector<std::vector<FloorInfo>>& _map);

	//トーチを設置していいか
	//_num...直線方向にnumずつ見る
	// _x ... マスのx
	// _y ... マスのy
	// _map ... マップ
	bool IsSetTorch(int _x, int _y, const std::vector<std::vector<FloorInfo>>& _map, int _num);
	///////////////////


	//階層の数
	static const int kHeightLevel = 2;
	//階層の最大数
	static const int kHeightLevelMax = kHeightLevel + 1;



	//坂を置ける場所か調べて置く
	// _map ... マップ
	void SlopeCheck(std::vector<std::vector<FloorInfo>>* _map);

	// 広い通路（2x2等の太い通路）をスリム化・クリーンアップする
	// _map ... マップ
	void CleanupWideCorridors(std::vector<std::vector<FloorInfo>>& _map);
	//通路のタイルを消してもいいか
	// _x ... マスのx
	// _y ... マスのy
	// _map ... マップ
	//戻り値 ... 消してもいいならtrue
	bool CanRemoveCorridorTile(int _x, int _y, std::vector<std::vector<FloorInfo>>& _map);



	///////////////////////////////////////
	//オブジェ
	//床モデル
	std::shared_ptr<KdModelWork>m_spFloorModel = nullptr;

	//Slopeモデル
	std::shared_ptr<KdModelWork>m_spSlopeModel = nullptr;

	//壁モデル
	std::shared_ptr<KdModelWork>m_spWallModel = nullptr;

	//階段モデル
	std::shared_ptr<KdModelWork>m_spStairsModel = nullptr;


	//チャンクごとのマップオブジェクト
	std::vector<std::vector<std::vector<std::weak_ptr<MapBase>>>> m_chunks;


	//1チャンクのタイル数(縦横)
	const	int  kChunkSize = 5;



	//カメラ
	std::weak_ptr<CameraBase> m_wpCamera;

	//最低限の部屋の数
	//プレイヤースポーン部屋,階段部屋,その他の3つが最低限必要な数
	static const int kMinRoomNum = 3;
};


