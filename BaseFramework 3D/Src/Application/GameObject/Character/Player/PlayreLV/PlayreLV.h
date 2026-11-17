#pragma once

class PlayerBase;

class PlayreLV
{
public:
	//コンストラクタ
	PlayreLV() {};
	//デストラクタ
	~PlayreLV() {};

	//経験値を増やして、たまったらレベルを上げる
	// _gainedExp ... 手に入れた経験値量
	// _player ... プレイヤーのポインター
	void AddExp(int _gainedExp, PlayerBase* _player);


	//次のレベルまでの経験値を計算する
	// _LV ... 今のレベル
	void CalcNextLevelExp(int _LV);
private:

	//次のレベルまでの経験値
	int m_nextLVExperiencePoints = 10;



	//現在経験値
	int m_nowExperiencePoints =0;
};
