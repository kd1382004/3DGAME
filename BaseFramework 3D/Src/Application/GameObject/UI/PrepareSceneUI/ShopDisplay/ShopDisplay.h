#pragma once
#include"../../UIBase.h"

class PrepareScene;

class ShopDisplay :public UIBase
{
public:
	ShopDisplay() {};
	~ShopDisplay() override{};

	void Init()override;
	void Update()override;
	void DrawSprite()override;

	// 準備シーンをセット
	void SetPrepareScene(std::shared_ptr<PrepareScene> _spPrepareScene) { m_wpPrepareScene = _spPrepareScene; }

	// 表示する(true)・しない(false)
	// PrepareScene::ChangeTabから呼ばれる
	void SetActive(bool _flg) { m_activeFlg = _flg; }
	bool GetActive() const { return m_activeFlg; }

private:

	// 準備シーン(循環参照を防ぐためweak_ptr)
	std::weak_ptr<PrepareScene> m_wpPrepareScene;

	// 表示中かどうか(開いているタブの時だけtrue)
	bool m_activeFlg = false;

	//マウスと当たってるかを返す
	bool MouseHit(Math::Vector2 _pos,float _w,float _h);

};
