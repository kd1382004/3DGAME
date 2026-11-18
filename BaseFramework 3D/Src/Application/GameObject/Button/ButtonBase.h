#pragma once


class ButtonBase :public KdGameObject
{
public:

	//---- 基本の処理 ----

	//初期化
	virtual void Init()override;

	//更新
	virtual void Update()override;

	//ボタンを描く
	void DrawSprite()override;

	//---- 取得 ----

	//表示しているか
	//戻り値 ... 表示しているならtrue
	bool GetActive() const { return m_activeFlg; }

	//---- 設定 ----

	//座標をセット
	// _pos ... 座標(x,yだけ使う)
	void SetPos(const Math::Vector3& _pos)override;

	// 表示する(true)・しない(false)。falseの間は押せない・描かない
	// _flg ... 表示するならtrue
	void SetActive(bool _flg) { m_activeFlg = _flg; }

	//---- その他の処理 ----

	//キー操作で選ばれているかをセット(選ばれていると大きくなる)
	// _choseFlg ... 選ばれているならtrue
	void KeyChosen(bool _choseFlg);

protected:

	//---- メンバ変数 ----

	//表示しているか(falseの間は押せない・描かない)
	bool m_activeFlg = true;

	//ボタンの画像
	std::shared_ptr<KdTexture> m_spTex;

	//ボタンの座標(画面の真ん中が0,0)
	Math::Vector2 m_pos = {};

	//ボタンの大きさ(画像の半分×倍率)
	Math::Vector2 m_diameter = {};

	//今の大きさの倍率
	float m_scale = 1.0f;

	//通常の大きさの倍率
	float m_defScale = 2.0;

	//選ばれたときに大きくする倍率
	float m_hitScale = 1.5;

	//選ばれているか(マウスが乗っているか)
	bool m_choseFlg;

	//不透明度
	float m_alpha = 1;

	//点滅するときの一番薄い不透明度
	float m_alphaMin = 0.3;

	//点滅するときの一番濃い不透明度
	float m_alphaMax = 1;

	//点滅するときに1フレームで変える量
	float m_alphaAdd = 0.01;

	//エディター用
	bool m_editorChoseFlg = false;

	//---- 基本の処理 ----

	//位置・大きさを変えるImGui
	void ImGUI()override;

	//---- その他の処理 ----

	//不透明度を変えて点滅させる
	void AlphaFlashing();

	//Mouseと当たっているかどうか
	void MouseChosen();

	//ファイルから位置・大きさを読み込む
	void DataLodo()override;
};
