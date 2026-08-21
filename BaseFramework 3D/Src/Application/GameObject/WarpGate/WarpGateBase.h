#pragma once
class PlayerBase;
class GameScene;

class WarpGateBase :public KdGameObject
{
public:
	WarpGateBase() {};
	~WarpGateBase()override {};

	virtual void Init()override;
	virtual void Update()override;
	void PreDraw() override;
	void DrawLit()override;


	void SetPos(const Math::Vector3& pos)override
	{
		m_pos = pos;
		m_mWorld.Translation(m_pos);
	}

	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayer) { m_wpPlayer = _spPlayer; }
	void SetGameScene(std::shared_ptr<GameScene>_spGameScene) { m_wpGameScene = _spGameScene; }
	
	void SetIsExpired(bool _flg) { m_isExpired = _flg; };

protected:

	//プレイヤーが乗ってるかどうか
	bool PlayerHit();

	//プレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayer;
	std::weak_ptr<GameScene>m_wpGameScene;

	//3Dモデル
	std::shared_ptr<KdModelData>m_spWarpGate;

	//座標
	Math::Vector3 m_pos;

};
