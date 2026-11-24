#pragma once
class EnemyBase;
class PlayerBase;
class MapManager;
class CameraBase;
class UIManager;
class GameScene;
class HitDamage;


//出す敵の種類
enum EnemyType
{
	//部屋タイプの敵
	RoomEnemy,

	//階段部屋タイプの敵
	RoomStarsEnemy,
};

//敵の管理
class EnemyManager:public KdGameObject, public std::enable_shared_from_this<EnemyManager>
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	EnemyManager() {};

	//デストラクタ
	~EnemyManager()override {};

	//---- 基本の処理 ----

	//初期化
	virtual void Init()override;

	//更新前の処理
	virtual void PreUpdate()override;

	//更新
	virtual void Update()override;

	//更新後の処理
	virtual void PostUpdate()override;

	//描画前の処理
	void PreDraw()override;

	//ライトありの描画
	void DrawLit()override;

	//影の描画
	void GenerateDepthMapFromLight() override;

	//デバッグ描画
	void DrawDebug() override;

	//---- 取得 ----

	//敵のリストを返す
	//戻り値 ... 敵のリスト
	std::list<std::shared_ptr<EnemyBase>>& GetEnemyList(){ return m_enemyList; }

	//更新する(プレイヤーの近くの)敵のリストを返す
	//戻り値 ... 更新する敵のリスト
	std::list<std::shared_ptr<EnemyBase>>& GetEnemyUpdateList(){ return m_enemyUpdateList; }

	//---- 設定 ----

	//プレイヤーをセット
	// _spPlayer ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayer);

	//マップの管理をセット
	// _spMapManager ... マップの管理
	void SetMapManager(std::shared_ptr<MapManager>_spMapManager) { m_wpMapManager = _spMapManager; }

	//カメラをセット
	// _spCamera ... カメラ
	void SetCamera(std::shared_ptr<CameraBase>_spCamera) { m_wpCamera = _spCamera; }

	//ゲームシーンをセット
	// _spGameScene ... ゲームシーン
	void SetGameScene(std::shared_ptr<GameScene>_spGameScene) { m_wpGameScene = _spGameScene; }

	//UIの管理をセット
	// _spUIManager ... UIの管理
	void AddUIList(std::shared_ptr<UIManager>_spUIManager) { m_wpUIManager = _spUIManager; }

	//ダメージ数字の表示をセット
	// _spHitDamage ... ダメージ数字の表示
	void SetHitDamage(std::shared_ptr<HitDamage>_spHitDamage) { m_wpHitDamage = _spHitDamage; }

	//自分自身をセット
	// _spEnemyManager ... 敵の管理(自分)
	void SetEnemyManager(std::shared_ptr<EnemyManager>_spEnemyManager) { m_wpEnemyManager = _spEnemyManager; }

	//プレイヤーの近くの敵だけを更新するリストに入れる
	// _spMapManager ... マップの管理
	void SetEnemyUpdateList(std::shared_ptr<MapManager> _spMapManager);

	//全部の敵の視界を表示するかをセット
	// _flg ... 表示するならtrue
	void SetVisionFlg(bool _flg);

	//---- その他の処理 ----

	//敵を出す
	// _enemyType ... 敵の種類
	// _spawnPos ... 出す位置
	void SpawnEnemy(EnemyType _enemyType,Math::Vector3 _spawnPos);

	//敵のリストを空にする
	void EnemyListReset() { m_enemyList.clear(); }

	//ボスを出す
	// _spawnPos ... 出す位置
	void SpawnBoss(Math::Vector3 _spawnPos);

protected:

	//---- メンバ変数 ----

	//ダメージ数字の表示
	std::weak_ptr<HitDamage>m_wpHitDamage;

	//UIの管理
	std::weak_ptr<UIManager>m_wpUIManager;

	//プレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayer;

	//マップ
	std::weak_ptr<MapManager>m_wpMapManager;

	//カメラ
	std::weak_ptr<CameraBase>m_wpCamera;

	//ゲームシーン
	std::weak_ptr<GameScene>m_wpGameScene;

	//雑魚敵リスト
	std::list<std::shared_ptr<EnemyBase>> m_enemyList;

	//ボス敵
	std::shared_ptr<EnemyBase> m_spEnemyBoss;

	//更新する(プレイヤーの近くの)敵のリスト
	std::list<std::shared_ptr<EnemyBase>> m_enemyUpdateList;

	//敵の管理(自分)
	std::weak_ptr<EnemyManager>m_wpEnemyManager;

	//---- 設定 ----

	//全部の敵にプレイヤーをセットする
	void SetEnemyListPlayer();
};
