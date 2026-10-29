#pragma once

class MapObjBase;
class CameraBase;
class MapManager;
class GameScene;

class MapObjManager :public KdGameObject
{
public:
	MapObjManager() {};
	~MapObjManager() {};


	// 生成される全てに共通するパラメータに対する初期化のみ
	void Init() override;

	void PreUpdate() override;
	void Update() override;
	void PostUpdate() override;

	// それぞれの状況で描画する関数
	void GenerateDepthMapFromLight()override;
	void PreDraw() override;
	void DrawLit() override;
	void DrawEffect() override;
	void DrawDebug()override;

	void AddMapObj(std::shared_ptr<MapObjBase>_obj);

	void ResetMapObj() { m_mapObjList.clear(); }


	void ObjSetCamera(std::shared_ptr<CameraBase> _spCameraBase);


	void SetMapObjUpdateList(std::shared_ptr<MapManager> _spMapManager);
	void SetMapObjDrawList(std::shared_ptr<MapManager> _spMapManager);

	// マップオブジェクトと当たり判定させたい対象を登録
	void MapObjHit(const std::shared_ptr<KdGameObject>& obj);

	std::list<std::shared_ptr<MapObjBase>> GetMapUpdateObj() { return m_mapObjUpdateList; }

	void SetGameScene(std::shared_ptr<GameScene>_spgameScene) { m_wpGameScene = _spgameScene; }
private:

	std::list<std::shared_ptr<MapObjBase>>m_mapObjList;

	//更新処理するチャンクにあるオブジェだけ入れる
	std::list<std::shared_ptr<MapObjBase>>m_mapObjUpdateList;
	std::list<std::shared_ptr<MapObjBase>>m_mapObjDrawList;

	std::weak_ptr<GameScene>m_wpGameScene;
};
