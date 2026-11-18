#pragma once

class PlayerBase;


//かかっているバフ1つ分の情報
struct StatusBuff
{
	//バフの種類の番号(-999は未設定)
	int m_ID = -999;

	//効果時間(秒)(現在時間)
	float m_nowSeconds = 0;

	//効果時間(秒)(最大時間)
	float m_maxSeconds = 0;

	//上げる攻撃力の量(数値)
	int m_buffAttack = 0;

	//上げる防御力の量(数値)
	int m_buffDefense = 0;

	//上げるHPの量(数値)
	int m_buffHP = 0;

	//上げるスタミナ回復量の量(数値)
	float m_buffStaminaRegen = 0.0f;
};

//Statusのアップ
class PlayerBuffManager
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	PlayerBuffManager() {};

	//デストラクタ
	~PlayerBuffManager() {};

	//---- 基本の処理 ----

	//バフの残り時間を減らし、時間切れのバフを終わらせる
	void Update();

	//---- 取得 ----

	//今かかっているバフの一覧を返す
	//戻り値 ... バフの一覧(コピー)
	std::list<StatusBuff> GetStatusBuff() { return m_statusBuffList; }

	//---- 設定 ----

	//バフをかけるプレイヤーをセット
	// _spPlayer ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayer) { m_wpPlayer = _spPlayer; }

	//バフを追加してプレイヤーに効果をかける
	// _statusBuff ... 追加するバフ
	void AddStatusBuff(StatusBuff _statusBuff);

	//---- その他の処理 ----

	//バフの効果を終わらせる(上げた分を元に戻す)
	// _statusBuff ... 終わらせるバフ
	void BuffFinish(const StatusBuff& _statusBuff);

	//全部のバフを終わらせて消す
	void ClearAllBuffs();

private:

	//---- メンバ変数 ----

	//バフをかけるプレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayer;

	//今かかっているバフの一覧
	std::list<StatusBuff> m_statusBuffList;
};
