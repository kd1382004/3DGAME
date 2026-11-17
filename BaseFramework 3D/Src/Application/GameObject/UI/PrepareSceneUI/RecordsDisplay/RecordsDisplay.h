#pragma once
#include"../../UIBase.h"
#include"../../../../Scene/SceneManager.h"

class PrepareScene;

class RecordsDisplay :public UIBase
{
public:
	RecordsDisplay() {};
	~RecordsDisplay() override{};

	void Init()override;
	void Update()override;
	void DrawSprite()override;

	// 準備シーンをセット
	void SetPrepareScene(std::shared_ptr<PrepareScene> _spPrepareScene) { m_wpPrepareScene = _spPrepareScene; }

	// プレイ記録をセット(PrepareScene::Initから呼ばれる)
	void SetGameRecord(const GameRecord& _gameRecord) { m_gameRecord = _gameRecord; }

	// 表示する(true)・しない(false)
	// PrepareScene::ChangeTabから呼ばれる
	void SetActive(bool _flg) { m_activeFlg = _flg; }
	bool GetActive() const { return m_activeFlg; }

private:

	// 準備シーン(循環参照を防ぐためweak_ptr)
	std::weak_ptr<PrepareScene> m_wpPrepareScene;

	// プレイ記録(最新1件 + 最高階層ベスト3)
	GameRecord m_gameRecord;

	// 表示中かどうか(開いているタブの時だけtrue)
	bool m_activeFlg = false;

	//マウスと当たってるかを返す
	bool MouseHit(Math::Vector2 _pos,float _w,float _h);

};
