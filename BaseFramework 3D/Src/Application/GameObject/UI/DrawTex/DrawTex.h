#pragma once
#include"../UIBase.h"

class DrawTex :public UIBase
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	DrawTex() {};

	//デストラクタ
	virtual ~DrawTex() {};

	//---- 基本の処理 ----

	//更新
	virtual void Update()override;

	//2Dの描画
	virtual void DrawSprite()override;

	//---- 取得 ----

	//描画優先度を返す
	//戻り値 ... 描画優先度(小さいほど優先)
	int GetLayerPriority() { return m_layerPriority; };

	//---- その他の処理 ----

	//画面上の座標をセット
	// _pos ... 画面上の座標
	virtual void Set2DPos(const Math::Vector2& _pos) { m_2DPos = _pos; };

	//テクスチャを読み込む
	// _path ... 画像のパス
	void LoadTex(std::string _path);

protected:

	//---- メンバ変数 ----

	//表示するテクスチャ
	std::shared_ptr<KdTexture>m_spTex;
};