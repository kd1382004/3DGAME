#pragma once

class PlayerBase;

class PlayreLV
{
public:
	PlayreLV() {};
	~PlayreLV() {};

	//引数
	//....手に入れた経験値料
	//...プレイヤーのポインター
	void AddExp(int _gainedExp, PlayerBase* player);
private:

	//次のレベルまでの経験値
	int m_nextLVExperiencePoints = 10;

	//次のレベルまでの経験値を計算する
	void CalcNextLevelExp(int _LV);


	//現在経験値
	int n_nowExperiencePoints =0;
};
