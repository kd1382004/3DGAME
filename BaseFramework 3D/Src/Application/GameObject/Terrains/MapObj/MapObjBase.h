#pragma once


class CameraBase;
class GameScene;

class MapObjBase:public KdGameObject
{
public:
	MapObjBase() {};
	virtual ~MapObjBase() override{};



	void SetCamera(std::shared_ptr<CameraBase>_camera) { m_wpCamera = _camera; }


	void SetChunkNum(Math::Vector2 _chunk) { m_chunkNum = _chunk; }
	Math::Vector2 GetChunkNum() { return m_chunkNum; }

	//チャンク内にいるか
	virtual void SetInHaunk(bool _flg) {};
	void SetGameScene(std::shared_ptr<GameScene>_spgameScene) { m_wpGameScene = _spgameScene; }
protected:
	std::weak_ptr<GameScene>m_wpGameScene;
	std::weak_ptr<CameraBase>m_wpCamera;

	Math::Vector2 m_chunkNum = Math::Vector2::Zero;
};
