#pragma once

class PlayerBase;


struct StatusBuff
{
	int m_ID = -999;

	//効果時間(秒)(現在時間)
	float m_nowSeconds = 0;

	//効果時間(秒)(最大時間)
	float m_maxSeconds = 0;

	//上げる攻撃力の量(数値)
	int m_buffAttck = 0;

	//上げる防御力の量(数値)
	int m_buffDefense = 0;

	//上げるHPの量(数値)
	int m_buffHP = 0;
};

//Statusのアップ
class PlayerBuffManager
{
public:
	PlayerBuffManager() {};
	~PlayerBuffManager() {};

	void Update();


	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayer) { m_wpPlayer = _spPlayer; }

	void BuffFinish(const StatusBuff& _statusBuff);

	void AddStatusBuff(StatusBuff _statusBuff);

	std::list<StatusBuff> GetStatusBuff() { return m_statusBuffList; }
private:

	std::weak_ptr<PlayerBase>m_wpPlayer;

	std::list<StatusBuff> m_statusBuffList;

};
