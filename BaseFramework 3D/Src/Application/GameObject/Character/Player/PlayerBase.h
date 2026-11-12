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

struct ActionKeyConfig
{
	// Z+に移動
	int moveForward = 'W';

	// Z-に移動
	int moveBackward = 'S';

	// X+に移動
	int moveRight = 'D';

	// X-に移動
	int moveLeft = 'A';

	//ジャンプ
	int jump = VK_SPACE;

	//ダッシュ
	int dash = VK_SHIFT;

	//回避
	int evasion = VK_RBUTTON;

	//攻撃
	int attack = VK_LBUTTON;

	//インタラクト
	int interact = 'E';
};

struct Inventory;

class PlayerBase :public CharacterBase
{
public:

	//ポーションのインベントリリストを取得
	const std::vector<Inventory>& GetPotionsInventoryList();
	//武器強化素材のインベントリリストを取得
	const std::vector<Inventory>& GetWeaponsStrengtheningInventoryList();

	//ポーションのインベントリリストを設定
	void SetPotionsInventoryList(const std::vector<Inventory>& _potions);
	//武器強化素材のインベントリリストを設定
	void SetWeaponsStrengtheningInventoryList(const std::vector<Inventory>& _weapons);


	void ClearAllBuffs();

	PlayerBase() {};
	~PlayerBase() {};

	void Init()override;
	void PreUpdate()override;
	void Update()override;
	void PostUpdate()override;

	void GenerateDepthMapFromLight()override;


	void SetGameScene(const std::shared_ptr<GameScene>& _GameScene);

	void SetNextFloorActionFlg(bool _flg) { m_nextFloorActionFlg = _flg; }
	void SetNextFloorGaugeUI(const std::shared_ptr<NextFloorGaugeUI>& _NextFloorGaugeUI);

	void SetWepon(const std::shared_ptr<WeaponBase>& _wepon);

	// 今使っているコンボファイルのパス
	const std::string& GetComboFilePath() const { return m_comboFilePath; }

	// コンボファイルを読み直す（エディターで保存した直後に呼ぶ）
	void ReloadCombo();

	void SetPos(const Math::Vector3& pos)override;


	void AddUIList(std::shared_ptr<UIManager>_spUIManager);


	//攻撃をくらった処理
	//_damage ... ダメージ量
	//_knockbackDistance ... ふっとばし距離
	//_knockbackDir ... ふっとばし方向
	// _hitStunTime ... のけぞり時間
	//_isCritical ... クリティカルかどうか
	//__ignoreRate ... 防御無視
	void OnAttackHit(float _damage, float _knockbackDistance, const Math::Vector3& _knockbackDir, float _hitStunTime, bool _isCritical, float _ignoreRate)override;

	void SetIsDetectedByEnemy(bool _flg)
	{
		m_IsDetectedByEnemy = _flg;
		if (_flg)
		{
			m_IsDetectedByEnemyNum++;
		}
	}

	const ActionKeyConfig GetActionKeyConfig() { return m_keyConfig; }

	//インベントリ
	std::shared_ptr<PlayerInventory> GetPlayerInventory() { return m_spPlayerInventory; }

	//////////////////////////////////////////
	//バフ
	std::shared_ptr<PlayerBuffManager> GetPlayerBuffManager() { return m_spPlayerBuffManager; }

	//////////////////////////
	//エフェクトマネージャー
	void SetEffectManager(std::shared_ptr<EffectManager>_spEffectManager);


	//HPMax増加
	//flg...増加分体力を回復するか
	void AddMaxHP(int _Bosst, bool flg = false);

	void HPHeal(int _Heal);

	void SetDead()override;

	int GetLV() { return m_status.LV; }
	void SetLV(int _LV)override;

	//_gainedExp...経験値量
	void AddExp(int _gainedExp);


	//ボーンの位置(ワールド)を返す
	Math::Vector3 GetBonePosition(std::string _boneName);

	//回転を返す
	float GetAngle() { return m_angle; }


	void SetAccessoryManager(std::shared_ptr<AccessoryManager>_spAccessoryManager)override;

	std::weak_ptr<WeaponBase> GetWeapon() { return m_wpWepon; }

	// インベントリ表示時などのSE全停止処理
	void StopSound();


	//Effect
	void PlayAttackBoostEffect();
	void StopAttackBoostEffect();

	void SetPlayerWeaponInfo(int info);



	Math::Matrix GetBoneWorldMatrix(const std::string& _boneName);
protected:

	std::shared_ptr<PlayreLV>m_spPlayreLV;


	//エフェクトマネージャー
	std::weak_ptr<EffectManager> m_wpEffectManager;

	std::shared_ptr<DamageOverlay>m_spDamageOverlay;

	//インベントリ
	std::shared_ptr<PlayerInventory> m_spPlayerInventory;


	//敵に見つかってるか
	bool m_IsDetectedByEnemy = false;
	//今フレーム敵身見つかった数
	int m_IsDetectedByEnemyNum = 0;

	std::weak_ptr<GameScene> m_wpGameScene;

	///////////////////////////////////////
	//武器
	std::weak_ptr<WeaponBase> m_wpWepon;

	void WeaponUpdate();

	///////////////////////////////////////////
	//Keyコンフィグ

	//Keyの設定が入ってるファイル名
	std::string m_keyConfigFilePath;

	ActionKeyConfig m_keyConfig;

	//_filePath ... ファイルパスを書く
	void LoadKeyConfig(std::string _filePath);

	void SaveKeyConfig(std::string _filePath);
	///////////////////////////////////////////

	///////////////////////////////////////////
	//移動
	void Move();

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

	MoveMode m_moveMode;

	//m_status.moveSpeed.nowSpeedを求める
	void MoveNowSpeedDecision();
	///////////////////////////////////////////


	///////////////////////////////////////////
	//ジャンプ&重力処理
	void JumpAndGravity();

	//ジャンプ力(+の値)
	float m_jumpPower;

	//ジャンプフラグ
	//今飛べる状態にいるかどうか
	bool m_jumpFlg = true;

	///////////////////////////////////////////


	///////////////////////////////////////////


	///////////////////////////////////////////
	//アクション

	//次の階に
	std::shared_ptr<NextFloorAction>m_spNextFloorAction;

	//次の階アクションをしていいかどうか
	bool m_nextFloorActionFlg = false;


	////////////////////////////////////////////
	//アニメーションモード
	struct PlayerAnimeName
	{
		std::string IdleAnime = "Idle";
		std::string WalkAnime = "Walk";
		std::string RunAnime = "Run";
		std::string PickUpAnime = "PickUp";

		std::string AttackAnime = "";

		//ダガーの攻撃アニメーション
		std::string DaggerAttackAnime = "Dagger_Attack";

		//剣の攻撃アニメーション
		std::string SwordAttackAnime = "Sword_Swing";


		std::string PunchAttackAnime = "Punch";
		std::string EvasionAnime = "Roll";
		std::string DeathAnime = "Death";
	};

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

	PlayerAnimeName m_playerAnimeName;

	PlayerAnimeMode m_nowPlayerAnimeMode;
	PlayerAnimeMode m_oldPlayerAnimeMode;

	void PlayerAnimeModeUpdate();


	////////////////////////////////////////
	//UI
	std::weak_ptr<UIMap_Player>m_wpUIMap_Player;
	std::weak_ptr<SkillGage>m_wpSkillGage;



	///////////////////////////////////
	//回避
	bool m_evasionAnimeFlg = false;
	bool m_evasionFlg = false;
	float m_evasionCost = 10;
	float m_evasionSpeed = 10;

	const float m_kEvasionAccelEnd = 0.3f;
	const float m_kEvasionMaxSpeedEnd = 0.8f;

	void EvasionUpdate();

	////////////////////////////////////
	//通常攻撃
	bool m_normalAttack = false;
	bool m_NowNormalAttack = false;
	float m_normalAttackWaitMax = 1;
	float m_normalAttackWaitNow = 0;

	float m_chargeAttackCount = 0;
	bool m_chargeAttacFlg = false;

	/////////////////////////////////////
	//バフ
	std::shared_ptr<PlayerBuffManager>m_spPlayerBuffManager;

	////////////////////////////////////
	//アクセサリー
	std::shared_ptr<PlayerAccessory>m_spPlayerAccessory;

	//死亡アニメーションが終わったかどうか
	bool m_isDeadAnime = false;

	//ゲームシーンに死亡アニメーションが終わったかどうかを送ったかどうか
	bool m_isGameScenouto = false;


	std::string m_sunaNum = "Suna/Suna.efkefc";

	//音
	std::shared_ptr<KdSoundInstance> m_spDamageSE;
	std::shared_ptr<KdSoundInstance> m_spRollSE;
	std::shared_ptr<KdSoundInstance> m_spJumpSE;


	//Effect
	std::weak_ptr<KdEffekseerObject> m_wpHeelEffect;
	std::weak_ptr<KdEffekseerObject> m_wpAttackBoostEffect;
	bool m_isAttackBoostActive = false;

	std::weak_ptr<KdEffekseerObject> m_wpDashEffect;
	bool m_isDashActive = false;

	//ダッシュできる時はするかどうか
	bool m_IsDash = false;



	////////////////////////////////////

	std::vector<ComboStep> m_attackCombo;

	// コンボファイルのフォルダー（持ち主ごと）と、今使っているファイルのパス
	std::string m_comboFolder = "Asset/Data/ObjeData/Combo/Player/";
	std::string m_comboFilePath;
	int   m_comboStep = -1;        // -1 = コンボしていない
	bool  m_comboReserved = false;
	float m_stepForwardDone = 0.0f;   // この段で踏み込んだ距離（m）
	float m_animeSpeedRate = 1.0f;

	void StartComboStep(int _step);
	void ComboUpdate();
	void AimToNearestEnemy(const std::shared_ptr<WeaponBase>& _spWeapon);
	void CancelCombo();
};
