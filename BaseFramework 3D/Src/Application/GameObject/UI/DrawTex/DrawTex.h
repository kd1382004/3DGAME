#pragma once
#include"../UIBase.h"

class DrawTex :public UIBase
{
public:
	//コンストラクタ
	DrawTex() {};
	//デストラクタ
	virtual ~DrawTex() {};

	//更新
	virtual void Update()override;

	//2Dの描画
	virtual void DrawSprite()override;

	//画面上の座標をセット
	// _pos ... 画面上の座標
	virtual void Set2DPos(const Math::Vector2& _pos) { m_2DPos = _pos; };

	//描画優先度を返す
	//戻り値 ... 描画優先度(小さいほど優先)
	int GetLayerPriority() { return m_layerPriority; };

	//テクスチャを読み込む
	// _path ... 画像のパス
	void LoadTex(std::string _path);

protected:

	//表示するテクスチャ
	std::shared_ptr<KdTexture>m_spTex;

};