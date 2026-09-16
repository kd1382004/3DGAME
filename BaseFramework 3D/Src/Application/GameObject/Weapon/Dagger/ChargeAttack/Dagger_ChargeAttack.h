#pragma once

class  CharacterBase;


struct StatusShockwave
{
	// 何体までヒットしていいか
	int maxHitNum = 0;

	// 最大飛距離（メートル）
	float maxDistanceM = 0;

	//進んだ距離
	float DistanceM = 0;

	// 飛ぶ速度（m/s）
	float speed = 0;

	// 最大ダメージ
	float maxDamage = 0;

	// 現在の飛行距離
	float nowDistance = 0.0f;

};

class  Dagger_ChargeAttack :public KdGameObject
{
public:
	Dagger_ChargeAttack() {};
	~Dagger_ChargeAttack()override {};

	void Init()override;
	void Update()override;


	void DrawEffect()override;


	//Statusセット
	//_hitNum...何体まで当たっていいか
	//_maxDistanceM ...　どれだけの距離(メートル)飛んでいいか
	//_speed ... 速度
	// _maxDamage ..最大ダメージ
	void SetShockwaveStatus(int _hitNum, float _maxDistanceM, float _speed, float _maxDamage, Math::Vector3 _pos, float _Angle)
	{
		m_shockwaveStatus.maxHitNum = _hitNum;
		m_shockwaveStatus.maxDistanceM = _maxDistanceM;
		m_shockwaveStatus.speed = _speed;
		m_shockwaveStatus.maxDamage = _maxDamage;
		m_pos = _pos;
		m_angle = _Angle;
	}
private:

	//あたり判定をするリスト
	std::list<std::weak_ptr<CharacterBase>>m_attackHitCharacterList;

	//既に当たったキャラリスト
	std::list<std::weak_ptr<CharacterBase>>m_hitCharactersList;

	//当り判定をする対象ががすでに当たってるかを返す
	bool IsAlreadyHit(const std::shared_ptr<CharacterBase>& _chara);

	StatusShockwave m_shockwaveStatus;


	//ポリゴン
	std::shared_ptr<KdSquarePolygon> m_Poly = nullptr;


	//座標
	Math::Vector3 m_pos = Math::Vector3::Zero;

	//進む角度
	float m_angle = 0;
};
