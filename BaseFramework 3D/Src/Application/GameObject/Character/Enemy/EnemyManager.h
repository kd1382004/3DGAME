#pragma once
class EnemyBase;
class PlayerBase;
class MapManager;
class CameraBase;
class UIManager;


enum EnemyType
{
	//部屋タイプの敵
	RoomEnemy

};

class EnemyManager:public KdGameObject
{
public:
	EnemyManager() {};
	~EnemyManager()override {};

	virtual void Init()override;
	virtual void PreUpdate()override;
	virtual void Update()override;
	virtual void PostUpdate()override;

	void PreDraw()override;
	void DrawLit()override;
	void GenerateDepthMapFromLight() override;
	void DrawDebug() override;

	void SetPlayer(std::shared_ptr<PlayerBase>_spPalyer);
	void SetMapManager(std::shared_ptr<MapManager>_spMapManager) { m_wpMapManager = _spMapManager; }
	void SetCamera(std::shared_ptr<CameraBase>_wpCamera) { m_wpCamera = _wpCamera; }

	std::list<std::shared_ptr<EnemyBase>>& GetEnemyList(){ return m_enemyList; }


	void SpawnEnemy(EnemyType _enemyType,Math::Vector3 _spawnPos);

	void EnemyListReset() { m_enemyList.clear(); }

	void AddUIList(std::shared_ptr<UIManager>_spUIManager) { m_wpUIManager = _spUIManager; }

	void SpawnBoss(Math::Vector3 _spawnPos);
protected:

	std::weak_ptr<UIManager>m_wpUIManager;

	void SetEnemyListPlayer();

	///////////////////////////////////////////
	//プレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayer;

	std::weak_ptr<MapManager>m_wpMapManager;

	std::weak_ptr<CameraBase>m_wpCamera;

	//敵リスト
	std::list<std::shared_ptr<EnemyBase>> m_enemyList;

};
