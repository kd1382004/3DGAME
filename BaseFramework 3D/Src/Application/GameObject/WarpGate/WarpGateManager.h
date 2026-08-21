#pragma once

class WarpGateBase;
class PlayerBase;
class GameScene;

class WarpGateManager:public KdGameObject
{
public:
	WarpGateManager() {};
	~WarpGateManager()override {};

	void Init()override;
	void PreUpdate()override;
	void Update()override;
	void PreDraw() override;
	void DrawLit()override;

	void WarpGateInit(Math::Vector3 _setPos);

	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayer) { m_wpPlayer = _spPlayer; }
	void SetGameScene(std::shared_ptr<GameScene>_spGameScene) { m_wpGameScene = _spGameScene; }

	void WarpGateReset() { m_warpGateList.clear(); }
private:
	//プレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayer;
	std::weak_ptr<GameScene>m_wpGameScene;

	std::list<std::shared_ptr<WarpGateBase>>m_warpGateList;


};
