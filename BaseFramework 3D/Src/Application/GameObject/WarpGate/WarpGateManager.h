#pragma once

class WarpGateBase;
class PlayerBase;

class WarpGateManager:public KdGameObject
{
public:
	WarpGateManager() {};
	~WarpGateManager()override {};

	void Init()override;
	void Update()override;
	void PreDraw() override;
	void DrawLit()override;

	void WarpGateInit(Math::Vector3 _setPos);

	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayer) { m_wpPlayer = _spPlayer; }
private:
	//プレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayer;

	std::list<std::shared_ptr<WarpGateBase>>m_warpGateList;


};
