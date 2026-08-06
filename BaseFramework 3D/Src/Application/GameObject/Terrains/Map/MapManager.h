#pragma once
class MapBase;
class CameraBase;
class PlayerBase;
class EnemyBase;
class EnemyManager;


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

private:

	std::weak_ptr<PlayerBase> m_wpPlayerBase;
	std::weak_ptr<EnemyManager> m_wpEnemyManager;

	Math::Vector3 m_playerSpawnPos;

	std::weak_ptr<CameraBase>m_wpCamera;


	std::list<std::shared_ptr<MapBase>> m_mapObj;

	float m_mapTileSiz = 10;
};
