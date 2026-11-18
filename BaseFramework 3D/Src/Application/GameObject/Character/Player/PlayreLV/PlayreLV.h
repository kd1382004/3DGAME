#pragma once

class PlayerBase;

class PlayreLV
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	PlayreLV() {};

	//デストラクタ
	~PlayreLV() {};

	//---- 設定 ----

	//経験値を増やして、たまったらレベルを上げる
	// _gainedExp ... 手に入れた経験値量
	// _player ... プレイヤーのポインター
	void AddExp(int _gainedExp, PlayerBase* _player);

	//---- その他の処理 ----

	//次のレベルまでの経験値を計算する
	// _LV ... 今のレベル
	void CalcNextLevelExp(int _LV);

private:

	//---- メンバ変数 ----

	//次のレベルまでの経験値
	int m_nextLVExperiencePoints = 10;

	//現在経験値
	int m_nowExperiencePoints =0;
};
