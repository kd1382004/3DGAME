#pragma once
#include"../CharacterBase.h"

class PlayerBase;
class MapManager;
class GameScene;
class EnemyCrowdController;
class EnemyManager;
class AttackGage;


struct Node;

//////////////////////////////////////////////////////////////
// 敵の体の質感
// 攻撃が当たったときに出すヒットエフェクトの種類を決めるのに使う
//   Flesh → Hit/Material/Flesh/Hit_Flesh.efkefc
//   Stone → Hit/Material/Stone/Hit_Stone.efkefc
//////////////////////////////////////////////////////////////
enum class EnemyMaterial
{
	Flesh,		// 肉・やわらかい体（スライム、獣など）… 血しぶき系
	Stone,		// 石・かたい体（ゴーレム、石像など）… 火花・破片系

	Num			// 種類の数（配列のサイズ・範囲チェック用。質感としては使わない）
};

class EnemyBase :public CharacterBase, public std::enable_shared_from_this<EnemyBase>
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	EnemyBase() {};

	//デストラクタ
	~EnemyBase() override { Release(); };

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

	//---- 取得 ----

	//向いている角度を返す
	//戻り値 ... 角度(度)
	float GetAngle() { return m_angle; }

	//プレイヤーの座標を返す
	//戻り値 ... プレイヤーの座標
	Math::Vector3 GetPlayerPos() { return m_playerPos; }

	//敵の管理を返す
	//戻り値 ... 敵の管理
	std::shared_ptr<EnemyManager> GetEnemyManager() { return m_wpEnemyManager.lock(); }

	//レベルを返す
	//戻り値 ... レベル
	int GetLV() const { return  m_status.m_LV; }

	//---- 設定 ----

	//出現位置をセット
	// _pos ... 出現位置
	void SetSpawnPos(const Math::Vector3& _pos);

	//プレイヤーをセット
	// _spPlayer ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayer) { m_wpPlayer = _spPlayer; };

	//マップの管理をセット
	// _spMapManager ... マップの管理
	void SetMapManager(std::shared_ptr<MapManager>_spMapManager) { m_wpMapManager = _spMapManager; };

	//ゲームシーンをセット
	// _spGameScene ... ゲームシーン
	void SetGameScene(std::shared_ptr<GameScene>_spGameScene) { m_wpGameScene = _spGameScene; };

	//UIの管理にHPバーなどを登録する
	// _spUIManager ... UIの管理
	void AddUIList(std::shared_ptr<UIManager>_spUIManager);

	//敵の管理をセット
	// _spEnemyManager ... 敵の管理
	void SetEnemyManager(std::shared_ptr<EnemyManager>_spEnemyManager) { m_wpEnemyManager = _spEnemyManager; }

	// レベル設定・取得
	// _lv ... レベル
	virtual void SetLV(int _lv)override;

	//移動方向をセット(正規化する)
	// _vec ... 移動方向
	void SetMoveVec(Math::Vector3 _vec)
	{
		m_moveVec = _vec;
		m_moveVec.Normalize();
	}

	//視界を表示するかをセット
	// _flg ... 表示するならtrue
	void SetVisionFlg(bool _flg) { m_isVisionFlg = _flg; }

	//---- その他の処理 ----

	//プレイヤーを探す
	void SearchPlayer();

	//プレイヤーを追いかける
	void PlayerChase();

	//攻撃をくらった処理
	//_damage ... ダメージ量
	//_knockbackDistance ... ふっとばし距離
	//_knockbackDir ... ふっとばし方向
	// _hitStunTime ... のけぞり時間
	//_isCritical ... クリティカルかどうか
	//_ignoreRate ... 防御無視
	// _hitPos ... 当たった位置(ダメージ数字を出す場所。nullptrなら頭の上)
	void OnAttackHit(float _damage, float _knockbackDistance, const Math::Vector3& _knockbackDir, float _hitStunTime, bool _isCritical, float _ignoreRate, const Math::Vector3* _hitPos = nullptr)override;

protected:

	//---- 型 ----

	//アニメーション
	enum EnemyAnimeMode
	{
		//待機
		EnemyAnimeMode_Idle,

		//歩き
		EnemyAnimeMode_Walk,

		//走り
		EnemyAnimeMode_Run,

		//攻撃
		EnemyAnimeMode_Attack,

		//死亡
		EnemyAnimeMode_Die
	};

	//---- メンバ変数 ----

	// レベルおよび基準ステータス
	Status m_baseStatus;

	//敵の管理
	std::weak_ptr<EnemyManager>m_wpEnemyManager;

	//マップマネージャー1
	std::weak_ptr<MapManager>m_wpMapManager;

	//プレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayer;

	//プレイヤー座標
	Math::Vector3 m_playerPos;

	// これより近づいたら止まる
	float m_playerChaseDir = 1;

	// 止まっているとき、これより離れたら走り出す
	float m_playerChaseResumeDir = 1.5f;

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

	//待機時間
	float m_stayTime = 0.0f;

	//プレイヤーを見逃したときの待機時間
	float m_lostSightWaitTime = 2;

	//目的地に着いた時の待機時間
	float m_arrivalWaitTime = 1;

	//経路探索
	std::vector<Node*> m_path;

	//今向かっている経路の番号
	int m_pathIndex = 0;

	//経路を探し直すまでの時間
	float m_repathTimer = 0.0f;

	//徘徊半径
	float m_wanderRadius = 0.0f;

	//今のアニメーションの状態
	EnemyAnimeMode m_enemyAnimeMode = EnemyAnimeMode_Idle;

	//アニメーション変更フラグ
	bool m_animeChangeFlg = false;

	//ゲームシーン
	std::weak_ptr<GameScene>m_wpGameScene;
	std::shared_ptr<EnemyCrowdController> m_spCrowdController;

	//適用アッタックゲージ
	std::weak_ptr<AttackGage>m_wpAttackGauge;

	//攻撃ゲージの溜まり具合(0〜1)
	float m_attackGaugePercent = 0.0f;

	//視界を表示するか
	bool m_isVisionFlg = false;

	//体の質感（ヒットエフェクトの種類を決める）
	EnemyMaterial m_material = EnemyMaterial::Flesh;


	int m_coin = 100;

private:

	//視錐台用のBoxInfo
	KdCollider::BoxInfo m_frustumBox;

	//カメラに映っているか
	bool m_isInView = false;

protected:

	//---- 基本の処理 ----

	//解放
	virtual void Release()override;

	//---- 設定 ----

	//死んだときの処理
	void SetDead();

	//攻撃ゲージの溜まり具合をセット
	// _percent ... 溜まり具合(0〜1)
	void SetAttackGaugePercent(float _percent);

	//---- その他の処理 ----

	//レベルに合わせてステータスを決める
	virtual void ApplyLevelStatus();

	//状態に合わせてアニメーションを変える
	void EnemyAnimeModeUpdate();

	//HPバーの位置を更新する
	void HPPosPostUpdate();

	// 壁などの遮蔽物を考慮した視界スポットライトの有効照射距離を計算
	// _eyePos ... 目の位置
	// _dir ... 見ている向き
	// _maxDist ... 見える最大の距離
	// _halfAngleDeg ... 視界の半分の角度(度)
	float CalcSightLightDistance(const Math::Vector3& _eyePos, const Math::Vector3& _dir, float _maxDist, float _halfAngleDeg);

	//出現位置に戻る
	void ReturnSpawnPos();

	// 質感（m_material）に合わせた効果音を1回鳴らす
	// _folder  : "Asset/Sound/SE/Hit/" など（中に Flesh.wav / Stone.wav がある）
	// _volRate : 音量の倍率
	void PlayMaterialSE(const std::string& _folder, float _volRate);
};
