#pragma once

class MapObjBase;
class CameraBase;
class MapManager;
class GameScene;

//マップオブジェクトの管理
class MapObjManager :public KdGameObject
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	MapObjManager() {};

	//デストラクタ
	~MapObjManager() {};

	//---- 基本の処理 ----

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

	//---- 取得 ----

	//更新するマップオブジェクトの一覧を返す
	//戻り値 ... 更新リスト
	std::list<std::shared_ptr<MapObjBase>> GetMapUpdateObj() { return m_mapObjUpdateList; }

	//---- 設定 ----

	//マップオブジェクトを追加
	// _spObj ... 追加するオブジェクト
	void AddMapObj(std::shared_ptr<MapObjBase>_spObj);

	//更新するチャンクにあるオブジェクトを更新リストに入れる
	// _spMapManager ... マップの管理
	void SetMapObjUpdateList(std::shared_ptr<MapManager> _spMapManager);

	//ゲームシーンをセット
	// _spGameScene ... ゲームシーン
	void SetGameScene(std::shared_ptr<GameScene>_spGameScene) { m_wpGameScene = _spGameScene; }

	//---- その他の処理 ----

	//マップオブジェクトを全部消す
	//(更新・描画リストも消して、前の階のオブジェクトがフェードアウトで残らないようにする)
	void ResetMapObj()
	{
		m_mapObjList.clear();
		m_mapObjUpdateList.clear();
		m_mapObjDrawList.clear();
		m_isFirstDraw = true;
	}

	//全部のマップオブジェクトにカメラをセット
	// _spCameraBase ... カメラ
	void ObjSetCamera(std::shared_ptr<CameraBase> _spCameraBase);

	// マップオブジェクトと当たり判定させたい対象を登録
	// _spObj ... 当たり判定させる対象
	void MapObjHit(const std::shared_ptr<KdGameObject>& _spObj);

private:

	//---- メンバ変数 ----

	//全部のマップオブジェクト
	std::list<std::shared_ptr<MapObjBase>>m_mapObjList;

	//更新処理するチャンクにあるオブジェだけ入れる
	std::list<std::shared_ptr<MapObjBase>>m_mapObjUpdateList;

	//描画するチャンクにあるオブジェだけ入れる
	std::list<std::shared_ptr<MapObjBase>>m_mapObjDrawList;

	//ゲームシーン
	std::weak_ptr<GameScene>m_wpGameScene;

	//生成直後の1回目か(最初に描画範囲に入ったものはフェードなしで表示する)
	bool m_isFirstDraw = true;
};
