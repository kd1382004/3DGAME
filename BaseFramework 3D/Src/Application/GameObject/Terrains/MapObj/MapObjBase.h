#pragma once


class CameraBase;

class MapObjBase:public KdGameObject
{
public:
	MapObjBase() {};
	~MapObjBase() override{};



	void SetCamera(std::shared_ptr<CameraBase>_camera) { m_wpCamera = _camera; }


	void SetChunkNum(Math::Vector2 _chunk) { m_chunkNum = _chunk; }
	Math::Vector2 GetChunkNum() { return m_chunkNum; }

	//チャンク内にいるか
	virtual void SetInHaunk(bool _flg) {};
protected:

	std::weak_ptr<CameraBase>m_wpCamera;

	Math::Vector2 m_chunkNum = Math::Vector2::Zero;
};
