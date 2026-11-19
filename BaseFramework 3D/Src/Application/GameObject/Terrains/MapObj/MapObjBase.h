#pragma once
#include"../../../Info/Fader/Fader.h"

class CameraBase;
class GameScene;

class MapObjBase:public KdGameObject
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	MapObjBase() {};

	//デストラクタ
	virtual ~MapObjBase() override{};

	//---- 取得 ----

	//チャンク番号を返す
	//戻り値 ... チャンク番号
	const Math::Vector2 GetChunkNum() { return m_chunkNum; }

	//出現・消失のフェードを返す
	//戻り値 ... フェード
	Fader& GetFader() { return m_fader; }

	//---- 設定 ----

	//カメラをセット
	// _spCamera ... カメラ
	void SetCamera(std::shared_ptr<CameraBase>_spCamera) { m_wpCamera = _spCamera; }

	//チャンク番号をセット
	// _chunk ... チャンク番号
	void SetChunkNum(Math::Vector2 _chunk) { m_chunkNum = _chunk; }

	//チャンク内にいるか
	// _flg ... チャンク内にいるか
	virtual void SetInChunk(bool _flg) {};

	//ゲームシーンをセット
	// _spGameScene ... ゲームシーン
	void SetGameScene(std::shared_ptr<GameScene>_spGameScene) { m_wpGameScene = _spGameScene; }

protected:

	//---- 描画の補助 ----

	//フェード(ディゾルブ)をかけてモデルを描く
	//消え終わっていたら何も描かない
	// _model  ... 描くモデル
	// _isEdge ... ディゾルブの縁を光らせるか(影の描画では false)
	void DrawModelWithFade(KdModelWork& _model, bool _isEdge = true);


	//---- メンバ変数 ----

	//描画範囲に出入りしたときの出現・消失
	Fader m_fader{ 1.0f };

	//ゲームシーン
	std::weak_ptr<GameScene>m_wpGameScene;

	//カメラ
	std::weak_ptr<CameraBase>m_wpCamera;

	//チャンク番号
	Math::Vector2 m_chunkNum = Math::Vector2::Zero;
};
