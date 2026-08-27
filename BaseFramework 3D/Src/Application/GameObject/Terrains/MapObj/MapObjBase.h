#pragma once

class CameraBase;

class MapObjBase:public KdGameObject
{
public:
	MapObjBase() {};
	~MapObjBase() override{};



	void SetCamera(std::shared_ptr<CameraBase>_camera) { m_wpCamera = _camera; }

protected:

	std::weak_ptr<CameraBase>m_wpCamera;


};
