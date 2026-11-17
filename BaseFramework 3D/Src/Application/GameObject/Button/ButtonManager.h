#pragma once

class ButtonBase;

class ButtonManager :public KdGameObject
{
public:

	//タイトル画面のボタンを作る
	void InitTitle();
	//リザルト画面のボタンを作る
	void InitResult();
	//更新
	virtual void Update()override;
	//ボタンを描く
	virtual void DrawSprite()override;

	//座標をセット
	// _pos ... 座標
	void SetPos(const Math::Vector3& _pos);

protected:

	//初期化
	void Init()override;

	//ボタンのリスト
	std::list<std::shared_ptr<ButtonBase>> m_buttonList;

	//ボタンを置く位置のリスト
	std::vector<Math::Vector3>m_posList;

	//ボタンの数
	int m_posListNum;

	//選ぶボタンを変えたときの音
	std::shared_ptr < KdSoundInstance> m_spButtonChange;
	//ボタンを決定したときの音
	std::shared_ptr < KdSoundInstance> m_spButtonDecision;
};
