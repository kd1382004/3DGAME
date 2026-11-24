#pragma once
#include <cmath>

class MapBase;
class CameraBase;

//Plyer
class PlayerBase;

//Enemy
class EnemyBase;
class EnemyManager;

//UI
class UIManager;

//マップオブジェクト
class MapObjManager;
class 	MapGenerate;

// ノード構造体 (A* 経路探索用)
struct Node
{
public:

	//---- その他の処理 ----

	//合計のコストを返す
	//戻り値 ... gCost+hCost
	float FCost() const { return m_gCost + m_hCost; }

	//---- メンバ変数 ----

	//ノードの位置(マス番号)
	Math::Vector2 m_pos = Math::Vector2::Zero;

	// 通れるかどうか
	bool m_walkable = false;

	// スタートからのコスト
	float m_gCost = 0.0f;

	// ゴールまでの推定コスト
	float m_hCost = 0.0f;

	// 経路復元用
	Node* m_parent = nullptr;
	uint32_t m_searchId = 0; // 今回の探索ID（これで初期化を判定）
	bool m_inClosed = false;	// 探索済み(クローズドリストに入っている)か
};

//マップの種類
enum MapType
{
	//地下迷宮(石造りの通路・松明)
	MapType_Dungeon,

	//機械仕掛け
	MapType_Factory,


	//マップの種類異数かつ設定されてない
	Maptype_Siz,
};

//マップの管理・経路探索
class MapManager : public KdGameObject
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	MapManager() = default;

	//デストラクタ
	~MapManager() override = default;

	//---- 基本の処理 ----

	//初期化
	void Init() override;

	//更新前の処理
	void PreUpdate() override;

	//更新
	void Update() override;


	//更新後の処理
	void PostUpdate() override;

	//ライトありの描画
	void DrawLit() override;

	//描画前の処理(視錐台カリングなど)
	void PreDraw() override;

	//影の描画
	void GenerateDepthMapFromLight() override;

	//---- 取得 ----

	//プレイヤーのスポーン位置を返す
	//戻り値 ... スポーン位置
	const Math::Vector3& GetPlayerSpawnPos() const { return m_playerSpawnPos; }

	//座標を渡して更新するかどうかを返す
	// _pos ... 座標
	bool GetChunksUpdate(Math::Vector3 _pos);

	//チャンク番号を渡して更新するかどうかを返す
	// _chunkNum ... チャンク番号
	bool GetChunksUpdate(Math::Vector2 _chunkNum);

	//チャンク番号を渡して描画するかどうかを返す
	// _chunkNum ... チャンク番号
	//戻り値 ... 描画するならtrue
	bool GetChunksDraw(Math::Vector2 _chunkNum);

	//このフレームでプレイヤーのいるチャンクが変わったか
	//戻り値 ... 変わったならtrue
	bool GetIsChunkChanged() { return m_isChunkChanged; }

	//更新するチャンクのマップを返す
	//戻り値 ... 更新するマップの一覧
	std::vector<std::shared_ptr<MapBase>> GetUpdateChunks() { return m_updateChunks; }

	//マップ上のランダムな座標を返す
	Math::Vector3 GetRandomPos();

	//次の階へのワープゲートの位置を返す
	//戻り値 ... ワープゲートの位置
	Math::Vector3 GetNextWarpGatePos() { return m_nextWarpGatePos; }

	//脱出用ワープゲートの位置を返す
	//戻り値 ... ワープゲートの位置
	Math::Vector3 GetEscapeWarpGatePos() { return m_escapeWarpGatePos; }

	//描画チャンクの中心の座標を返す
	//戻り値 ... 中心の座標
	Math::Vector3 GetChunkCenterPos();

	//---- 設定 ----

	//カメラをセット
	// _spCamera ... カメラ
	void SetCamera(const std::shared_ptr<CameraBase>& _spCamera);

	//プレイヤーのスポーン位置をセット
	// _playerSpawnPos ... スポーン位置
	void SetPlayerSpawnPos(const Math::Vector3& _playerSpawnPos) { m_playerSpawnPos = _playerSpawnPos; }

	//プレイヤーをセット
	// _spPlayerBase ... プレイヤー
	void SetPlayer(const std::shared_ptr<PlayerBase>& _spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }

	//敵の管理をセット
	// _spEnemyManager ... 敵の管理
	void SetEnemyManager(const std::shared_ptr<EnemyManager>& _spEnemyManager) { m_wpEnemyManager = _spEnemyManager; }

	//UIの管理をセット
	// _spUIManager ... UIの管理
	void SetUIManager(const std::shared_ptr<UIManager>& _spUIManager) { m_wpUIManager = _spUIManager; }

	//マップオブジェクトの管理をセット
	// _spObj ... マップオブジェクトの管理
	void SetMapObjManager(std::shared_ptr<MapObjManager>_spObj) { m_wpMapObjManager = _spObj; }

	//プレイヤーの座標をセットして、Update以降処理するチャンクを決める
	// _pos ... プレイヤーの座標
	void SetPlayerChunk(Math::Vector3 _pos);

	//---- その他の処理 ----

	// マップと当たり判定させたいオブジェクトを登録
	// _spObj ... 当たり判定させるオブジェクト
	void MapHit(const std::shared_ptr<KdGameObject>& _spObj);

	// マップと当たり判定させたい敵を登録
	// _spObj ... 当たり判定させる敵
	void MapHitEnemy(const std::shared_ptr<EnemyBase>& _spObj);

	//マップ生成
	//_mapSize			...マップのサイズ(マップタイルが縦横それぞれ何個ずつか)
	//_roomNum			...部屋の最大個数
	//_type				...マップの種類
	void GenerateMap(Math::Vector2 _mapSize, int _roomNum, MapType _type);

	//_mapSize			...マップのサイズ(マップタイルが縦横それぞれ何個ずつか)
	//_type				...マップの種類
	void GenerateBossMap(Math::Vector2 _mapSize, MapType _type);

	// ノードからワールド座標に変換
	// _node ... ノード
	Math::Vector3 NodeToWorld(const Node* _node) const;

	// ワールド座標からノードに変換
	// _worldPos ... ワールド座標
	Node* WorldToNode(const Math::Vector3& _worldPos);

	// A* 経路探索
	// _start ... スタートのノード
	// _goal ... ゴールのノード
	std::vector<Node*> FindPath(Node* _start, Node* _goal);

private:

	//---- メンバ変数 ----

	//次の階へのワープゲートの位置
	Math::Vector3 m_nextWarpGatePos;

	//脱出用ワープゲートの位置
	Math::Vector3 m_escapeWarpGatePos;

	//マップの基準位置
	Math::Vector3 m_mapBasePos;

	//ノードの配列
	std::vector<std::vector<Node>> m_nodes;

	//今の探索ID
	uint32_t m_currentSearchId = 0;

	/// </ノード>

	//プレイヤー
	std::weak_ptr<PlayerBase> m_wpPlayerBase;

	//プレイヤーのスポーン位置
	Math::Vector3 m_playerSpawnPos = Math::Vector3::Zero;

	//敵の管理
	std::weak_ptr<EnemyManager> m_wpEnemyManager;

	//カメラ
	std::weak_ptr<CameraBase> m_wpCamera;

	//マップオブジェクトの一覧
	std::list<std::shared_ptr<MapBase>> m_mapObj;

	//チャンクごとのマップオブジェクト
	std::vector<std::vector<std::vector<std::weak_ptr<MapBase>>>> m_chunks;

	//更新処理するチャンク
	std::vector<std::shared_ptr<MapBase>>m_updateChunks;

	//描画するチャンク
	std::vector<std::shared_ptr<MapBase>>m_drawChunks;

	//チャンクのXYを保存
	Math::Vector2 m_chunkNum = { -999,-999 };

	//このフレームでプレイヤーのいるチャンクが変わったか
	bool m_isChunkChanged = false;

	//1タイルの大きさ
	float m_mapTileSize = 10.0f;

	// UI用マップ
	std::weak_ptr<UIManager> m_wpUIManager;

	//マップオブジェクト
	std::weak_ptr<MapObjManager>m_wpMapObjManager;

	//マップ生成
	std::shared_ptr<MapGenerate> m_spMapGenerate;

	//1チャンクのタイル数(縦横)
	int m_chunkSize = 1;

	//更新チャンク
	//縦横に+-xyずつ
	Math::Vector2 m_updateChunkRadius = { 2,2 };

	//描画チャンク
	//縦横に更新チャンクに対して+-xyずつ
	Math::Vector2  m_drawChunkRadius = { 0, 0 };

	//チャンクを囲むボックス
	Math::Vector3 m_boxPos;

	//チャンクを囲むボックスの大きさ
	Math::Vector3 m_boxSiz;

	//チャンクを囲むボックスのモデル
	std::shared_ptr<KdModelData>m_spBoxModel;

	//生成直後の1回目か(最初に描画範囲に入ったものはフェードなしで表示する)
	bool m_isFirstDraw = true;

	//---- 取得 ----

	// 上下左右を返す
	// _node ... 中心のノード
	std::vector<Node*> GetNeighbors(Node* _node);

	//---- その他の処理 ----

	// ノード配列初期化
	// _width ... 横のマス数
	// _height ... 縦のマス数
	// _tileSize ... 1マスの大きさ
	void CreateNodeGrid(int _width, int _height, float _tileSize);

	// 歩けるかどうかを設定
	// _mapData ... 歩けるかどうかのマップデータ
	void ApplyWalkableFromMap(const std::vector<std::vector<bool>>& _mapData);

	// 経路復元
	// _goal ... ゴールのノード
	std::vector<Node*> BuildPath(Node* _goal) const;

	// 8方向移動用ヒューリスティック(Octile距離)
	// _a ... ノード1
	// _b ... ノード2
	float Heuristic(const Node* _a, const Node* _b) const
	{
		float dx = std::abs(_a->m_pos.x - _b->m_pos.x);
		float dy = std::abs(_a->m_pos.y - _b->m_pos.y);
		return (dx + dy) + (1.41421356f - 2.0f) * std::min(dx, dy);
	}

	//マップの種類に合わせたモデルを読み込んでセットする
	// _type ... マップの種類
	void LoadModelByMapType(MapType _type);

	MapType m_oldMapType = MapType::Maptype_Siz;
};

