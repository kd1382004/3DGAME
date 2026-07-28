#pragma once

class CameraBase;
class PlayerBase;

enum MapObjType
{
	None,       // 何でもない
	Ground,     // 地面
	Wall        // 壁
};

class MapBase :public KdGameObject
{
public:
	MapBase() {};
	~MapBase() {};


	void Init()override;
	void Update()override;
	void DrawLit()override;
	void GenerateDepthMapFromLight()override;
	void PreDraw()override;


	void SetRotation(Math::Matrix _rMat);

	void SetCamera(std::shared_ptr<CameraBase> _spCamera) { m_wpCamera = _spCamera; }

	void SetMapObjType(MapObjType _mapObjType) { m_mapObjType = _mapObjType; }
	MapObjType GetMapObjType() { return m_mapObjType; }

	void SetPlayer(std::shared_ptr<PlayerBase> _spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }

protected:

	std::weak_ptr<PlayerBase> m_wpPlayerBase;

	std::weak_ptr<CameraBase>m_wpCamera;

	std::shared_ptr<KdModelWork>m_spModel = nullptr;

	//視錐台用のBoxInfo
	KdCollider::BoxInfo m_frustumBox;

	bool m_isInView = false;


	MapObjType m_mapObjType = MapObjType::None;
};

