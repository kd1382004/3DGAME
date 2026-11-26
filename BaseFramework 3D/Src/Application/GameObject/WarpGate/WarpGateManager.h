#pragma once

class WarpGateBase;
class PlayerBase;
class GameScene;

//ワープゲートの管理
class WarpGateManager:public KdGameObject
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	WarpGateManager() {};

	//デストラクタ
	~WarpGateManager()override {};

	//---- 基本の処理 ----

	//初期化
	void Init()override;

	//更新前の処理
	void PreUpdate()override;

	//更新
	void Update()override;

	//描画前の処理
	void PreDraw() override;

	//ライトありの描画
	void DrawLit()override;

	//全ワープゲートの音量を今の設定に合わせる(ポーズ中も呼ばれる)
	void ApplySoundVolume();

	//---- 設定 ----

	//プレイヤーをセット
	// _spPlayer ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayer) { m_wpPlayer = _spPlayer; }

	//ゲームシーンをセット
	// _spGameScene ... ゲームシーン
	void SetGameScene(std::shared_ptr<GameScene>_spGameScene) { m_wpGameScene = _spGameScene; }

	//---- その他の処理 ----

	//ワープゲート(次の階・脱出)を置く
	// _setNextGatePos ... 次の階へのゲートの位置
	// _setEscapeGatePos ... 脱出のゲートの位置
	void WarpGateInit(Math::Vector3 _setNextGatePos, Math::Vector3 _setEscapeGatePos);

	//ワープゲートを全部消す
	void WarpGateReset();

private:

	//---- メンバ変数 ----

	//プレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayer;

	//ゲームシーン
	std::weak_ptr<GameScene>m_wpGameScene;

	//ワープゲートのリスト
	std::list<std::shared_ptr<WarpGateBase>>m_warpGateList;
};
