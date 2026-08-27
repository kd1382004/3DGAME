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

//宝箱
class TreasureChestManager;

// ノード構造体 (A* 経路探索用)
struct Node
{
	Math::Vector2 pos = Math::Vector2::Zero;

	// 通れるかどうか
	bool walkable = false;

	// スタートからのコスト
	float gCost = 0.0f;

	// ゴールまでの推定コスト
	float hCost = 0.0f;

	// 経路復元用
	Node* parent = nullptr;

	float fCost() const { return gCost + hCost; }
};

enum MapType
{
	//草原
	MapType_Grassland

};

class MapManager : public KdGameObject
{
public:
	MapManager() = default;
	~MapManager() override = default;

	void Init() override;
	void Update() override;
	void PostUpdate() override;
	void DrawLit() override;
	void PreDraw() override;
	void GenerateDepthMapFromLight() override;

	// マップと当たり判定させたいオブジェクトを登録
	void MapHit(const std::shared_ptr<KdGameObject>& obj);

	// マップと当たり判定させたい敵を登録
	void MapHitEnemy(const std::shared_ptr<EnemyBase>& obj);

	void SetCamera(const std::shared_ptr<CameraBase>& spCamera);

	const Math::Vector3& GetPlayerSpawnPos() const { return m_playerSpawnPos; }

	void SetPlayerSpawnPos(const Math::Vector3& playerSpawnPos) { m_playerSpawnPos = playerSpawnPos; }


	//マップ生成
	//_mapSiz			...マップのサイズ(マップタイルが縦横それぞれ何個ずつか)
	//roomNum			...部屋の最大個数
	//_type				...マップの種類
	void GenerateMap(Math::Vector2 _mapSiz, int roomNum, MapType _type);

	//_mapSiz			...マップのサイズ(マップタイルが縦横それぞれ何個ずつか)
	//_type				...マップの種類
	void GenerateBossMap(Math::Vector2 _mapSiz, MapType _type);

	void SetPlayer(const std::shared_ptr<PlayerBase>& spPlayerBase) { m_wpPlayerBase = spPlayerBase; }
	void SetEnemyManager(const std::shared_ptr<EnemyManager>& spEnemyManager) { m_wpEnemyManager = spEnemyManager; }
	void SetUIManager(const std::shared_ptr<UIManager>& spUIManager) { m_wpUIManager = spUIManager; }
	void SetTreasureChestManager(const std::shared_ptr<TreasureChestManager>& spTreasureChestManager) { m_wpTreasureChestManager = spTreasureChestManager; }

	////////////////////////////////////////////
	/// <ノード>

	// ノードからワールド座標に変換
	Math::Vector3 NodeToWorld(const Node* node) const;

	// ワールド座標からノードに変換
	Node* WorldToNode(const Math::Vector3& worldPos);

	// A* 経路探索
	std::vector<Node*> FindPath(Node* start, Node* goal);

	/// </ノード>
	////////////////////////////////////////////


	void SetMapObjManager(std::shared_ptr<MapObjManager>_obj) { m_wpMapObjManager = _obj; }

private:

	////////////////////////////////////////////
	/// <ノード>

	// ノード配列初期化
	void CreateNodeGrid(int width, int height, float tileSize);

	// 歩けるかどうかを設定
	void ApplyWalkableFromMap(const std::vector<std::vector<bool>>& mapData);

	std::vector<std::vector<Node>> m_nodes;

	// 経路復元
	std::vector<Node*> BuildPath(Node* goal) const;

	// 上下左右を返す
	std::vector<Node*> GetNeighbors(Node* node);

	// マンハッタン距離
	float Heuristic(const Node* a, const Node* b) const
	{
		return std::abs(a->pos.x - b->pos.x) + std::abs(a->pos.y - b->pos.y);
	}

	/// </ノード>
	////////////////////////////////////////////

	std::weak_ptr<PlayerBase> m_wpPlayerBase;
	Math::Vector3 m_playerSpawnPos = Math::Vector3::Zero;

	std::weak_ptr<EnemyManager> m_wpEnemyManager;
	std::weak_ptr<TreasureChestManager> m_wpTreasureChestManager;



	std::weak_ptr<CameraBase> m_wpCamera;

	std::list<std::shared_ptr<MapBase>> m_mapObj;

	float m_mapTileSiz = 10.0f;

	////////////////////////////////////////
	// UI用マップ
	std::weak_ptr<UIManager> m_wpUIManager;

	///////////////////////////////////////
	//マップオブジェクト
	std::weak_ptr<MapObjManager>m_wpMapObjManager;
};

