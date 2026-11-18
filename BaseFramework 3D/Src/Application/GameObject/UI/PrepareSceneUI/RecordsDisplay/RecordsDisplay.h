#pragma once
#include"../../UIBase.h"
#include"../../../../Scene/SceneManager.h"

class PrepareScene;

class RecordsDisplay :public UIBase
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	RecordsDisplay() {};

	//デストラクタ
	~RecordsDisplay() override{};

	//---- 基本の処理 ----

	//初期化
	void Init()override;

	//更新
	void Update()override;

	//2Dの描画
	void DrawSprite()override;

	//---- 取得 ----

	//表示中かを返す
	//戻り値 ... 表示中ならtrue
	bool GetActive() const { return m_activeFlg; }

	//---- 設定 ----

	// 準備シーンをセット
	// _spPrepareScene ... 準備シーン
	void SetPrepareScene(std::shared_ptr<PrepareScene> _spPrepareScene) { m_wpPrepareScene = _spPrepareScene; }

	// プレイ記録をセット(PrepareScene::Initから呼ばれる)
	// _gameRecord ... プレイ記録
	void SetGameRecord(const GameRecord& _gameRecord) { m_gameRecord = _gameRecord; }

	// 表示する(true)・しない(false)
	// PrepareScene::ChangeTabから呼ばれる
	// _flg ... 表示するか
	void SetActive(bool _flg) { m_activeFlg = _flg; }

private:

	//---- メンバ変数 ----

	// 準備シーン(循環参照を防ぐためweak_ptr)
	std::weak_ptr<PrepareScene> m_wpPrepareScene;

	// プレイ記録(最新1件 + 最高階層ベスト3)
	GameRecord m_gameRecord;

	// 表示中かどうか(開いているタブの時だけtrue)
	bool m_activeFlg = false;

	//---- その他の処理 ----

	//マウスと当たってるかを返す
	// _pos ... 四角の真ん中
	// _w ... 四角の横幅
	// _h ... 四角の高さ
	bool MouseHit(Math::Vector2 _pos,float _w,float _h);
};
