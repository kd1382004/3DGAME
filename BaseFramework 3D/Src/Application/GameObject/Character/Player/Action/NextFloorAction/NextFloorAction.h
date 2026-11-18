#pragma once

class GameScene;
class NextFloorGaugeUI;

class NextFloorAction
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	NextFloorAction() {};

	//デストラクタ
	~NextFloorAction() {};

	//---- 基本の処理 ----

	//初期化
	void Init();

	//長押しで次の階に行く処理の更新
	// _nextFloorActionFlg ... 次の階に行ける場所にいるか
	void Update(bool _nextFloorActionFlg);

	//---- 設定 ----

	//次の階に行くアクションキーをセット
	// _actionKey ... キーの番号
	void SetActionKey(int _actionKey) { m_actionKey = _actionKey; }

	//ゲームシーンをセット
	// _spGameScene ... ゲームシーン
	void SetGameScene(const std::shared_ptr<GameScene>& _spGameScene) { m_wpGameScene = _spGameScene; }

	//長押しゲージのUIをセット
	// _spNextFloorGaugeUI ... 長押しゲージのUI
	void SetNextFloorGaugeUI(const std::shared_ptr<NextFloorGaugeUI>& _spNextFloorGaugeUI);

protected:

	//---- 定数 ----

	//次の階に行くまでの長押しフレーム数
	static const int kNextFloorCountMax = 60;

	//---- メンバ変数 ----

	//ゲームシーン
	std::weak_ptr<GameScene> m_wpGameScene;

	//長押しゲージのUI
	std::weak_ptr<NextFloorGaugeUI> m_wpNextFloorGaugeUI;

	//次の階に行くアクションキー
	int m_actionKey;

	//長押しカウント
	int m_nextFloorCount;
};
