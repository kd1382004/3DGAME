#pragma once
#include"../CharacterBase.h"

class CameraBase;

class PlayerBase :public CharacterBase
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
		int moveForward = 'W';

		// Z-に移動
		int moveBackward = 'S';

		// X+に移動
		int moveRight = 'D';

		// X-に移動
		int moveLeft = 'A';

		//ジャンプ
		int jump = VK_SPACE;
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
	bool m_jumpFlg=true;

	///////////////////////////////////////////


	///////////////////////////////////////////
	//カメラ
	std::weak_ptr<CameraBase>m_wpCamera;



};
