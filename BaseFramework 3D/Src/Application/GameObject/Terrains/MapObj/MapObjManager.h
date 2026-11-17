#pragma once

class MapObjBase;
class CameraBase;
class MapManager;
class GameScene;

class MapObjManager :public KdGameObject
{
public:
	//コンストラクタ
	MapObjManager() {};
	//デストラクタ
	~MapObjManager() {};


	// 生成される全てに共通するパラメータに対する初期化のみ
	void Init() override;

	//更新前の処理
	void PreUpdate() override;
	//更新
	void Update() override;
	//更新後の処理
	void PostUpdate() override;

	// それぞれの状況で描画する関数
	void GenerateDepthMapFromLight()override;
	//描画前の処理(視錐台カリングなど)
	void PreDraw() override;
	//ライトありの描画
	void DrawLit() override;
	//エフェクトの描画
	void DrawEffect() override;
	//デバッグ描画
	void DrawDebug()override;

	//マップオブジェクトを追加
	// _spObj ... 追加するオブジェクト
	void AddMapObj(std::shared_ptr<MapObjBase>_spObj);

	//マップオブジェクトを全部消す
	void ResetMapObj() { m_mapObjList.clear(); }


	//全部のマップオブジェクトにカメラをセット
	// _spCameraBase ... カメラ
	void ObjSetCamera(std::shared_ptr<CameraBase> _spCameraBase);


	//更新するチャンクにあるオブジェクトを更新リストに入れる
	// _spMapManager ... マップの管理
	void SetMapObjUpdateList(std::shared_ptr<MapManager> _spMapManager);
	//描画するチャンクにあるオブジェクトを描画リストに入れる
	// _spMapManager ... マップの管理
	void SetMapObjDrawList(std::shared_ptr<MapManager> _spMapManager);

	// マップオブジェクトと当たり判定させたい対象を登録
	// _spObj ... 当たり判定させる対象
	void MapObjHit(const std::shared_ptr<KdGameObject>& _spObj);

	//更新するマップオブジェクトの一覧を返す
	//戻り値 ... 更新リスト
	std::list<std::shared_ptr<MapObjBase>> GetMapUpdateObj() { return m_mapObjUpdateList; }

	//ゲームシーンをセット
	// _spGameScene ... ゲームシーン
	void SetGameScene(std::shared_ptr<GameScene>_spGameScene) { m_wpGameScene = _spGameScene; }
private:

	//全部のマップオブジェクト
	std::list<std::shared_ptr<MapObjBase>>m_mapObjList;

	//更新処理するチャンクにあるオブジェだけ入れる
	std::list<std::shared_ptr<MapObjBase>>m_mapObjUpdateList;
	//描画するチャンクにあるオブジェだけ入れる
	std::list<std::shared_ptr<MapObjBase>>m_mapObjDrawList;

	//ゲームシーン
	std::weak_ptr<GameScene>m_wpGameScene;
};
