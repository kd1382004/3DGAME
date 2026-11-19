#pragma once
#include"../CharacterBase.h"
#include"../../Weapon/ComboStep/ComboStep.h"

class CameraBase;
class GameScene;

//次の階シリーズ
class NextFloorAction;
class NextFloorGaugeUI;

//インベントリ
class PlayerInventory;

//バフ
class PlayerBuffManager;

//アクセサリー
class PlayerAccessory;

//UI
class UIManager;
class UIMap_Player;
class SkillGage;

//武器
class WeaponBase;

//エフェクト
class EffectManager;
class DamageOverlay;

//LV
class PlayreLV;

//操作ごとに割り当てるキー
struct ActionKeyConfig
{
	// Z+に移動
	int m_moveForward = 'W';

	// Z-に移動
	int m_moveBackward = 'S';

	// X+に移動
	int m_moveRight = 'D';

	// X-に移動
	int m_moveLeft = 'A';

	//ジャンプ
	int m_jump = VK_SPACE;

	//ダッシュ
	int m_dash = VK_SHIFT;

	//回避
	int m_evasion = VK_RBUTTON;

	//攻撃
	int m_attack = VK_LBUTTON;

	//インタラクト
	int m_interact = 'E';
};

struct Inventory;

class PlayerBase :public CharacterBase
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	PlayerBase() {};

	//デストラクタ
	~PlayerBase() {};

	//---- 基本の処理 ----

	//初期化
	void Init()override;

	//更新前の処理
	void PreUpdate()override;

	//更新
	void Update()override;

	//更新後の処理
	void PostUpdate()override;

	//影の描画
	void GenerateDepthMapFromLight()override;

	//---- 取得 ----

	//ポーションのインベントリリストを取得
	const std::vector<Inventory>& GetPotionsInventoryList();

	//武器強化素材のインベントリリストを取得
	const std::vector<Inventory>& GetWeaponsStrengtheningInventoryList();

	// 今使っているコンボファイルのパス
	const std::string& GetComboFilePath() const { return m_comboFilePath; }

	// プレイヤーかどうか（ダメージ数字の色分けに使う）
	bool IsPlayer() const override { return true; }

	//キー設定を返す
	//戻り値 ... キー設定
	const ActionKeyConfig GetActionKeyConfig() { return m_keyConfig; }

	//インベントリ
	std::shared_ptr<PlayerInventory> GetPlayerInventory() { return m_spPlayerInventory; }

	//バフ
	std::shared_ptr<PlayerBuffManager> GetPlayerBuffManager() { return m_spPlayerBuffManager; }

	//今のレベルを返す
	//戻り値 ... 今のレベル
	int GetLV() { return m_status.m_LV; }

	//ボーンの位置(ワールド)を返す
	// _boneName ... ボーンの名前
	Math::Vector3 GetBonePosition(std::string _boneName);

	//回転を返す
	float GetAngle() { return m_angle; }

	//持っている武器を返す
	//戻り値 ... 持っている武器
	std::weak_ptr<WeaponBase> GetWeapon() { return m_wpWeapon; }

	//ボーンのワールド行列を返す
	// _boneName ... ボーンの名前
	//戻り値 ... ボーンのワールド行列
	Math::Matrix GetBoneWorldMatrix(const std::string& _boneName);

	//---- 設定 ----

	//ポーションのインベントリリストを設定
	// _potions ... 設定するポーションのリスト
	void SetPotionsInventoryList(const std::vector<Inventory>& _potions);

	//武器強化素材のインベントリリストを設定
	// _weapons ... 設定する武器強化素材のリスト
	void SetWeaponsStrengtheningInventoryList(const std::vector<Inventory>& _weapons);

	//ゲームシーンをセット
	// _spGameScene ... ゲームシーン
	void SetGameScene(const std::shared_ptr<GameScene>& _spGameScene);

	//次の階に行ける場所にいるかをセット
	// _flg ... 次の階に行ける場所にいるか
	void SetNextFloorActionFlg(bool _flg) { m_nextFloorActionFlg = _flg; }

	//次の階に行く長押しゲージのUIをセット
	// _spNextFloorGaugeUI ... 長押しゲージのUI
	void SetNextFloorGaugeUI(const std::shared_ptr<NextFloorGaugeUI>& _spNextFloorGaugeUI);

	//武器を持たせる(コンボも読み込む)
	// _spWeapon ... 持たせる武器
	void SetWeapon(const std::shared_ptr<WeaponBase>& _spWeapon);

	//座標をセット
	// _pos ... 座標
	void SetPos(const Math::Vector3& _pos)override;

	//UIの管理にプレイヤーのUIを登録する
	// _spUIManager ... UIの管理
	void AddUIList(std::shared_ptr<UIManager>_spUIManager);

	//敵に見つかっているかをセット
	// _flg ... 見つかっているか
	void SetIsDetectedByEnemy(bool _flg)
	{
		m_isDetectedByEnemy = _flg;
		if (_flg)
		{
			m_isDetectedByEnemyNum++;
		}
	}

	//エフェクトマネージャー
	// _spEffectManager ... エフェクトマネージャー
	void SetEffectManager(std::shared_ptr<EffectManager>_spEffectManager);

	//HPMax増加
	//_flg...増加分体力を回復するか
	// _boost ... 増やす量
	void AddMaxHP(int _boost, bool _flg = false);

	//死んだときの処理
	void SetDead()override;

	//レベルをセット
	// _LV ... レベル
	void SetLV(int _LV)override;

	//_gainedExp...経験値量
	void AddExp(int _gainedExp);

	//アクセサリーの管理をセット
	// _spAccessoryManager ... アクセサリーの管理
	void SetAccessoryManager(std::shared_ptr<AccessoryManager>_spAccessoryManager)override;

	//武器の種類に合わせて攻撃アニメーションを切り替える
	// _info ... 武器の種類(PlayerWeaponInfo)
	void SetPlayerWeaponInfo(int _info);

	//---- その他の処理 ----

	//かかっているバフを全部消す
	void ClearAllBuffs();

	// コンボファイルを読み直す（エディターで保存した直後に呼ぶ）
	void ReloadCombo();

	//攻撃をくらった処理
	//_damage ... ダメージ量
	//_knockbackDistance ... ふっとばし距離
	//_knockbackDir ... ふっとばし方向
	// _hitStunTime ... のけぞり時間
	//_isCritical ... クリティカルかどうか
	//_ignoreRate ... 防御無視
	// _hitPos ... 当たった位置(無ければnullptr)
	void OnAttackHit(float _damage, float _knockbackDistance, const Math::Vector3& _knockbackDir, float _hitStunTime, bool _isCritical, float _ignoreRate, const Math::Vector3* _hitPos = nullptr)override;

	//HPを回復する
	// _heal ... 回復する量
	void HPHeal(int _heal);

	// インベントリ表示時などのSE全停止処理
	void StopSound();

	//Effect
	void PlayAttackBoostEffect();

	//攻撃力アップのエフェクトを止める
	void StopAttackBoostEffect();

protected:

	//---- 型 ----

	//移動モード
	enum MoveMode
	{
		//止まってる
		MoveStop,

		//歩いてる
		MoveWalk,

		//走ってる
		MoveRun

	};

	//アニメーションモード
	struct PlayerAnimeName
	{
		//待機アニメーション
		std::string m_idleAnime = "Idle";
		//歩きアニメーション
		std::string m_walkAnime = "Walk";
		//走りアニメーション
		std::string m_runAnime = "Run";
		//拾うアニメーション
		std::string m_pickUpAnime = "PickUp";

		//今の武器の攻撃アニメーション
		std::string m_attackAnime = "";

		//ダガーの攻撃アニメーション
		std::string m_daggerAttackAnime = "Dagger_Attack";

		//剣の攻撃アニメーション
		std::string m_swordAttackAnime = "Sword_Swing";


		//素手の攻撃アニメーション
		std::string m_punchAttackAnime = "Punch";
		//回避アニメーション
		std::string m_evasionAnime = "Roll";
		//死亡アニメーション
		std::string m_deathAnime = "Death";
	};

	//プレイヤーのアニメーションの種類
	enum PlayerAnimeMode
	{
		IdleAnime,
		WalkAnime,
		RunAnime,
		PickUpAnime,
		SwordAttackAnime,
		PunchAttackAnime,
		RollAnime,
		DeathAnime
	};

	//---- メンバ変数 ----

	//レベルの管理
	std::shared_ptr<PlayreLV>m_spPlayerLV;

	//エフェクトマネージャー
	std::weak_ptr<EffectManager> m_wpEffectManager;

	//ダメージを受けたときの画面の演出
	std::shared_ptr<DamageOverlay>m_spDamageOverlay;

	//インベントリ
	std::shared_ptr<PlayerInventory> m_spPlayerInventory;

	//敵に見つかってるか
	bool m_isDetectedByEnemy = false;

	//今フレーム敵身見つかった数
	int m_isDetectedByEnemyNum = 0;

	//ゲームシーン
	std::weak_ptr<GameScene> m_wpGameScene;

	//武器
	std::weak_ptr<WeaponBase> m_wpWeapon;

	//Keyコンフィグ

	//Keyの設定が入ってるファイル名
	std::string m_keyConfigFilePath;

	//キー設定
	ActionKeyConfig m_keyConfig;

	//今の移動の状態
	MoveMode m_moveMode = MoveStop;

	//ジャンプ力(+の値)
	float m_jumpPower = 0.0f;

	//ジャンプフラグ
	//今飛べる状態にいるかどうか
	bool m_jumpFlg = true;

	//アクション

	//次の階に
	std::shared_ptr<NextFloorAction>m_spNextFloorAction;

	//次の階アクションをしていいかどうか
	bool m_nextFloorActionFlg = false;

	//アニメーションの名前
	PlayerAnimeName m_playerAnimeName;

	//今のアニメーションの状態
	PlayerAnimeMode m_nowPlayerAnimeMode = IdleAnime;

	//前のアニメーションの状態
	PlayerAnimeMode m_oldPlayerAnimeMode = IdleAnime;

	//UI
	std::weak_ptr<UIMap_Player>m_wpUIMap_Player;

	//スキルゲージ
	std::weak_ptr<SkillGage>m_wpSkillGauge;

	//回避
	bool m_evasionAnimeFlg = false;

	//回避中か
	bool m_evasionFlg = false;

	//回避に使うスタミナ
	float m_evasionCost = 10;

	//回避の最高速度
	float m_evasionSpeed = 10;

	//回避の加速が終わる割合(0〜1)
	const float kEvasionAccelEnd = 0.3f;

	//回避の最高速度が終わる割合(0〜1)
	const float kEvasionMaxSpeedEnd = 0.8f;

	//通常攻撃
	bool m_normalAttack = false;

	//通常攻撃中か
	bool m_nowNormalAttack = false;

	//通常攻撃の待ち時間(最大)
	float m_normalAttackWaitMax = 1;

	//通常攻撃の待ち時間(今)
	float m_normalAttackWaitNow = 0;

	//溜め攻撃の溜めている時間
	float m_chargeAttackCount = 0;

	//溜め攻撃中か
	bool m_chargeAttackFlg = false;

	//バフ
	std::shared_ptr<PlayerBuffManager>m_spPlayerBuffManager;

	//アクセサリー
	std::shared_ptr<PlayerAccessory>m_spPlayerAccessory;

	//死亡アニメーションが終わったかどうか
	bool m_isDeadAnime = false;

	//ゲームシーンに死亡アニメーションが終わったかどうかを送ったかどうか
	bool m_isGameSceneOut = false;

	//砂のエフェクトのファイル
	std::string m_sunaNum = "Suna/Suna.efkefc";

	//音
	std::shared_ptr<KdSoundInstance> m_spDamageSE;

	//回避の音
	std::shared_ptr<KdSoundInstance> m_spRollSE;

	//ジャンプの音
	std::shared_ptr<KdSoundInstance> m_spJumpSE;

	//Effect
	std::weak_ptr<KdEffekseerObject> m_wpHealEffect;

	//攻撃力アップのエフェクト
	std::weak_ptr<KdEffekseerObject> m_wpAttackBoostEffect;

	//攻撃力アップのエフェクトが出ているか
	bool m_isAttackBoostActive = false;

	//ダッシュのエフェクト
	std::weak_ptr<KdEffekseerObject> m_wpDashEffect;

	//ダッシュのエフェクトが出ているか
	bool m_isDashActive = false;

	//ダッシュできる時はするかどうか
	bool m_isDash = false;

	//今の武器のコンボ
	std::vector<ComboStep> m_attackCombo;

	// コンボファイルのフォルダー（持ち主ごと）と、今使っているファイルのパス
	std::string m_comboFolder = "Asset/Data/ObjeData/Combo/Player/";

	//コンボのファイルのパス
	std::string m_comboFilePath;
	int   m_comboStep = -1;        // -1 = コンボしていない
	bool  m_comboReserved = false;
	float m_stepForwardDone = 0.0f;   // この段で踏み込んだ距離（m）
	float m_animeSpeedRate = 1.0f;

	//---- その他の処理 ----

	//武器の更新
	void WeaponUpdate();

	//_filePath ... ファイルパスを書く
	void LoadKeyConfig(std::string _filePath);

	//キー設定をファイルに保存する
	// _filePath ... 保存するファイルのパス
	void SaveKeyConfig(std::string _filePath);

	//移動
	void Move();

	//m_status.moveSpeed.nowSpeedを求める
	void MoveNowSpeedDecision();

	//ジャンプ&重力処理
	void JumpAndGravity();

	//アニメーションの状態が変わったらアニメーションを切り替える
	void PlayerAnimeModeUpdate();

	//回避の更新
	void EvasionUpdate();

	//コンボの段を始める
	// _step ... 始める段の番号
	void StartComboStep(int _step);

	//コンボの更新
	void ComboUpdate();

	//一番近い敵の方を向く
	// _spWeapon ... 持っている武器
	void AimToNearestEnemy(const std::shared_ptr<WeaponBase>& _spWeapon);

	//コンボをやめる
	void CancelCombo();
};
