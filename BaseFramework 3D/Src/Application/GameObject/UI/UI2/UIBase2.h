#pragma once
#include"../UIBase.h"

//キーの画像をどちら側に出すか
enum DrawKeyPos
{
	DrawKeyPos_Left,
	DrawKeyPos_Right,
	DrawKeyPos_Top,
	DrawKeyPos_Bot,
};

class UIBase2 :public UIBase
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	UIBase2() {};

	//デストラクタ
	virtual ~UIBase2() {};

	//---- 基本の処理 ----

	//更新(キーの点滅)
	virtual void Update()override;

	//2Dの描画
	virtual void DrawSprite()override;

	//---- 取得 ----

	//描画優先度を返す
	//戻り値 ... 描画優先度(小さいほど優先)
	int GetLayerPriority() { return m_layerPriority; };

	//---- 設定 ----

	//表示するキーをセット
	// _key ... キーの番号
	void SetDrawKey(int _key) { m_drawKey = _key; }

	//一緒に出すテクスチャをセット
	// _spTex ... テクスチャ
	void SetTex(std::shared_ptr<KdTexture>_spTex) { m_spTex = _spTex; }

	//表示するかをセット
	// _flg ... 表示するか
	void SetDrawFlg(bool _flg) { m_drawFlg = _flg; }

	//キーをテクスチャのどこに出すかをセット
	// _drawKeyPos ... キーの位置
	void SetDrawKeyPos(DrawKeyPos _drawKeyPos) {m_drawKeyPos = _drawKeyPos;}

	//---- その他の処理 ----

	//画面上の座標をセット
	// _pos ... 画面上の座標
	virtual void Set2DPos(const Math::Vector2& _pos) { m_2DPos = _pos; };

protected:

	//---- メンバ変数 ----

	//表示するか
	bool m_drawFlg = false;

	//表示するキーの番号
	int m_drawKey = 0;

	//一緒に出すテクスチャ
	std::shared_ptr<KdTexture>m_spTex;

	//画面上の座標
	Math::Vector2 m_2DPos;

	//キーをテクスチャのどこに出すか
	DrawKeyPos m_drawKeyPos = DrawKeyPos_Left;

	//描画優先度
	//数字が小さいほうが優先度高い
	int m_layerPriority = INT_MAX;

	//アルファ
	float m_alpha = 1;

	//1秒あたりのアルファの増減(点滅の速さ)
	float m_addAlpha = 0.5;
};