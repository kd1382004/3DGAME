#pragma once


class CameraBase;
class GameScene;

class MapObjBase:public KdGameObject
{
public:
	//コンストラクタ
	MapObjBase() {};
	//デストラクタ
	virtual ~MapObjBase() override{};



	//カメラをセット
	// _spCamera ... カメラ
	void SetCamera(std::shared_ptr<CameraBase>_spCamera) { m_wpCamera = _spCamera; }


	//チャンク番号をセット
	// _chunk ... チャンク番号
	void SetChunkNum(Math::Vector2 _chunk) { m_chunkNum = _chunk; }
	//チャンク番号を返す
	//戻り値 ... チャンク番号
	const Math::Vector2 GetChunkNum() { return m_chunkNum; }

	//チャンク内にいるか
	// _flg ... チャンク内にいるか
	virtual void SetInChunk(bool _flg) {};
	//ゲームシーンをセット
	// _spGameScene ... ゲームシーン
	void SetGameScene(std::shared_ptr<GameScene>_spGameScene) { m_wpGameScene = _spGameScene; }
protected:
	//ゲームシーン
	std::weak_ptr<GameScene>m_wpGameScene;
	//カメラ
	std::weak_ptr<CameraBase>m_wpCamera;

	//チャンク番号
	Math::Vector2 m_chunkNum = Math::Vector2::Zero;
};
