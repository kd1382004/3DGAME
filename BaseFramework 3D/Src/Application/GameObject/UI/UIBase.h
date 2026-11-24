#pragma once

//UIの基底クラス
class UIBase :public KdGameObject
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	UIBase() {};

	//デストラクタ
	virtual ~UIBase() {};

	//---- 基本の処理 ----

	//初期化
	virtual void Init()override;

	//更新前の処理
	virtual void PreUpdate()override;

	//更新
	virtual void Update()override;

	//描画前の処理
	virtual void PreDraw()override;

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

protected:

	//---- メンバ変数 ----

	//画面上の座標
	Math::Vector2 m_2DPos;

	//描画優先度
	//数字が小さいほうが優先度高い
	int m_layerPriority = INT_MAX;
};