#pragma once
#include"../CharacterBase.h"

class CameraBase;
class GameScene;

//次の階シリーズ
class NextFloorAction;
class NextFloorGaugeUI;

class WeaponBase;

class PlayerBase :public CharacterBase
{
public:

	void Init()override;
	void PreUpdate()override;
	void Update()override;
	void PostUpdate()override;



	void SetGameScene(const std::shared_ptr<GameScene>& _GameScene);

	void SetNextFloorActionFlg(bool _flg) { m_nextFloorActionFlg = _flg; }
	void SetNextFloorGaugeUI(const std::shared_ptr<NextFloorGaugeUI>& _NextFloorGaugeUI);

	void SetWepon(const std::shared_ptr<WeaponBase>& _wepon) { m_wpWepon = _wepon; }




protected:

	std::weak_ptr<GameScene> m_wpGameScene;

	///////////////////////////////////////
	//武器
	std::weak_ptr<WeaponBase> m_wpWepon;

	void WeaponUpdate();

	///////////////////////////////////////////
	//Keyコンフィグ

	//Keyの設定が入ってるファイル名
	std::string m_keyConfigFilePath;

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

		//攻撃
		int attack = VK_LBUTTON;

		//インタラクト
		int interact = 'E';
	};

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
	};

	enum PlayerAnimeMode
	{
		IdleAnime,
		WalkAnime,
		RunAnime,
		PickUpAnime,
		SwordAttackAnime,
		PunchAttackAnime
	};

	PlayerAnimeName m_playerAnimeName;

	PlayerAnimeMode m_nowPlayerAnimeMode;
	PlayerAnimeMode m_oldPlayerAnimeMode;

	void PlayerAnimeModeUpdate();

};
