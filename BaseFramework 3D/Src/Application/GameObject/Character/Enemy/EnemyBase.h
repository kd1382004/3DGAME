#pragma once
#include"../CharacterBase.h"

class PlayerBase;
class MapManager;
class GameScene;
class EnemyCrowdController;
class EnemyManager;
class AttackGage;


struct Node;

class EnemyBase :public CharacterBase, public std::enable_shared_from_this<EnemyBase>
{
public:
	EnemyBase() {};
	~EnemyBase() override { Release(); };

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

	void SetGameScene(std::shared_ptr<GameScene>_spGameScene) { m_wpGameScene = _spGameScene; };


	void AddUIList(std::shared_ptr<UIManager>_spUIManager);

	float GetAngle() { return m_angle; }


	//攻撃をくらった処理
	//_damage ... ダメージ量
	//_knockbackDistance ... ふっとばし距離
	//_knockbackDir ... ふっとばし方向
	// _hitStunTime ... のけぞり時間
	//_isCritical ... クリティカルかどうか
	//__ignoreRate ... 防御無視
	void OnAttackHit(float _damage, float _knockbackDistance, const Math::Vector3& _knockbackDir, float _hitStunTime, bool _isCritical, float _ignoreRate)override;

	Math::Vector3 GetPlayerPos() { return m_playerPos; }


	void SetEnemyManager(std::shared_ptr<EnemyManager>_spEnemyManager) { m_wpEnemyManager = _spEnemyManager; }
	std::shared_ptr<EnemyManager> GetEnemyManager() { return m_wpEnemyManager.lock(); }


	void SetMoveVec(Math::Vector3 _vec)
	{
		m_moveVec = _vec;
		m_moveVec.Normalize();
	}
protected:

	std::weak_ptr<EnemyManager>m_wpEnemyManager;

	void Release()override;

	void EnemyAnimeModeUpdate();

	void HPPosPostUpdate();

	///////////////////////////////////////////
	//マップマネージャー1
	std::weak_ptr<MapManager>m_wpMapManager;


	///////////////////////////////////////////
	//プレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayer;

	//プレイヤー座標
	Math::Vector3 m_playerPos;

	//
	float m_PlayerChaseDir = 1.5;


	///////////////////////////////////////////
	//出現位置
	Math::Vector3 m_spawnPos = { 0,0,0 };

	//視野角(度数法)
	float m_viewAngle = 180;

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

	/////////////////////////////////////
	//アニメーション
	enum EnemyAnimeMode
	{
		//待機
		EnemyAnimeMode_Idel,

		//歩き
		EnemyAnimeMode_Walk,

		//走り
		EnemyAnimeMode_Run,

		//攻撃
		EnemyAnimeMode_Attck,

		//死亡
		EnemyAnimeMode_Die
	};

	EnemyAnimeMode m_enemyAnimeMode = EnemyAnimeMode_Idel;

	//アニメーション変更フラグ
	bool m_AnimeChangeFlg = false;

	void SetDead();


	std::weak_ptr<GameScene>m_wpGameScene;


	//
	std::shared_ptr<EnemyCrowdController> m_crowdController;



	//適用アッタックゲージ
	std::weak_ptr<AttackGage>m_wpAttackGage;

	float m_attackgagePercent;


	void SetAttackGagePercent(float _percent);
private:
	//視錐台用のBoxInfo
	KdCollider::BoxInfo m_frustumBox;
	bool m_isInView = false;
};
