#pragma once
class PlayerBase;

class WarpGateBase :public KdGameObject
{
public:
	WarpGateBase() {};
	~WarpGateBase()override {};

	void Init()override;
	void Update()override;
	void PreDraw() override;
	void DrawLit()override;


	void SetPos(const Math::Vector3& pos)override
	{
		m_pos = pos;
		m_mWorld.Translation(m_pos);
	}

	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayer) { m_wpPlayer = _spPlayer; }

	
protected:

	//プレイヤーが乗ってるかどうか
	bool PlayerHit();

	//プレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayer;

	//3Dモデル
	std::shared_ptr<KdModelData>m_spWarpGate;

	//座標
	Math::Vector3 m_pos;

};
