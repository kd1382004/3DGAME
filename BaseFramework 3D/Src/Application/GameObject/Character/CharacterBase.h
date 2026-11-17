#pragma once

class CameraBase;
class UIManager;
class HPBar;
class HitDamage;
class AccessoryManager;
class StaminaGage;


struct StatusHP
{
	//基礎体力
	float m_baseHP = 0;

	//最大体力
	float m_maxHP = 0;

	//現在体力
	float m_nowHP = 0;
};

struct StatusAttck
{
	//基礎攻撃力
	float m_baseAttackPower = 0;

	//現在攻撃力
	float m_nowAttack = 0;

	//増加攻撃力(期間限定の攻撃力アップの数値などを入れる)
	float m_addAttack = 0;
};

struct StatusDefense
{
	//基礎防御力
	float m_baseDefensePower = 0;

	//現在防御力
	float m_nowDefense = 0;

	//増加防御力(期間限定の防御力アップの数値などを入れる)
	float m_addDefense = 0;
};

struct StatusMoveSpeed
{
	//基礎速度
	float m_baseSpeed = 0;

	//歩き速度(基礎速度に足す値)
	float m_walkMovePower = 0;

	//ダッシュ速度(基礎速度に足す値)
	float m_runMovePower = 0;

	//現在の速度
	float m_nowSpeed = 0;
};

struct Stamina
{
	//スタミナ
	float m_staminaMax = 100;
	//今のスタミナ
	float m_staminaNow = m_staminaMax;

	//dashでの減少量(1秒あたり)
	float m_dashStaminaDrainPerSec = 10;

	//自然回復速度(1秒あたり)
	float m_staminaRegenPerSec = 1;

	//バフによる回復速度増加量(1秒あたり)
	float m_staminaRegenBuff = 0;


	//スタミナが自然回復しているか
	bool m_staminaRegenFlg = false;
};

struct Status
{
	//体力
	StatusHP m_HP;

	// 攻撃力
	StatusAttck m_attack;

	//防御力
	StatusDefense m_defense;

	//スピード
	StatusMoveSpeed m_moveSpeed;


	//スタミナ
	Stamina m_stamina;

	//LV
	int m_LV;
};

class CharacterBase : public KdGameObject
{
public:
	//コンストラクタ
	CharacterBase() {};
	//デストラクタ
	~CharacterBase()	override { Release(); };

	//初期化
	void Init()			override;
	//更新
	void Update()		override;
	//更新後の処理
	void PostUpdate()	override;

	//影の描画
	void GenerateDepthMapFromLight()override;
	//ライトありの描画
	void DrawLit()		override;

	//ステータスを編集するImGui
	void ImGUI()override;


	//_filePath ... ファイルパスを書く
	void LoadCharaStatus(std::string _filePath);
	//キャラのステータスをファイルに保存する
	// _filePath ... 保存するファイルのパス
	void SaveCharaStatus(std::string _filePath);

	//座標をセット
	// _pos ... 座標
	void SetPos(const Math::Vector3& _pos)override
	{
		m_pos = _pos;
		m_mWorld.Translation(m_pos);
	}


	//武器を持たせる位置の行列を取得
	//戻り値 ... 武器の親になる行列
	Math::Matrix GetWeaponParentMatrix()
	{
		return m_weaponParentMatrix;
	}

	//カメラをセット
	// _spCamera ... カメラ
	void SetCamera(const std::shared_ptr<CameraBase>& _spCamera) { m_wpCamera = _spCamera; }
	//カメラを取得
	//戻り値 ... カメラ(無ければnullptr)
	std::shared_ptr<CameraBase> GetCamera() const { return m_wpCamera.lock(); }

	// カメラを揺らす（カメラが無ければ何もしない）
	// _time ... 揺らす秒数
	// _power ... 揺らす強さ
	void CameraShake(float _time, float _power);

	// _pos で起きた揺れ。カメラから遠いほど弱くなり、_range より遠いと揺らさない
	// _time ... 揺らす秒数
	// _power ... 揺らす強さ
	void CameraShakeAt(const Math::Vector3& _pos, float _time, float _power, float _range);


	//攻撃をくらった処理
	//_damage ... ダメージ量
	//_knockbackDistance ... ふっとばし距離
	//_knockbackDir ... ふっとばし方向
	// _hitStunTime ... のけぞり時間
	//_isCritical ... クリティカルかどうか
	//_ignoreRate ... 防御無視
	//_hitPos ... 当たった位置（ダメージ数字を出す場所。nullptr ならキャラの頭の上）
	virtual void OnAttackHit(float _damage, float _knockbackDistance, const Math::Vector3& _knockbackDir, float _hitStunTime, bool _isCritical, float _ignoreRate, const Math::Vector3* _hitPos = nullptr);

	// プレイヤーかどうか（ダメージ数字の色分けに使う）
	virtual bool IsPlayer() const { return false; }


	//UIの管理にHPバーなどを登録する
	// _spUIManager ... UIの管理
	virtual void AddUIList(std::shared_ptr<UIManager>_spUIManager);

	//ダメージ数字の表示をセット
	// _spHitDamage ... ダメージ数字の表示
	void SetHitDamage(std::shared_ptr<HitDamage>_spHitDamage) { m_wpHitDamage = _spHitDamage; }


	/////////////////////////////////////
	//HP

	//MaxのHPを返す
	int GetMaxHP() { return m_status.m_HP.m_maxHP; }
	//今のHPを返す
	//戻り値 ... 今のHP
	int GetNowHP() { return m_status.m_HP.m_nowHP; }



	//死んだときの処理
	virtual void SetDead();

	/////////////////////////////////////
	//攻撃力

	//基礎攻撃力アップ
	// _boost ... 増やす量
	void BoostAttackBase(int _boost) { m_status.m_attack.m_baseAttackPower += _boost; }

	//基礎攻撃力を返す
	int GetAttackBase() { return m_status.m_attack.m_baseAttackPower; }

	//増加攻撃力アップ
	// _boost ... 増やす量
	void BoostAddAttack(int _boost) { m_status.m_attack.m_addAttack += _boost; }

	//今の攻撃力(基礎+増加)を返す
	//戻り値 ... 今の攻撃力
	float NowAttack() { return m_status.m_attack.m_baseAttackPower + m_status.m_attack.m_addAttack; }

	/////////////////////////////////////
	//防御力

	//基礎防御力アップ
	// _boost ... 増やす量
	void BoostDefenseBase(int _boost) { m_status.m_defense.m_baseDefensePower += _boost; }

	//基礎防御力を返す
	int GetDefenseBase() { return m_status.m_defense.m_baseDefensePower; }

	//増加防御力アップ
	// _boost ... 増やす量
	void BoostAddDefense(int _boost) { m_status.m_defense.m_addDefense += _boost; }

	//今の防御力(基礎+増加)を返す
	//戻り値 ... 今の防御力
	float NowDefense() { return m_status.m_defense.m_baseDefensePower + m_status.m_defense.m_addDefense; }

	////////////////////////////////////////
	//速度

	//今の移動速度を返す
	//戻り値 ... 今の移動速度
	float GetNowSpeed() { return m_status.m_moveSpeed.m_nowSpeed; }

	//歩きの速度を返す
	//戻り値 ... 歩きの速度
	float GetWalkSpeed() { return m_status.m_moveSpeed.m_baseSpeed + m_status.m_moveSpeed.m_walkMovePower; }
	//基礎速度を返す
	//戻り値 ... 基礎速度
	float GetBaseSpeed() { return m_status.m_moveSpeed.m_baseSpeed; }

	///////////////////////////////////
	// スタミナ即座に回復
	// _amount ... 回復する量
	void StaminaHeal(float _amount);


	// スタミナ回復量アップ（永続）
	// _amount ... 増やす量(1秒あたり)
	void BoostStaminaRegenBase(float _amount)
	{
		m_status.m_stamina.m_staminaRegenPerSec += _amount;
	}

	// スタミナ回復量アップ（時間制限バフ）
	// _amount ... 増やす量(1秒あたり)
	void AddStaminaRegenBuff(float _amount)
	{
		m_status.m_stamina.m_staminaRegenBuff += _amount;
	}

	// スタミナ量増加（最大スタミナ永続増加）
	// _boost ... 増やす量
	void AddMaxStamina(float _boost);



	//最大スタミナを返す(GetStaminaMaxと同じ)
	//戻り値 ... 最大スタミナ
	float GetMaxMP() { return m_status.m_stamina.m_staminaMax; }
	//最大スタミナを返す
	//戻り値 ... 最大スタミナ
	float GetStaminaMax() { return m_status.m_stamina.m_staminaMax; }
	//今のスタミナを返す
	//戻り値 ... 今のスタミナ
	float GetStaminaNow() { return m_status.m_stamina.m_staminaNow; }
	//スタミナの回復速度(1秒あたり、バフ込み)を返す
	//戻り値 ... 回復速度
	float GetStaminaRegen() { return m_status.m_stamina.m_staminaRegenPerSec + m_status.m_stamina.m_staminaRegenBuff; }

	///////////////////////////////////

	//レベルをセット(継承先で使う)
	// _lv ... レベル
	virtual void SetLV(int _lv) {};

	//HPバー
	std::shared_ptr<HPBar> GetHPBar() { return m_wpHPBar.lock(); }



	///////////////////////////////////////////

	//アクセサリーの管理をセット
	// _spAccessoryManager ... アクセサリーの管理
	virtual void SetAccessoryManager(std::shared_ptr<AccessoryManager>_spAccessoryManager) { m_wpAccessoryManager = _spAccessoryManager; }


	//死んでいるか
	//戻り値 ... 死んでいるならtrue
	bool GetIsDead() { return m_isDead; }

	//DefaultStatusが入ってるファイルパスを返す
	std::string GetCharaDefaultStatusFilePath() { return m_charaDefaultStatusFilePath; }
private:
	// 衝突判定とそれに伴う座標の更新
	void CollisionUpdate();

	// 解放処理
	void Release();

	//当たり判定をする半径(この円ないならあたり判定)
	const float m_detectRange = 20;


protected:

	//重力をかける
	void ApplyGravity();

	//ダメージ計算式
	//ダメージ量を返す
	// _damage ... 元のダメージ量
	// _ignoreRate ... 防御無視の割合
	float DamageCalculationFormula(float _damage, float _ignoreRate);

	//キャラのモデル
	std::shared_ptr<KdModelWork> m_spCharaModel = nullptr;
	//アニメーション
	std::shared_ptr<KdAnimator>	m_spAnimator = nullptr;


	//ダメージ数字の表示
	std::weak_ptr<HitDamage>m_wpHitDamage;


	//今の落ちる速さ
	float m_gravity = 0;


	//重力の強さ
	float m_gravityPower = 432.0f;

	///////////////////////////////////////////
	//キャラステータス

	//キャラのデフォルトステータスが入ってるファイル名
	std::string m_charaDefaultStatusFilePath;

	//スタミナ減少
	// _amount...実際に減らす量
	//戻り値 ... _amount分減少できたかどうか
	bool ConsumeStamina(float _amount);

	//スタミナの減少・回復を管理する
	void StaminaManager();
	//スタミナゲージ
	std::weak_ptr<StaminaGage>m_wpStaminaGauge;

	//ステータス
	Status m_status;
	///////////////////////////////////////////

	///////////////////////////////////////////

	//座標
	Math::Vector3 m_pos;

	//サイズ
	float m_siz;

	//移動方向
	Math::Vector3 m_moveVec;

	///////////////////////////////////////////

	///////////////////////////////////////////
	//キャラの回転
	//角度
	float m_angle = 0;

	//移動方向に向きを合わせる
	void AngleUpdate();

	///////////////////////////////////////////

	///////////////////////////////////////////
	//ステータスエディターに表示する
	virtual void StatusEditor();

	//ステータスエディタを開いているか
	bool m_statusEditorFlg = false;

	//ステータスエディタに出す名前
	std::string m_statusEditorName = "キャラクター";

	///////////////////////////////////////////

	///////////////////////////////////////////

	//地面と当たってるか
	bool m_groundHit = false;

	//壁と当たってるか
	bool m_wallHit = false;

	///////////////////////////////////////////
	//武器用

	//武器を持たせる位置の行列
	Math::Matrix m_weaponParentMatrix;

	///////////////////////////////////////////
	//カメラ
	std::weak_ptr<CameraBase>m_wpCamera;



	///////////////////////////
	//のけぞり時間
	float m_hitStunTimer;
	//のけぞり中か
	bool m_hitStunFlg = false;

	/////////////////////////////
	//ふっとばし
	Math::Vector3 m_knockbackStartPos;
	//ふっとばしの終わりの位置
	Math::Vector3 m_knockbackEndPos;
	//ふっとばされ中か
	bool m_isKnockbackFlg = false;


	//何秒で飛ばしきるか
	float m_knockbackSpeed = 1;
	//ふっとばしの更新
	void UpdateKnockback();

	//ふっとばしの進み具合(0〜1)
	float m_knockbackProgress = 0;


	///////////////////////////////////

	//スイープ判定
	//当たったかどうかを返す(座標の補正もおこなっている)
	// _nextPos ... 次に移動する座標
	bool RaycastFromTo(Math::Vector3 _nextPos);

	///////////////////////////////////
	//UI
	//HPBar
	std::weak_ptr<HPBar>m_wpHPBar;

	//死んでいるか
	bool m_isDead = false;


	////////////////////////////////
	std::weak_ptr<AccessoryManager>m_wpAccessoryManager;

	//音
	std::shared_ptr<KdSoundInstance> m_spWalkSE;
	//ダッシュの音
	std::shared_ptr<KdSoundInstance> m_spDashSE;
};
