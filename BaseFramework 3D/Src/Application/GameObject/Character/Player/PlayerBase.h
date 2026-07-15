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

	



	std::weak_ptr<CameraBase>m_wpCamera;



};
