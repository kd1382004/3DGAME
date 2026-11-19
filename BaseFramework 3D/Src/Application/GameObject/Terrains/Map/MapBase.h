#pragma once

class CameraBase;
class PlayerBase;

//マップオブジェクトの種類
enum MapObjType
{
	None,       // 何でもない
	Ground,     // 地面
	Wall,        // 壁
	Stairs,       // 階段
	TypeSlope       // 階段
};


//部屋の種類
enum RoomType
{
	//部屋じゃない
	RoomType_NORoom,




	//敵が多く出てくる部屋 --- 部屋の種類増やすときはこの中↓ ---
	RoomType_EnemyRoom,





	//宝箱が多い部屋 --- 部屋の種類増やすときはこの中↑ ---
	RoomType_TreasureChestRoom,



	//敵がわかない部屋
	RoomType_SafeRoom,

	//プレイヤースポーン部屋
	RoomType_PlayerSpawn,

	//階段部屋
	RoomType_Stairs
};




class MapBase :public KdGameObject
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	MapBase() {};

	//デストラクタ
	~MapBase() {};

	//---- 基本の処理 ----

	//初期化
	void Init()override;

	//更新
	void Update()override;

	//更新後の処理
	void PostUpdate()override;

	//ライトありの描画
	void DrawLit()override;

	//影の描画
	void GenerateDepthMapFromLight()override;

	//描画前の処理(視錐台カリングなど)
	void PreDraw()override;

	//---- 取得 ----

	//マップオブジェクトの種類を返す
	//戻り値 ... 種類
	MapObjType GetMapObjType() { return m_mapObjType; }

	//部屋の種類を返す
	//戻り値 ... 部屋の種類
	RoomType GetRoomType() { return m_roomType; }

	//部屋IDを返す
	//戻り値 ... 部屋ID
	int GetRoomID() { return m_roomID; }

	//---- 設定 ----

	//回転をセット
	// _rMat ... 回転行列
	void SetRotation(Math::Matrix _rMat);

	//カメラをセット
	// _spCamera ... カメラ
	void SetCamera(std::shared_ptr<CameraBase> _spCamera) { m_wpCamera = _spCamera; }

	//マップオブジェクトの種類をセット
	// _mapObjType ... 種類
	void SetMapObjType(MapObjType _mapObjType) { m_mapObjType = _mapObjType; }

	//プレイヤーをセット
	// _spPlayerBase ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase> _spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }

	//部屋の種類をセット
	// _roomType ... 部屋の種類
	void SetRoomType(RoomType _roomType) { m_roomType = _roomType; }

	//部屋IDをセット
	// _ID ... 部屋ID
	void SetRoomID(int _ID) { m_roomID = _ID; }

	//モデルをセット
	// _spModel ... モデル
	void SetModel(std::shared_ptr<KdModelWork>_spModel) { m_spModel = _spModel; }


	void SetUVTiling(const Math::Vector2& _tiling) { m_uvTiling = _tiling; }

protected:

	//---- メンバ変数 ----

	//プレイヤー
	std::weak_ptr<PlayerBase> m_wpPlayerBase;

	//カメラ
	std::weak_ptr<CameraBase>m_wpCamera;

	//モデル
	std::shared_ptr<KdModelWork>m_spModel = nullptr;

	//視錐台用のBoxInfo
	KdCollider::BoxInfo m_frustumBox;

	//カメラに映っているか
	bool m_isInView = false;

	//マップオブジェクトの種類
	MapObjType m_mapObjType = MapObjType::None;

	//部屋の種類を監理
	RoomType m_roomType = RoomType_NORoom;

	//部屋なら部屋番号を保存
	int m_roomID = -999;

	//テクスチャのくり返し回数。x が横、y が縦
	Math::Vector2 m_uvTiling = { 1.0f, 1.0f };
};

