#pragma once

class UIMap_Map;

//ミニマップの敵アイコン
class UIMap_EnemyManage
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	UIMap_EnemyManage() {};

	//デストラクタ
	~UIMap_EnemyManage() {};

	//---- 基本の処理 ----

	//初期化
	void Init();

	//2Dの描画
	void DrawSprite();

	//---- 設定 ----

	//敵の座標をセット
	// _3DPos ... 敵の座標(ワールド)
	void SetEnemy3DPos(Math::Vector3 _3DPos)
	{
		m_enemy3DPos = _3DPos;
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
	// _size ... 1タイルの大きさ
	void SetTileSize(float _size) { m_tileSize = _size; }

	//向きをセット
	// _angle ... 角度(度)
	void SetAngle(float _angle) { m_angle = _angle; }

	//敵の位置と向きを一覧に追加
	// _pos ... 敵の座標(ワールド)
	// _angle ... 角度(度)
	void SetEnemyPosAngleList(Math::Vector3 _pos,float _angle);

	//マップのUIをセット
	// _spUIMap_Map ... マップのUI
	void SetUIMap_Map(std::shared_ptr<UIMap_Map>_spUIMap_Map) { m_wpUIMap_Map = _spUIMap_Map; }

	//ミニマップの1マスの画像の大きさをセット
	// _size ... 画像の大きさ
	void SetMapTexSize(Math::Vector2 _size) { m_mapTexSize = _size; }

	//---- その他の処理 ----

	//敵の位置の一覧を空にする
	void PosListReset();

private:

	//---- 型 ----

	//ミニマップに出す敵1体分の情報
	struct List
	{
		//敵の座標(ワールド)
		Math::Vector3 m_pos;
		//向き
		float m_angle = 0.0f;
		//表示するか
		bool m_drawFlg = false;
	};

	//---- メンバ変数 ----

	//ミニマップの1マスの画像の大きさ
	Math::Vector2 m_mapTexSize = { };

	//向き
	float m_angle = 0.0f;

	//1タイルの大きさ
	float m_tileSize = 10.0f;

	//ミニマップの基準位置(画面)
	Math::Vector2 m_basePos;

	//敵のアイコン
	std::shared_ptr<KdTexture>m_spEnemyTex;

	//敵の座標(ワールド)
	Math::Vector3 m_enemy3DPos;

	//敵の座標(画面)
	Math::Vector2 m_enemy2DPos;

	//マップの基準位置(ワールド)
	Math::Vector3 m_base3DPos;

	//敵の位置の一覧
	std::list<List> m_posList;

	//マップのUI
	std::weak_ptr<UIMap_Map>m_wpUIMap_Map;

	//大きさの倍率
	float m_size = 1.0f;

	//---- その他の処理 ----

	//ワールド座標をミニマップの画面座標にする
	// _pos ... ワールド座標
	//戻り値 ... 画面座標
	Math::Vector2 Change2DPos(Math::Vector3 _pos);

	//プレイヤーの近くなら表示するようにする
	// _info ... 調べる敵
	void DiscoverEnemy(List* _info);
};
