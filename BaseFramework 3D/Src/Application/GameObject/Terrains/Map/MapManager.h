#pragma once
class MapBase;
class CameraBase;
class PlayerBase;
class EnemyBase;
class EnemyManager;


struct Node {
	Math::Vector2 pos;

	// 通れるかどうか
	bool walkable;   

	// スタートからのコスト
	float gCost;   

	// ゴールまでの推定コスト
	float hCost;    

	// 経路復元用
	Node* parent;  

	float fCost() const { return gCost + hCost; }
};


class MapManager :public KdGameObject
{
public:
	MapManager() {};
	~MapManager() {};


	void Init()override;
	void Update()override;
	void PostUpdate()override;
	void DrawLit()override;
	void PreDraw()override;
	void GenerateDepthMapFromLight() override;

	//マップと当り判定させたいやつを入れる
	void MapHit(std::shared_ptr< KdGameObject>obj);

	//マップと当り判定させたいやつを入れる
	void MapHitEnemy(std::shared_ptr<EnemyBase>obj);

	void SetCamera(std::shared_ptr<CameraBase> _spCamera);

	Math::Vector3 GetPlayerSpawnPos() { return m_playerSpawnPos; }
	void SetPlayerSpawnPos(Math::Vector3 _playerSpawnPos) { m_playerSpawnPos = _playerSpawnPos; }


	void GenerateMap();

	void SetPlayer(std::shared_ptr<PlayerBase> _spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }
	void SetEnemyManager(std::shared_ptr<EnemyManager> _spEnemyManager) { m_wpEnemyManager = _spEnemyManager; }


	////////////////////////////////////////////
	/// <ノード>
	
	//ノードからワールド座標に変換
	Math::Vector3 NodeToWorld(const Node* node);

	//ワールド座標からノードに変換
	Node* WorldToNode(const Math::Vector3& worldPos);

	/// </ノード>
	////////////////////////////////////////////
private:

	////////////////////////////////////////////
	/// <ノード>

	//ノード配列初期化
	void CreateNodeGrid(int width, int height, float tileSize);

	//歩けるかどうかを設定
	void ApplyWalkableFromMap(const std::vector<std::vector<int>>& mapData);

	std::vector<std::vector<Node>> m_nodes;

	/// </ノード>
	////////////////////////////////////////////

	std::weak_ptr<PlayerBase> m_wpPlayerBase;
	std::weak_ptr<EnemyManager> m_wpEnemyManager;

	Math::Vector3 m_playerSpawnPos;

	std::weak_ptr<CameraBase>m_wpCamera;


	std::list<std::shared_ptr<MapBase>> m_mapObj;

	float m_mapTileSiz = 10;
};
