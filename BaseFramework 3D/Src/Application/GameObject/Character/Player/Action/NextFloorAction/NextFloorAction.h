#pragma once

class GameScene;
class NextFloorGaugeUI;

class NextFloorAction
{
public:
	NextFloorAction() {};
	~NextFloorAction() {};

	void Init();
	void Update(bool _nextFloorActionFlg);

	void SetActionKey(int _actionKey) { m_actionKey = _actionKey; }
	void SetGameScene(const std::shared_ptr<GameScene>& _GameScene) { m_wpGameScene = _GameScene; }
	void SetNextFloorGaugeUI(const std::shared_ptr<NextFloorGaugeUI>& _NextFloorGaugeUI);


protected:
	std::weak_ptr<GameScene> m_wpGameScene;
	std::weak_ptr<NextFloorGaugeUI> m_wpNextFloorGaugeUI;

	//次の階に行くアクションキー
	int m_actionKey;

	//長押しカウント
	int m_nextFloorCount;

	static const int m_nextFloorCountMax = 60;
};
