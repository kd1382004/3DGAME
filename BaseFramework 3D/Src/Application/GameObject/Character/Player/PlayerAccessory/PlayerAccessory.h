#pragma once

class Flashlight;
class PlayerBase;

class PlayerAccessory
{
public:
	//コンストラクタ
	PlayerAccessory() {};
	//デストラクタ
	~PlayerAccessory() {};

	//初期化
	void Init();
	//懐中電灯をプレイヤーの腰に合わせて動かす
	// _player ... プレイヤー
	void Update(PlayerBase*_player);


	//懐中電灯をセット
	// _spFlashlight ... 懐中電灯
	void SetFlashlight(std::shared_ptr<Flashlight> _spFlashlight) { m_wpFlashlight = _spFlashlight; }

private:

	//プレイヤーに付ける懐中電灯
	std::weak_ptr<Flashlight>m_wpFlashlight;

};
