#pragma once

class MapObjBase;
class CameraBase;
class MapManager;

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

	void AddMapObj(std::shared_ptr<MapObjBase>_obj) { m_mapObjList.push_back(_obj); }

	void ResetMapObj() { m_mapObjList.clear(); }


	void ObjSetCamera(std::shared_ptr<CameraBase> _spCameraBase);


	void SetMapObjUpdateList(std::shared_ptr<MapManager> _spMapManager);

private:

	std::list<std::shared_ptr<MapObjBase>>m_mapObjList;

	//更新処理するチャンクにあるオブジェだけ入れる
	std::list<std::shared_ptr<MapObjBase>>m_mapObjUpdateList;
};
