#pragma once
#include"../CharacterBase.h"

class CameraBase;

class PlayerBase:public CharacterBase
{
public:

	void Init()override;
	void Update()override;

	void SetCamera(const std::shared_ptr<CameraBase>& _camera) { m_wpCamera = _camera; }
protected:


	///////////////////////////////////////////
	//Keyコンフィグ

	//Keyの設定が入ってるファイル名
	std::string m_keyConfigFilePath;

	struct ActionKeyConfig
	{
		// Z+に移動
		int moveForward;   

		// Z-に移動
		int moveBackward;  

		// X+に移動
		int moveRight;    

		// X-に移動
		int moveLeft;      

		//ジャンプ
		int jump;
	};

	ActionKeyConfig m_keyConfig;

	//_filePath ... ファイルパスを書く
	void LoadKeyConfig(std::string _filePath);

	///////////////////////////////////////////

	///////////////////////////////////////////
	//キャラステータス

	//キャラステータスが入ってるファイル名
	std::string m_charaStatusFilePath;

	struct StatusHP
	{
		//最大体力
		float maxHP;

		//現在体力
		float nowHP;
	};
	
	struct StatusAttck
	{
		//基礎攻撃力
		float baseAttckPowe;

		//現在攻撃力
		float nowAttck;
	};

	struct StatusDefense
	{
		//基礎防御力
		float baseDefensePowe;

		//現在防御力
		float nowDefense;
	};

	struct StatusMoveSpeed
	{
		//基礎速度
		float baseSpeed;

		//歩き速度(基礎速度に足す値)
		float walkMovePowe;
		
		//ダッシュ速度(基礎速度に足す値)
		float runMovePowe;

		//現在の速度
		float nowSpeed;
	};

	struct Status
	{
		//体力
		StatusHP HP;

		// 攻撃力
		StatusAttck attck;

		//防御力
		StatusDefense defense;

		//スピード
		StatusMoveSpeed moveSpeed;
	};

	Status m_status;

	//_filePath ... ファイルパスを書く
	void LoadCharaStatus(std::string _filePath);

	///////////////////////////////////////////



	std::weak_ptr<CameraBase>m_wpCamera;



};
