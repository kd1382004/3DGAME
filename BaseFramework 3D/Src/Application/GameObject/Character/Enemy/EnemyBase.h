#pragma once
#include"../CharacterBase.h"

class PlayerBase;
class MapManager;

struct Node;

class EnemyBase :public CharacterBase
{
public:
	EnemyBase() {};
	~EnemyBase() override {};

	virtual void Init()override;
	virtual void PreUpdate()override;
	virtual void Update()override;
	virtual void PostUpdate()override;

	void PreDraw()override;


	void SetSpawnPos(const Math::Vector3& _pos);

	//プレイヤーを探す
	void SearchPlayer();

	void PlayerChase();

	void SetPlayer(std::shared_ptr<PlayerBase>_spPalyer) { m_wpPlayer = _spPalyer; };
	void SetMapManager(std::shared_ptr<MapManager>_spMapManager) { m_wpMapManager = _spMapManager; };
protected:




	///////////////////////////////////////////
	//マップマネージャー1
	std::weak_ptr<MapManager>m_wpMapManager;


	///////////////////////////////////////////
	//プレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayer;

	//プレイヤー座標
	Math::Vector3 m_playerPos;


	///////////////////////////////////////////
	//出現位置
	Math::Vector3 m_spawnPos = { 0,0,0 };

	//移動方向
	Math::Vector3 m_moveDir;

	//視野角(度数法)
	float m_viewAngle = 90;

	//視界の距離
	float m_viewDistance = 10;

	//視界から見失う距離
	float m_loseSightDistance = 20.0f;

	//プレイヤーChaseフラグ
	bool m_playerChaseFlg = false;

	//スポーン地点に戻る
	bool m_returnSpawnPosFlg = false;


	void ReturnSpawnPos();

	//待機時間
	float m_stayTime;

	//プレイヤーを見逃したときの待機時間
	float m_lostSightWaitTime = 2;

	//目的地に着いた時の待機時間
	float m_arrivalWaitTime = 1;

	//経路探索
	std::vector<Node*> m_path;
	int m_pathIndex = 0;
	float m_repathTimer = 0.0f;


	//徘徊半径
	float wanderRadius;
private:
	//視錐台用のBoxInfo
	KdCollider::BoxInfo m_frustumBox;
	bool m_isInView = false;
};
