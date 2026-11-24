#pragma once
#include"../CharacterBase.h"
#include"../../Weapon/ComboStep/ComboStep.h"
class PlayerBase;
class MapManager;
class GameScene;
class EnemyCrowdController;
class EnemyManager;
class AttackGage;
class AttackShape;


struct Node;


//---- 型(攻撃パターン) ----

	//段の当たり判定の形
enum class AttackHitShape
{
	Bone,		// m_hitNodes の骨に付ける
	Circle,		// 地面に円で出す
	Box,		// 敵の前に箱で出す
};

//段の中での動き方
enum class AttackMoveType
{
	None,			// 動かない(m_stepForward の踏み込みだけ)
	Step,			// m_moveStart〜m_moveEnd の間、前へ進む
	LeapToTarget,	// 着地点へ飛ぶ
};

//段のイベントの種類
enum class AttackEventType
{
	Effect,			// エフェクトを出す
	Sound,			// 音を鳴らす
	CameraShake,	// 画面を揺らす
};

//段のイベント1つ分
struct AttackEvent
{
	float           m_progress = 0.0f;					// 出す進行度(ファイルでは "p")
	AttackEventType m_type = AttackEventType::Effect;	// 種類(ファイルでは "type")
	std::string     m_name;								// エフェクト・音のパス(ファイルでは "name")
	float           m_power = 0.0f;						// 揺れの強さ(ファイルでは "power")
	float           m_time = 0.0f;						// 揺れの時間(秒)(ファイルでは "time")
};

//段ごとの特別な設定(何も書いていない段は普通の単発と同じ)
struct AttackStepExtra
{
	AttackMoveType           m_moveType = AttackMoveType::None;	// 動き方
	float                    m_aimLockEnd = 0.3f;				// この進行度まで着地点をプレイヤーに合わせる
	float                    m_moveStart = 0.0f;				// 動き始める進行度
	float                    m_moveEnd = 0.3f;					// 動き終わる進行度
	bool                     m_cancelRootMotion = false;		// アニメ自体の前後の動きを打ち消すか
	AttackHitShape           m_hitShape = AttackHitShape::Bone;	// 当たり判定の形
	float                    m_hitRadius = 1.0f;				// Bone・Circle の半径
	Math::Vector3            m_hitBoxSize = { 1.0f, 1.0f, 1.0f };	// Box の大きさの半分(横・高さ・奥行き)
	float                    m_hitForward = 0.0f;				// Circle・Box を敵の前何mに出すか(LeapToTarget のときは着地点に出すので使わない)
	float                    m_warnStart = 0.0f;				// 予告を出し始める進行度(Circle・Box のとき)
	std::vector<AttackEvent> m_events;							// エフェクト・音・画面の揺れ
	std::string              m_specialType;						// 一品物の名前(空なら使わない)
};

//パターン1つ分
struct AttackPattern
{
	std::string                  m_name;					// 名前
	std::string                  m_comboFilePath;			// 単発を並べたコンボファイル
	std::vector<ComboStep>       m_steps;					// コンボファイルから読んだ段の並び
	float                        m_minDistance = 0.0f;		// 出す距離(下限)
	float                        m_maxDistance = 5.0f;		// 出す距離(上限)
	int                          m_minLV = 1;				// この敵のLVがこれ以上で出る
	float                        m_weight = 1.0f;			// 出やすさ
	float                        m_cooldown = 0.0f;		// 使った後、また使えるまでの秒数
	float                        m_cooldownTimer = 0.0f;	// クールタイムの残り(ファイルには無い)
	float                        m_waitAfter = 0.0f;		// 終わった後の待ち(0なら m_attackWaitMax)
	std::vector<std::string>     m_hitNodes;				// 段ごとの当たり判定の骨
	std::vector<AttackStepExtra> m_stepExtras;			// 段ごとの特別な設定
};



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

//敵の基底クラス
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

	//攻撃パターンを読み直す(エディターで保存したときに呼ぶ。パターンを使わない敵は何もしない)
	void ReloadAttackPatterns();

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

	//敵同士の押し合い・群れの制御
	std::shared_ptr<EnemyCrowdController> m_spCrowdController;

	//この敵の攻撃ゲージ
	std::weak_ptr<AttackGage>m_wpAttackGauge;

	//攻撃ゲージの溜まり具合(0〜1)
	float m_attackGaugePercent = 0.0f;

	//視界を表示するか
	bool m_isVisionFlg = false;

	//体の質感（ヒットエフェクトの種類を決める）
	EnemyMaterial m_material = EnemyMaterial::Flesh;

	//倒したときに落とすコインの基本の額(LV1の額。LVが上がると増える)
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


	//---- 攻撃パターン ----

	//m_patternFilePath のパターン一覧を読んで m_patternList に入れる
	//戻り値 ... 1つでも読めたらtrue
	bool LoadAttackPatterns();

	//全パターンのクールタイムを減らす(進め方5で使う)
	void UpdatePatternCooldown();

	//距離・LV・重み・クールタイムで次のパターンを選ぶ(進め方5で使う)
	// _distance ... プレイヤーとの距離
	// _ignoreCooldown ... trueならクールタイム中のパターンも候補に入れる(普通に選べなかったときの2回目用)
	//戻り値 ... 選んだパターンの番号(選べなければ-1)
	int SelectPattern(float _distance, bool _ignoreCooldown = false);

	//今のパターン(攻撃していなければnullptr)
	const AttackPattern* GetNowPattern() const;

	//今の段(攻撃していなければnullptr)
	const ComboStep* GetNowStep() const;

	//今の段の特別な設定(攻撃していなければnullptr)
	const AttackStepExtra* GetNowStepExtra() const;

	//今の段の当たり判定を付ける骨の名前(なければ空)
	std::string GetNowHitNode() const;

	//今の段の進行度(0〜1)
	float GetNowStepProgress() const;

	//今の段が最後の段か
	bool IsLastStep() const;

	//段の特別な設定をjsonから作る(書いていない項目は初期値)
	// _j ... 段1つ分のjson
	//戻り値 ... 段の特別な設定
	static AttackStepExtra StepExtraFromJson(const nlohmann::json& _j);


	//プレイヤーの方をすぐ向く(m_angle と m_moveVec をプレイヤーへの向きにする)
	void FaceToPlayer();

	//今の段の踏み込み(m_stepForward)を進める
	// _progress ... 今の段の進行度(0〜1)
	void UpdateStepForward(float _progress);

	//パターン攻撃の毎フレームの処理(待ち時間・選ぶ・段を進める・攻撃ゲージ)
	//各敵の Update から呼ぶ
	void UpdatePatternAttack();

	//パターンを始める
	// _patternIndex ... m_patternList の番号
	void StartPattern(int _patternIndex);

	//段を始める(向き直し・アニメの切り替え・当たり判定の作り直し)
	// _stepIndex ... 何段目か(0が1段目)
	void StartStep(int _stepIndex);

	//攻撃中の毎フレームの処理(移動・当たり判定・イベント・次の段へ・終わり)
	void UpdateStep();

	//パターンを終わる(クールタイム・待ち時間を入れて走りに戻す)
	void EndPattern();

	//パターンをこの場でやめる(当たり判定を消して番号を-1に戻すだけ。アニメも待ち時間も触らない)
	//死んだとき・見失ったとき・解放するときに使う
	void CancelPattern();

	//今の段の当たり判定の中心(形と動き方で決まる)
	//戻り値 ... 中心の位置
	Math::Vector3 GetStepHitCenter() const;

	//今の段のイベント(エフェクト・音・画面の揺れ)を出す
	// _progress ... 今の段の進行度(0〜1)
	void UpdateStepEvents(float _progress);

	//今の段の飛びかかり(LeapToTarget)を進める
	// _progress ... 今の段の進行度(0〜1)
	void UpdateStepLeap(float _progress);

	//パターンを始めたときに呼ばれる(叫び声など。敵ごとに override する)
	virtual void OnPatternStart() {}

	//m_specialType が入っている段で、毎フレーム呼ばれる(一品物の攻撃。敵ごとに override する)
	// _specialType ... 段の m_specialType
	// _progress ... 今の段の進行度(0〜1)
	virtual void UpdateSpecialStep(const std::string& _specialType, float _progress) {}



	//---- 攻撃パターン ----

	//パターン一覧ファイルのパス(空ならパターンを使わない。各敵の Init で入れる)
	std::string m_patternFilePath;

	//パターンの一覧
	std::vector<AttackPattern> m_patternList;

	//今のパターンの番号(攻撃していなければ-1)
	int m_nowPatternIndex = -1;

	//今の段の番号(攻撃していなければ-1)
	int m_nowStepIndex = -1;

	//前に使ったパターンの番号(同じ技を続けないため。進め方5で使う)
	int m_lastPatternIndex = -1;

	//この段で当たり判定を出したか
	bool m_hitOn = false;

	//この段で踏み込んだ距離(進め方4で使う)
	float m_stepForwardDone = 0.0f;

	//待ち始めたときの待ち時間(攻撃ゲージ用。進め方5で使う)
	float m_attackWaitNowMax = 0.0f;

	//この段のイベントを出したか(m_events と同じ数。進め方7で使う)
	std::vector<bool> m_eventDone;

	//跳んだ位置(LeapToTarget 用)
	Math::Vector3 m_leapStartPos = Math::Vector3::Zero;

	//着地点(LeapToTarget 用)
	Math::Vector3 m_leapTargetPos = Math::Vector3::Zero;

	//アニメの再生速度の倍率(段の m_speed。攻撃していないときは1)
	float m_animeSpeedRate = 1.0f;

	//ひるみ値(攻撃中に受けたダメージの合計。パターンを始めるときに0に戻す)
	float m_flinchValue = 0.0f;

	//ひるむ割合(攻撃中に、最大HPのこの割合以上のダメージを受けたら攻撃が止まる。0以下ならひるまない)
	float m_flinchRate = 0.15f;

	//パターン攻撃中か
	bool m_attackFlg = false;

	//パターン攻撃の当たり判定
	std::weak_ptr<AttackShape> m_wpPatternAttack;

	//攻撃待機時間(秒。m_waitAfter が0のパターンの後に使う)
	float m_attackWaitMax = 5.0f;

	//次の攻撃までの待機時間(秒)
	float m_attackWait = 5.0f;

	//ふっとばし距離の基準(段の m_knockbackRate を掛ける)
	float m_patternKnockbackDistance = 1.0f;

	//アニメの前後の動きを打ち消す骨の名前(m_cancelRootMotion 用。空なら打ち消さない)
	std::string m_rootMotionNodeName;

	//前後の動きを打ち消し続ける残り時間(秒。攻撃が終わった後のクロスフェード用)
	float m_rootMotionCancelTimer = 0.0f;
};
