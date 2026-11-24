#pragma once

class UIMap_Map;

//ミニマップの宝箱アイコン
class UIMap_TreasureChest:public KdGameObject
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	UIMap_TreasureChest() {};

	//デストラクタ
	~UIMap_TreasureChest()override {};

	//---- 基本の処理 ----

	//初期化
	void Init()override;

	//2Dの描画
	void DrawSprite()override;

	//---- 取得 ----

	//ミニマップ上の位置を返す
	//戻り値 ... 画面座標
	Math::Vector2 GetPos();

	//---- 設定 ----

	//マップの基準位置(ワールド)をセット
	// _3DPos ... 基準位置(ワールド)
	void SetBase3DPos(Math::Vector3 _3DPos)
	{
		m_base3DPos = _3DPos;
	}

	//ミニマップの基準位置(画面)をセット
	// _basePos ... 基準位置(画面)
	void SetBasePos(Math::Vector2 _basePos) { m_basePos = _basePos; }

	//ミニマップ上のプレイヤーの位置をセット
	// _miniMapPlayerPos ... プレイヤーの位置(画面)
	void SetMiniMapPlayerPos(Math::Vector2 _miniMapPlayerPos) { m_miniMapPlayerPos = _miniMapPlayerPos; }

	//宝箱の座標をセット
	// _3DPos ... 宝箱の座標(ワールド)
	// _worldTileSize ... 1タイルの大きさ
	void SetPos(Math::Vector3 _3DPos, float _worldTileSize);

	//1タイルの大きさをセット
	// _size ... 1タイルの大きさ
	void SetTileSize(float _size) { m_tileSize = _size; }

	//ミニマップの1マスの画像の大きさをセット
	// _size ... 画像の大きさ
	void SetMapTexSize(Math::Vector2 _size) { m_mapTexSize = _size; }

	//マップのUIをセット
	// _spUIMap_Map ... マップのUI
	void SetUIMap_Map(std::shared_ptr<UIMap_Map> _spUIMap_Map) { m_wpUIMap_Map = _spUIMap_Map; }

	//もう表示しない(開けた)かをセット
	// _flg ... もう表示しないか
	void SetExpired(bool _flg) { m_isExpired = _flg; }

private:

	//---- 型 ----

	//ミニマップに出す宝箱1つ分の情報
	struct List
	{
		//宝箱の座標(ワールド)
		Math::Vector3 m_pos;
		//表示するか
		bool m_drawFlg = false;
	};

	//---- メンバ変数 ----

	//マップのUI
	std::weak_ptr<UIMap_Map>m_wpUIMap_Map;

	//ミニマップ上のプレイヤーの位置
	Math::Vector2 m_miniMapPlayerPos;

	//プレイヤーから何マス以内なら見つけたことにするか
	int m_discoverTileNum = 2;

	//宝箱の位置
	List m_posList;

	//ミニマップの1マスの画像の大きさ
	Math::Vector2 m_mapTexSize = { };

	//大きさの倍率
	float m_size = 1.0f;

	//1タイルの大きさ
	float m_tileSize = 10.0f;

	//ミニマップの基準位置(画面)
	Math::Vector2 m_basePos;

	//宝箱のアイコン
	std::shared_ptr<KdTexture>m_spTreasureChestTex;

	//画面上の座標
	Math::Vector3 m_2DPos;

	//マップの基準位置(ワールド)
	Math::Vector3 m_base3DPos;

	//表示するか
	bool m_drawFlg=false;

	//---- その他の処理 ----

	//プレイヤーの近くなら表示するようにする
	// _info ... 調べる宝箱
	void DiscoverTile(List* _info);
};
