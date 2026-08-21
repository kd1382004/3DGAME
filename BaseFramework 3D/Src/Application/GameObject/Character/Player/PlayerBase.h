#pragma once
#include"../CharacterBase.h"



class CameraBase;
class GameScene;

//次の階シリーズ
class NextFloorAction;
class NextFloorGaugeUI;

//インベントリ
class PlayerInventory;

//バフ
class PlayerBuffManager;

//UI
class UIManager;
class UIMap_Player;
class StaminaGage;
class SkillGage;

//武器
class WeaponBase;


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


class PlayerBase :public CharacterBase
{
public:

	PlayerBase() {};
	~PlayerBase() {};

	void Init()override;
	void PreUpdate()override;
	void Update()override;
	void PostUpdate()override;



	void SetGameScene(const std::shared_ptr<GameScene>& _GameScene);

	void SetNextFloorActionFlg(bool _flg) { m_nextFloorActionFlg = _flg; }
	void SetNextFloorGaugeUI(const std::shared_ptr<NextFloorGaugeUI>& _NextFloorGaugeUI);

	void SetWepon(const std::shared_ptr<WeaponBase>& _wepon) { m_wpWepon = _wepon; }

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



	/////////////////////////////////////
	//HP

	void HPHeal(int _Heal);
	
	//MaxのHPを返す
	int GetMaxHP() { return m_status.HP.maxHP; }

	//HPMax増加
	//flg...増加分体力を回復するか
	void AddMaxHP(int _Bosst,bool flg=false);

	/////////////////////////////////////
	//攻撃力

	//基礎攻撃力アップ
	void BoostAttackBase(int _Bosst) { m_status.attck.baseAttckPowe += _Bosst; }

	//基礎攻撃力を返す
	int GetAttackBase() { return m_status.attck.baseAttckPowe; }

	//増加攻撃力アップ
	void BoostAddAttack(int _Bosst) { m_status.attck.addAttack += _Bosst; }

	/////////////////////////////////////
	//防御力

	//基礎防御力アップ
	void BoostDefenseBase(int _Bosst) { m_status.defense.baseDefensePowe += _Bosst; }

	//基礎防御力を返す
	int GetDefenseBase() { return m_status.defense.baseDefensePowe; }

	//増加防御力アップ
	void BoostAddDefense(int _Bosst) { m_status.defense.addDefense += _Bosst; }

	////////////////////////////////////////
	//インベントリ
	std::shared_ptr<PlayerInventory> GetPlayerInventory() { return m_spPlayerInventory; }

	//////////////////////////////////////////
	//バフ
	std::shared_ptr<PlayerBuffManager> GetPlayerBuffManager() { return m_spPlayerBuffManager; }

protected:
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
		std::string SwordAttackAnime = "Dagger_Attack";
		std::string PunchAttackAnime = "Punch";
		std::string EvasionAnime = "Roll";
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
	};

	PlayerAnimeName m_playerAnimeName;

	PlayerAnimeMode m_nowPlayerAnimeMode;
	PlayerAnimeMode m_oldPlayerAnimeMode;

	void PlayerAnimeModeUpdate();


	////////////////////////////////////////
	//UI
	std::weak_ptr<UIMap_Player>m_wpUIMap_Player;
	std::weak_ptr<StaminaGage>m_wpStaminaGage;
	std::weak_ptr<SkillGage>m_wpSkillGage;

	////////////////////////////////////////
	//スタミナ
	float m_staminaMax = 100;
	float m_staminaNow = m_staminaMax;

	//dashでの減少量(1秒あたり)
	float m_dashStaminaDrainPerSec = 10;

	//自然回復速度(1秒あたり)
	float m_staminaRegenPerSec = 1;

	//スタミナ減少
	// _amount...実際に減らす量
	//戻り値 ... _amount分減少できたかどうか
	bool ConsumeStamina(float _amount);

	void StaminaManager();

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
	float m_normalAttackWaitMax = 1;
	float m_normalAttackWaitNow = 0;

	/////////////////////////////////////
	//バフ
	std::shared_ptr<PlayerBuffManager>m_spPlayerBuffManager;
};
