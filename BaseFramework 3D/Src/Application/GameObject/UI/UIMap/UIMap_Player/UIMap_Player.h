#pragma once

class UIMap_Player
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	UIMap_Player() {};

	//デストラクタ
	~UIMap_Player() {};

	//---- 基本の処理 ----

	//初期化
	void Init();

	//描画前の処理
	void PreDraw();

	//2Dの描画
	void DrawSprite();

	//---- 取得 ----

	//ミニマップ上のプレイヤーの位置を返す
	//戻り値 ... 画面座標
	Math::Vector2 GetPos();

	//---- 設定 ----

	//プレイヤーの座標をセット
	// _3DPos ... プレイヤーの座標(ワールド)
	void SetPlayer3DPos(Math::Vector3 _3DPos)
	{
		m_player3DPos = _3DPos;
	}

	//マップの基準位置(ワールド)をセット
	// _3DPos ... 基準位置(ワールド)
	void SetBase3DPos(Math::Vector3 _3DPos)
	{
		m_base3DPos = _3DPos;
	}

	//ミニマップの基準位置(画面)をセット
	// _basePos ... 基準位置(画面)
	void SetBasePos(Math::Vector2 _basePos) { m_basePos= _basePos; }

	//1タイルの大きさをセット
	// _siz ... 1タイルの大きさ
	void SetTileSiz(float _siz) { m_tileSiz = _siz; }

	//向きをセット
	// _angle ... 角度(度)
	void SetAngle(float _angle) { m_angle = _angle; }

	//ミニマップの1マスの画像の大きさをセット
	// _siz ... 画像の大きさ
	void SetMapTexSiz(Math::Vector2 _siz) { m_mapTexSiz = _siz; }

private:

	//---- メンバ変数 ----

	//ミニマップの1マスの画像の大きさ
	Math::Vector2 m_mapTexSiz = { };

	//大きさの倍率
	float m_siz = 1.0f;

	//向き
	float m_angle = 0.0f;

	//1タイルの大きさ
	float m_tileSiz = 10.0f;

	//ミニマップの基準位置(画面)
	Math::Vector2 m_basePos;

	//プレイヤーのアイコン
	std::shared_ptr<KdTexture>m_spPlayerTex;

	//プレイヤーの座標(ワールド)
	Math::Vector3 m_player3DPos;

	//プレイヤーの座標(画面)
	Math::Vector2 m_player2DPos;

	//マップの基準位置(ワールド)
	Math::Vector3 m_base3DPos;

	//---- その他の処理 ----

	//プレイヤーの座標を画面座標にする
	void Change2DPos();
};
