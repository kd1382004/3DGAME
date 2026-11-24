#pragma once

class MapBase;
class MapObjManager;
class MapObjBase;
class CameraBase;

enum MapType;

//////////////////////////////////////////////////////
// マップ生成で使う型
//////////////////////////////////////////////////////

//---- マス(タイル) ----

//マス1つの種類
enum class TileType
{
	None = 0,	// 何もない(壁の外。歩けない)
	Floor,		// 床（通路・交差点）
	Room,		// 部屋の床
	Slope		// 坂(高さの違う床をつなぐ)
};

//マス1つ分の情報
//マップは std::vector<std::vector<FloorInfo>> で、[y][x] の2次元に並べて持つ
struct FloorInfo
{
	//タイルの種類
	TileType m_tileType = TileType::None;

	//階層(床の高さ。0 が一番下)
	int m_heightLevel = 0;

	//回転(坂の向きなどに使う。度)
	int m_angle = 0;

	//松明を置いたかどうか(同じマスに2つ置かないため)
	bool m_setTorchFlg = false;
};

//---- 部屋 ----

//部屋の四方の端(マス番号)
//端のマスも部屋に含む
struct RoomEnd
{
	//右端
	int m_farRight = 0;

	//左端
	int m_farLeft = 0;

	//上
	int m_topEnd = 0;

	//下
	int m_downEnd = 0;
};

//部屋(または交差点)1つ分の情報
//通路でつなぐ「点」としても使う(区画分割では交差点も同じ型で持つ)
struct RoomInfo
{
	//部屋ID(交差点は -1)
	int m_roomID = 0;

	//中心(マス)
	Math::Vector2 m_center;

	//部屋の端
	RoomEnd m_roomEnd;

	//プレイヤーのスポーン部屋
	bool m_playerSpawnRoom = false;

	//階段部屋か
	bool m_stairsRoom = false;

	//部屋の階層
	int m_heightLevel = 0;

	//この部屋(交差点)がある区画の列番号(ランダム配置のときは0)
	int m_areaX = 0;
	//この部屋(交差点)がある区画の行番号(ランダム配置のときは0)
	int m_areaY = 0;

	//部屋ではなく交差点(通路の分かれ道)ならtrue
	//trueのときは m_roomID = -1 で、部屋の種類・スポーン・階段の対象にしない
	bool m_isDummy = false;
};

//---- 区画分割 ----

//区画(マップを格子状に分けた1つ分)の情報
//区画1つにつき、部屋か交差点を1つ置く
struct AreaInfo
{
	//区画の列番号(左から0)
	int m_areaX = 0;
	//区画の行番号(上から0)
	int m_areaY = 0;

	//区画の左端のタイル番号
	int m_left = 0;
	//区画の上端のタイル番号
	int m_top = 0;
	//区画の右端のタイル番号(このマスも区画に含む)
	int m_right = 0;
	//区画の下端のタイル番号(このマスも区画に含む)
	int m_bottom = 0;

	//部屋ではなく交差点を置く区画ならtrue
	bool m_isDummy = false;
};

//---- 通路 ----

//通路でつなぐ2つの部屋(または交差点)の組
//GetRoomConnectionPairs が作り、Generate がこの順に通路を掘る
struct ConnectPair
{
	//部屋A(通路を掘り始める側)
	RoomInfo m_a;
	//部屋B(通路の行き先)
	RoomInfo m_b;

	//部屋Aの connectNodeList での番号
	int m_nodeA = -1;
	//部屋Bの connectNodeList での番号
	int m_nodeB = -1;

	//小さな輪を作るために足した辺ならtrue(最小全域木の辺ならfalse)
	//trueの辺だけがT字路の候補になる
	bool m_isLoop = false;
};

//部屋のどの辺から通路を出すか(入るか)
enum class ExitSide
{
	Left,	// 左の辺
	Right,	// 右の辺
	Top,	// 上の辺
	Bottom,	// 下の辺
};

//---- 生成後の部屋の床 ----

//生成し終わった部屋の床1マス分の情報
//敵・宝箱を置くときや、外から部屋の一覧を見るとき(GetRoomInfoList)に使う
struct RoomI
{
	//部屋ID
	int m_roomID = 0;

	//部屋の床の座標
	Math::Vector3 m_pos;

	//部屋のType
	int m_roomType = 0;

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


//////////////////////////////////////////////////////
// ダンジョンのマップを作るクラス
// 階ごとに「区画分割」か「ランダム配置」で部屋を置き、部屋どうしを
// くねくねした通路(小さな輪・T字路つき)でつないで、床・壁・坂などを作る
//////////////////////////////////////////////////////
class MapGenerate
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	MapGenerate();

	//デストラクタ
	~MapGenerate() {};

	//---- マップ生成 ----

	//マップ生成
	//_mapSize			...マップのサイズ(マップタイルが縦横それぞれ何個ずつか)
	//_roomNum			...部屋の数(区画が足りないときは減る。最低 kMinRoomNum)
	//_tileSiz			...1タイルのサイズ(正方形)
	//_type				...マップの種類
	//_ret				...生成結果を格納する先
	//_playerSpawnPos	...	プレイヤーのスポーン位置
	//_basePos	...	マップの基準位置
	//戻り値...ノード用のマップデータ(歩けるか歩けないか)
	std::vector<std::vector<bool>> Generate(Math::Vector2 _mapSize, int _roomNum, float _tileSiz, int _type, std::list<std::shared_ptr<MapBase>>* _ret, Math::Vector3* _playerSpawnPos, Math::Vector3* _basePos);

	//ボス部屋のマップ生成
	// _mapSize ... マップのサイズ(マップタイルが縦横それぞれ何個ずつか)
	// _tileSiz ... 1タイルのサイズ(正方形)
	// _type ... マップの種類
	// _ret ... 生成結果を格納する先
	// _playerSpawnPos ... プレイヤーのスポーン位置を入れる先
	// _basePos ... マップの基準位置を入れる先
	//戻り値 ... ノード用のマップデータ(歩けるか歩けないか)
	std::vector<std::vector<bool>> GenerateBoss(Math::Vector2 _mapSize, float _tileSiz, int _type, std::list<std::shared_ptr<MapBase>>* _ret, Math::Vector3* _playerSpawnPos, Math::Vector3* _basePos);

	//---- 取得 ----

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

	//チャンクごとのマップオブジェクトを返す
	//戻り値 ... チャンク[x][y]ごとのマップの一覧
	std::vector<std::vector<std::vector<std::weak_ptr<MapBase>>>> GetChunks() { return m_chunks; }

	//1チャンクのタイル数を返す
	//戻り値 ... 1チャンクのタイル数(縦横)
	int GetChunkSize() { return kChunkSize; }

	//階層の最大数を返す
	//戻り値 ... 階層の最大数
	const int GetHeightLevelMax() { return kHeightLevelMax; }

	//---- 設定 ----

	//マップオブジェクトの管理をセット
	// _spObj ... マップオブジェクトの管理
	void SetMapObjManager(std::shared_ptr<MapObjManager>_spObj) { m_wpMapObjManager = _spObj; }

	//カメラをセット
	// _spCamera ... カメラ
	void SetCamera(std::shared_ptr<CameraBase> _spCamera) { m_wpCamera = _spCamera; }

	//マップの種類に合わせたモデルを読み込んでセットする
	// _type ... マップの種類
	void LoadModelByMapType(MapType _type);
private:

	//////////////////////////////////////////////////////
	// 定数
	//////////////////////////////////////////////////////

	//---- 部屋 ----

	//部屋IDの初期値
	static const int kInitialRoomID = 1;

	//最低限の部屋の数
	//プレイヤースポーン部屋,階段部屋,その他の3つが最低限必要な数
	static const int kMinRoomNum = 3;

	//---- 部屋の置き方 ----

	//区画分割で部屋を置く確率(%)。外れたらランダム配置にする
	static const int kAreaLayoutRate = 50;

	//---- 高さ(階層) ----

	//階層の数
	static const int kHeightLevel = 2;

	//階層の最大数
	static const int kHeightLevelMax = kHeightLevel + 5;

	//---- 通路の段差 ----

	//段差の場所が足りないときに、通路の形を作り直す最大回数
	static const int kSlopeRetryMax = 5;

	//段差どうしを何マス以上離すか(2未満だと続けて上がって坂にならない)
	static const int kSlopeMinGap = 2;

	//---- チャンク ----

	//1チャンクのタイル数(縦横)
	const	int  kChunkSize = 5;

	//---- 区画分割 ----

	//区画の内側に空ける余白(マス)。隣の部屋との距離はこの2倍になる
	//今までの部屋の間隔(kHeightLevel + 2)を、両側の区画で半分ずつ持つ
	//※ kHeightLevel を使うので、kHeightLevel より後ろに置く
	static const int kAreaPadding = (kHeightLevel + 2 + 1) / 2;

	//区画数を選ぶときに、余る区画(交差点になる区画)をどれだけ嫌うか
	static constexpr float kAreaAspectWeight = 0.05f;

	//---- 小さな輪 ----

	//部屋＋交差点の数に対して、足す輪の数の割合
	static constexpr float kLoopRatio = 0.25f;

	//木の上で何本の通路をたどれば行ける2部屋までを、輪の候補にするか
	//3なら2×2区画の四角い輪になる
	static const int kLoopMaxTreeDist = 3;

	//---- くねくね通路 ----

	//通路の直線1本の最低マス数(部屋から出るときと入る前の直線にも使う)
	static const int kWiggleMinRun = 2;

	//部屋がまっすぐ並んでいるときに、わざと寄り道する確率(%)
	static const int kWiggleBumpRate = 70;

	//寄り道で横にずれる最大マス数
	static const int kWiggleBumpMax = 5;

	//出口・入口の位置を選び直す最大回数
	static const int kEdgeRetryMax = 10;

	//---- T字路 ----

	//輪の辺をT字路にする確率(%)
	static const int kTJunctionRate = 60;

	//T字路の合流点を、どの部屋からも何マス以上離すか
	static const int kTMinFromRoom = 3;

	//T字路の合流点を、部屋Aの中心から何マス以上離すか
	static const int kTMinDistFromA = 6;

	//////////////////////////////////////////////////////
	// 中で使う構造体
	//////////////////////////////////////////////////////

	//---- 敵・宝箱の設定 ----

	//部屋のTypeごとの敵の割合(部屋のタイル数にかける)
	struct RoomEnemyPercent
	{
		//敵の部屋
		float m_enemyRoom = 0.1f;
		//敵の部屋以外
		float m_notEnemyRoom = 0.05f;
		//安全な部屋
		float m_safeRoom = 0.0f;
	};

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

	//////////////////////////////////////////////////////
	// メンバ変数
	//////////////////////////////////////////////////////

	//---- 部屋のサイズ設定 ----

	//部屋のサイズが入ってるファイル名
	std::string m_roomSizPath;

	//部屋のサイズ(MapSiz.json から読み込む)
	int m_roomMax = 0; //最大値
	int m_roomMin = 0; //最小値

	//---- 部屋と交差点の情報 ----

	//部屋ごとの情報
	std::vector<RoomInfo> m_roomInfo;

	//部屋の数
	int m_roomNum = 0;

	//部屋の一覧情報が入る
	std::vector<std::vector<RoomI>> m_roomInfoList;

	//部屋ごとの松明(添え字は部屋ID)
	//部屋の壁と同じく、最後に部屋がまたがるチャンク全部を松明に教える
	std::vector<std::vector<std::shared_ptr<MapObjBase>>> m_roomTorchList;

	//交差点の一覧(本物の部屋の m_roomInfo とは分けて持つ)
	std::vector<RoomInfo> m_dummyNodeList;

	//---- 部屋の置き方 ----

	//今回の生成で区画分割を使ったならtrue、ランダム配置ならfalse
	//(PlaceRoomsByArea / PlaceRoomsRandom の先頭で入れる)
	//trueのときだけ、つなぐ候補を隣の区画どうしに絞る
	bool m_isAreaLayout = true;

	//---- 区画 ----

	//今回の生成での横の区画数(Generateのたびに計算する)
	int m_areaDivX = 1;

	//今回の生成での縦の区画数(Generateのたびに計算する)
	int m_areaDivY = 1;

	//区画の一覧。[行][列]で引く
	std::vector<std::vector<AreaInfo>> m_areaGrid;

	//---- 敵・宝箱 ----

	//部屋の種類ごとの敵の割合
	RoomEnemyPercent m_roomEnemyPercent;

	//部屋の種類ごとの宝箱の数
	RoomTreasureChestNum m_roomTreasureChestNum;

	//部屋の種類ごとの宝箱の割合
	RoomTreasureChestPercent m_roomTreasureChestPercent;

	//敵のスポーン位置の一覧
	std::vector<Math::Vector3>m_enemySpawnList;

	//---- ボス部屋・ワープゲートの位置 ----

	//ボスの出現位置
	Math::Vector3 m_bossSpawnPos;

	//ワープゲートの出現位置
	Math::Vector3 m_nextWarpGatePos;

	//脱出用ワープゲートの出現位置
	Math::Vector3 m_escapeWarpGatePos;

	//---- モデル ----

	//床モデル
	std::shared_ptr<KdModelWork>m_spFloorModel = nullptr;

	//Slopeモデル
	std::shared_ptr<KdModelWork>m_spSlopeModel = nullptr;

	//壁モデル
	std::shared_ptr<KdModelWork>m_spWallModel = nullptr;

	//階段モデル
	std::shared_ptr<KdModelWork>m_spStairsModel = nullptr;

	//---- チャンク ----

	//チャンクごとのマップオブジェクト
	std::vector<std::vector<std::vector<std::weak_ptr<MapBase>>>> m_chunks;

	//---- 外から受け取るもの ----

	//マップオブジェクトの管理
	std::weak_ptr<MapObjManager>m_wpMapObjManager;

	//カメラ
	std::weak_ptr<CameraBase> m_wpCamera;

	//////////////////////////////////////////////////////
	// 関数
	//////////////////////////////////////////////////////

	//---- 設定ファイル ----

	//部屋のサイズをファイルから読み込む
	// _filePath ... 読み込むファイルのパス
	void LoadRoomSize(std::string _filePath);

	//部屋のサイズをファイルに保存する
	// _filePath ... 保存するファイルのパス
	void SaveRoomSize(std::string _filePath);

	//---- 部屋の置き方(どちらか1つを Generate から呼ぶ) ----

	//区画分割で、マップに部屋と交差点を置く
	//区画の数を決めて区画に分け、区画ごとに部屋か交差点を1つ置く
	// _mapSize ... マップの大きさ(マス)
	// _roomNum ... 置きたい部屋の数(区画が足りないときは区画数まで減らして書き戻す)
	// _map ... マップ(部屋と交差点のタイルを書き込む)
	// _roomIDVector ... マスごとの部屋ID(部屋のマスに書き込む)
	// _roomID ... 次に使う部屋ID(部屋を置くたびに1進める)
	void PlaceRoomsByArea(const Math::Vector2& _mapSize, int& _roomNum, std::vector<std::vector<FloorInfo>>& _map, std::vector<std::vector<int>>& _roomIDVector, int& _roomID);

	//ランダムな位置に部屋を置く(区画分割を使わないやり方)
	//重ならない場所が見つかるまでくり返し、部屋が kMinRoomNum 個に届かなければ全部作り直す
	// _mapSize ... マップの大きさ(マス)
	// _roomNum ... 置きたい部屋の数
	// _map ... マップ(部屋のタイルを書き込む)
	// _roomIDVector ... マスごとの部屋ID(部屋のマスに書き込む)
	// _roomID ... 次に使う部屋ID(部屋を置くたびに1進める)
	void PlaceRoomsRandom(const Math::Vector2& _mapSize, int& _roomNum, std::vector<std::vector<FloorInfo>>& _map, std::vector<std::vector<int>>& _roomIDVector, int& _roomID);

	//---- 区画分割(PlaceRoomsByArea の中で使う) ----

	//区画の数を決めて m_areaDivX / m_areaDivY に入れる
	//区画が正方形に近く、余る区画(交差点)が少ない組み合わせを選ぶ
	// _mapSize ... マップの大きさ(マス)
	// _roomNum ... 置きたい部屋の数(区画が足りないときは区画数まで減らして書き戻す)
	void DecideAreaDiv(const Math::Vector2& _mapSize, int& _roomNum);

	//マップを区画に分けて m_areaGrid を作る
	//割り切れない余りは、右端の列と下端の行の区画に足す
	// _mapSize ... マップの大きさ(マス)
	void DivideAreas(const Math::Vector2& _mapSize);

	//どの区画を交差点にするか決める(区画数 - _roomNum 個をランダムに選ぶ)
	// _roomNum ... 置く部屋の数
	void DecideDummyAreas(int _roomNum);

	//区画に部屋を置いて m_roomInfo に登録する
	// _area ... 部屋を置く区画
	// _map ... マップ(部屋のタイルを書き込む)
	// _roomIDVector ... マスごとの部屋ID(部屋のマスに書き込む)
	// _roomID ... 次に使う部屋ID(置けたら1進める)
	//戻り値 ... 置けたらtrue。区画が小さくて置けなければfalse
	bool PlaceRoomInArea(const AreaInfo& _area, std::vector<std::vector<FloorInfo>>& _map, std::vector<std::vector<int>>& _roomIDVector, int& _roomID);

	//区画に交差点(1マスの通路)を置いて m_dummyNodeList に登録する
	//部屋IDは進めない
	// _area ... 交差点を置く区画
	// _map ... マップ(交差点のタイルを書き込む)
	void PlaceDummyInArea(const AreaInfo& _area, std::vector<std::vector<FloorInfo>>& _map);

	//---- つなぐ組(最小全域木＋小さな輪) ----

	//つなぐ部屋の組を作る
	//最小全域木(クラスカル法)でつなぎ、小さな輪になる辺を足す
	//区画分割のときは、隣の区画どうしだけを候補にする
	//木の辺が先、輪の辺が後の順に並ぶ(T字路は木の通路が先に掘られている前提)
	// _nodeList ... 部屋と交差点をつなげたリスト(connectNodeList)
	//戻り値 ... つなぐ組のリスト
	std::vector<ConnectPair> GetRoomConnectionPairs(const std::vector<RoomInfo>& _nodeList);

	//2つの部屋(交差点)の区画が、上下左右で隣り合っているか(斜めは含めない)
	// _a ... 部屋A
	// _b ... 部屋B
	//戻り値 ... 隣ならtrue
	bool IsAdjacentArea(const RoomInfo& _a, const RoomInfo& _b) const;

	//木の上で、2つの部屋の間に何本の通路があるかをBFSで数える
	// _treeAdjList ... 部屋ごとの、木でつながっている相手の番号のリスト
	// _from ... 数え始める部屋の番号
	// _to ... 目的の部屋の番号
	//戻り値 ... 通路の本数(たどり着けなければ -1)
	int GetTreeDist(const std::vector<std::vector<int>>& _treeAdjList, int _from, int _to) const;

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

	//(旧版。今は上の ConnectPair を返す GetRoomConnectionPairs を使う)
	//どの部屋とどの部屋をつなぐかを返す
	// _roomInfo ... 部屋の一覧
	//std::vector<std::pair<RoomInfo, RoomInfo>> GetRoomConnectionPairs(const std::vector<RoomInfo>& _roomInfo);

	//※ 定義がない宣言(どこからも使っていない)。消してよい
	std::vector<ConnectPair> GetRoomConnectionPairsA(const std::vector<RoomInfo>& _nodeList);

	//---- 通路を掘る ----

	//通路の座標リストを返す
	// _a ... 通路の始まりの部屋
	// _b ... 通路の終わりの部屋
	// _map ... 今のマップ
	// _stopAtExisting ... すでにある床に着いたら止めるか
	std::vector<Math::Vector3> GenerateCorridorPath(const RoomInfo& _a, const RoomInfo& _b, const std::vector<std::vector<FloorInfo>>& _map, bool _stopAtExisting = true);

	//位置が部屋の中か、部屋の上下左右の隣にあるか
	// _pos ... 調べる位置(マス)
	// _room ... 部屋
	//戻り値 ... 届いていればtrue
	bool IsReachedRoom(const Math::Vector2& _pos, const RoomInfo& _room) const;

	//---- くねくね通路 ----

	//くねくね通路のマスの並びを作る(GenerateCorridorPath のL字の代わり)
	// _a ... 部屋A(出る側)
	// _b ... 部屋B(入る側。T字路のときは1×1の一時的な合流先)
	// _map ... マップ(出口が空いているかの確認に使う)
	//戻り値 ... 部屋Aの出口から部屋Bの入口までのマスの並び
	std::vector<Math::Vector2> BuildWigglePath(const RoomInfo& _a, const RoomInfo& _b, const std::vector<std::vector<FloorInfo>>& _map) const;

	//相手の部屋がどちらにあるかで、出る辺と入る辺を決める
	// _a ... 部屋A(出る側)
	// _b ... 部屋B(入る側)
	// _sideA ... 部屋Aの出る辺(結果を入れる)
	// _sideB ... 部屋Bの入る辺(結果を入れる)
	void DecideExitSide(const RoomInfo& _a, const RoomInfo& _b, ExitSide& _sideA, ExitSide& _sideB) const;

	//辺の上のランダムな位置を返す(角のマスは除く。交差点なら中心のマス)
	// _room ... 部屋
	// _side ... どの辺か
	//戻り値 ... 辺の上の位置(マス)
	Math::Vector2 PickEdgePoint(const RoomInfo& _room, ExitSide _side) const;

	//辺から外向きの向きを返す
	// _side ... どの辺か
	// _dx ... xの向き(-1, 0, 1 のどれかを入れる)
	// _dy ... yの向き(-1, 0, 1 のどれかを入れる)
	void GetExitDir(ExitSide _side, int& _dx, int& _dy) const;

	//出口から kWiggleMinRun マスまっすぐ出る区間と、その両横が空いているか
	// _pos ... 出口の位置(マス)
	// _side ... 出る辺
	// _map ... マップ
	//戻り値 ... 空いていればtrue
	bool IsExitClear(const Math::Vector2& _pos, ExitSide _side, const std::vector<std::vector<FloorInfo>>& _map) const;

	//パスの最後の位置から目標まで、1マスずつまっすぐ足す(横→縦の順)
	// _path2D ... 足す先のパス
	// _x ... 目標のx(マス)
	// _y ... 目標のy(マス)
	void AppendLine(std::vector<Math::Vector2>& _path2D, int _x, int _y) const;

	//---- T字路 ----

	//両隣が床で、直角方向が空いている通路のマスか(曲がり角や分かれ道は false)
	// _x ... 調べるマスのx
	// _y ... 調べるマスのy
	// _map ... マップ
	//戻り値 ... まっすぐな通路の途中ならtrue
	bool IsStraightCorridor(int _x, int _y, const std::vector<std::vector<FloorInfo>>& _map) const;

	//どれかの部屋(m_roomInfo)から指定マス以内にあるか
	// _x ... 調べるマスのx
	// _y ... 調べるマスのy
	// _dist ... 何マス以内を「近い」とするか
	//戻り値 ... 近ければtrue
	bool IsNearRoom(int _x, int _y, int _dist) const;

	//---- 通路の仕上げ ----

	// 全部屋が連結になるまで通路を追加する
	// _map ... マップ(通路を書き込む)
	void ConnectAllRegions(std::vector<std::vector<FloorInfo>>& _map);

	// 広い通路（2x2等の太い通路）をスリム化・クリーンアップする
	// _map ... マップ
	void CleanupWideCorridors(std::vector<std::vector<FloorInfo>>& _map);

	//通路のタイルを消してもいいか
	// _x ... マスのx
	// _y ... マスのy
	// _map ... マップ
	//戻り値 ... 消してもいいならtrue
	bool CanRemoveCorridorTile(int _x, int _y, std::vector<std::vector<FloorInfo>>& _map);

	//---- 高さ・坂 ----

	//パスの中で、段差(坂)を置けるマスの番号を集める
	//前後のマスと一直線に並んでいる所だけ(曲がり角と両端は除く)
	// _path2D ... 通路のマスの並び
	//戻り値 ... 段差を置ける番号の一覧
	std::vector<int> FindSlopeCandidates(const std::vector<Math::Vector2>& _path2D) const;

	//段差を置く場所を、kSlopeMinGap 以上あけて _needNum 個選ぶ
	// _candidates ... 段差を置ける番号の一覧
	// _needNum ... 必要な段差の数(高さの差)
	// _result ... 選んだ番号(小さい順)
	//戻り値 ... 足りたらtrue
	bool PickSlopeIndices(const std::vector<int>& _candidates, int _needNum, std::vector<int>& _result) const;

	//パスを、既存の床に当たった所(または隣に来た所)で切る
	// _path2D ... 通路のマスの並び(切ったものに書き換える)
	// _a ... 部屋A(この部屋のマスは無視する)
	// _map ... マップ
	// _hB ... 切った場合は、つながる床の高さを入れる
	void CutPathAtExisting(std::vector<Math::Vector2>& _path2D, const RoomInfo& _a, const std::vector<std::vector<FloorInfo>>& _map, int& _hB) const;

	//高さを線形補間で割り振る(段差の場所が足りないときの予備)
	// _path2D ... 通路のマスの並び
	// _hA ... 始まりの高さ
	// _hB ... 終わりの高さ
	//戻り値 ... 高さつきのマスの並び
	std::vector<Math::Vector3> AssignHeightsLinear(const std::vector<Math::Vector2>& _path2D, int _hA, int _hB) const;

	//坂を置ける場所か調べて置く
	// _map ... マップ
	void SlopeCheck(std::vector<std::vector<FloorInfo>>* _map);

	//---- できあがりのチェック ----

	//マップに異常がないかどうか
	//true...異常なし
	//false...異常あり
	// _map ... 調べるマップ
	bool MapCheck(std::vector<std::vector<FloorInfo>>& _map);

	//---- 壁・階段・松明 ----

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
	// _layerNum ... 何段ぶんの高さの壁にするか(1なら今までどおり1段)
	std::shared_ptr<MapBase> CreateWallOrStairs(const Math::Vector3& _pos, float _rotYDegree, bool _isStairs, std::list<std::shared_ptr<MapBase>>* _ret, int _roomID, int _x, int _y, std::vector<std::vector<FloorInfo>>& _map, Math::Vector2 _chunkNum, bool* _torchFlg = nullptr, int _layerNum = 1);

	//松明を置く
	// _rotYDegree ... Y軸の回転(度)
	// _pos ... 置く座標
	// _spObj ... くっつける壁
	// _chunk ... チャンク番号
	//戻り値 ... 置いた松明(置けなかったら nullptr)
	std::shared_ptr<MapObjBase> SetTorch(float _rotYDegree, Math::Vector3 _pos, std::shared_ptr<KdGameObject>_spObj, Math::Vector2 _chunk);

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

	//---- 敵 ----

	//敵のスポーン位置を決める
	// _map ... マップ
	void EnemySpawnListDecision(std::vector<std::vector<int>> _map);



	//---- その他の処理 ----
	// 
	//マップの種類ごとのモデルのフォルダ名を返す
	// _type ... マップの種類
	//戻り値 ... フォルダ名(例: "Castle")
	std::string GetMapModelFolder(MapType _type) const;

	//モデルを読み込む(入れ物がなければ作る)
	// _spModel ... 読み込み先のモデル
	// _path    ... モデルのパス
	void LoadModel(std::shared_ptr<KdModelWork>& _spModel, const std::string& _path);
};