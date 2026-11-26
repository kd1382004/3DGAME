#pragma once


//ミニマップのタイル1つ分の情報
struct MiniMapInfo
{
	//階段があるか
	bool m_flg = false;


	//タイルの位置(画面)
	Math::Vector2 m_pos;
	//タイルの番号
	Math::Vector2 m_tileIndex;
	//表示するか(踏破したか)
	bool m_drawFlg = false;

	//部屋番号(部屋じゃなければkNOTRoom)
	int m_roomNum = -1;
};

//ミニマップの地形
class UIMap_Map
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	UIMap_Map() {};

	//デストラクタ
	~UIMap_Map() {};

	//---- 基本の処理 ----

	//初期化
	void Init();

	//2Dの描画
	void DrawSprite();

	//描画前の処理
	void PreDraw();

	//---- 取得 ----

	//タイルの一覧を返す
	//戻り値 ... タイルの一覧
	const std::list<MiniMapInfo>& GetMiniMapInfo() const { return m_posList; }

	//1マスの画像の大きさを返す
	//戻り値 ... 画像の大きさ
	Math::Vector2 GetMapTexSize() { return m_mapTexSize; }


	//---- 設定 ----

	//床のタイルを追加
	// _3DPos ... 床の座標(ワールド)
	// _worldTileSize ... 1タイルの大きさ
	// _roomNum ... 部屋番号
	void AddPosList(Math::Vector3 _3DPos, float _worldTileSize,int _roomNum= kNOTRoom);

	//階段のタイルを追加
	// _3DPos ... 階段の座標(ワールド)
	// _worldTileSize ... 1タイルの大きさ
	// _roomNum ... 部屋番号
	void AddStairsPos(Math::Vector3 _3DPos, float _worldTileSize, int _roomNum);

	//ミニマップの基準位置(画面)をセット
	// _basePos ... 基準位置(画面)
	void SetBasePos(Math::Vector2 _basePos) { m_basePos = _basePos; }

	//マップの基準位置(ワールド)をセット
	// _3DPos ... 基準位置(ワールド)
	void SetBase3DPos(Math::Vector3 _3DPos)
	{
		m_base3DPos = _3DPos;
	}

	//ミニマップ上のプレイヤーの位置をセット
	// _pos ... プレイヤーの位置(画面)
	void SetMiniMapPlayerPos(Math::Vector2 _pos) { m_miniMapPlayerPos = _pos; }

	//階段を見つけたかをセット
	// _flg ... 見つけたか
	void SetIsStairsMine(bool _flg) { m_isStairsMine = _flg; }

	// ミニマップの表示モードを設定する
	// true  : 全タイルを常時表示
	// false : プレイヤーが踏破したタイルだけ表示
	// _enable ... 全タイルを常時表示するか
	void SetMinimapRevealAll(bool _enable);

	//---- その他の処理 ----

	//タイルの一覧を空にする
	void PosListReset();

	// タイル座標から直接引く（見つからなければ nullptr）
	// _x ... タイルのx
	// _y ... タイルのy
	const MiniMapInfo* FindTile(int _x, int _y) const
	{
		auto it = m_tileMap.find(MakeKey(_x, _y));
		return (it != m_tileMap.end()) ? it->second : nullptr;
	}


	//プレイヤーと同じ部屋のタイルか
	// _x ... タイルのx
	// _y ... タイルのy
	//戻り値 ... 同じ部屋ならtrue(どちらかが通路ならfalse)
	bool IsSamePlayerRoom(int _x, int _y) const
	{
		if (m_playerRoomNum == kNOTRoom) { return false; }

		const MiniMapInfo* tile = FindTile(_x, _y);
		if (!tile) { return false; }

		return tile->m_roomNum == m_playerRoomNum;
	}

	//プレイヤーが部屋にいるか
	bool IsPlayerInRoom() const { return m_playerRoomNum != kNOTRoom; }

	//プレイヤーがいるタイルのx
	int GetPlayerTileX() const { return m_playerTileX; }

	//プレイヤーがいるタイルのy
	int GetPlayerTileY() const { return m_playerTileY; }

private:

	//---- 定数 ----

	//部屋じゃないときの部屋番号
	static const int kNOTRoom = -999;

	//---- メンバ変数 ----

	int m_playerRoomNum = kNOTRoom;
	int m_playerTileX = 0;
	int m_playerTileY = 0;

	// タイル座標 → タイル（1座標に1タイル）
	std::unordered_map<long long, MiniMapInfo*> m_tileMap;

	//大きさの倍率
	float m_size = 1.0f;

	//ミニマップの基準位置(画面)
	Math::Vector2 m_basePos;

	//マップの基準位置(ワールド)
	Math::Vector3 m_base3DPos;

	//床の画像
	std::shared_ptr<KdTexture>m_spMapTex;

	//階段の画像
	std::shared_ptr<KdTexture>m_spMapStairsTex;

	//1マスの画像の大きさ
	Math::Vector2 m_mapTexSize;

	//ミニマップ上のプレイヤーの位置
	Math::Vector2 m_miniMapPlayerPos;

	//プレイヤーから何マス以内を踏破したことにするか
	int m_discoverTileNum = 1;

	//タイルの一覧
	std::list<MiniMapInfo>m_posList;

	//部屋の床リスト
	std::vector< std::list<MiniMapInfo>>m_posRoomFloorList;

	//階段のタイル
	MiniMapInfo m_stairsMiniMapInfo;

	//階段を見つけたか
	bool m_isStairsMine = false;

	//タイルを焼き付けるテクスチャ
	KdRenderTargetPack m_rtPack;

	//焼き付け直す必要があるか
	bool m_isDirty = true;

	//---- その他の処理 ----

	//タイル座標を1つの数字にする
	// _x ... タイルのx
	// _y ... タイルのy
	//戻り値 ... 検索用のキー
	static long long MakeKey(int _x, int _y)
	{
		return (static_cast<long long>(_x) << 32) ^ static_cast<unsigned int>(_y);
	}

	//プレイヤーの近くのタイルを踏破したことにする
	void DiscoverTile();

	//タイルをテクスチャに焼き付ける
	void BakeTiles();
};
