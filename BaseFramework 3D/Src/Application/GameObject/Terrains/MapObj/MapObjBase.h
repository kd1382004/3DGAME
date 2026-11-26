#pragma once
#include"../../../Info/Fader/Fader.h"

class CameraBase;
class GameScene;

//マップオブジェクトの基底クラス
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

	//追加のチャンク番号の一覧を返す(部屋の松明など、部屋がまたがるチャンク全部)
	//戻り値 ... チャンク番号の一覧(空なら m_chunkNum だけ)
	const std::vector<Math::Vector2>& GetChunkNumList() const { return m_chunkNumList; }

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

	//追加のチャンク番号の一覧をセット
	//部屋の壁と同じく、部屋がまたがっているチャンクのどれかが範囲内なら更新・描画する
	// _list ... チャンク番号の一覧
	void SetChunkNumList(const std::vector<Math::Vector2>& _list) { m_chunkNumList = _list; }

	//チャンク内にいるか
	// _flg ... チャンク内にいるか
	virtual void SetInChunk(bool _flg) {};

	//鳴っている音の音量を今の設定に合わせる(音を持つものだけ上書きする)
	virtual void ApplySoundVolume() {}

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

	//追加のチャンク番号(部屋がまたがるチャンク全部。空なら m_chunkNum だけを見る)
	std::vector<Math::Vector2> m_chunkNumList;
};
