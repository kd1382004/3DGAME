#pragma once

#include"../BaseScene/BaseScene.h"

class TitleSceneEditor;
class MapManager;
class TPSCamera;
class TitelMove;
class MapObjManager;
class EnemyManager;

//タイトルシーン
class TitleScene : public BaseScene
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	TitleScene()  {}

	//デストラクタ
	~TitleScene()override;

	//---- 基本の処理 ----

	//ImGuiの表示
	void ImGUI()override;

	//シーンの初期化
	void Init()  override;

private:

	//---- メンバ変数 ----

	//カメラ
	std::shared_ptr<TPSCamera> m_spCamera;

	//敵の管理
	std::shared_ptr<EnemyManager>m_spEnemyManager;

	//マップの管理
	std::shared_ptr<MapManager>m_spMapManager;

	//タイトルシーンのエディタ
	std::shared_ptr<TitleSceneEditor>m_spTitleSceneEditor;

	//タイトル画面のカメラの動き
	std::shared_ptr<TitelMove> m_spTitleMove;

	//マップ上のオブジェクトの管理
	std::shared_ptr<MapObjManager>m_spMapObjManager;

	//名前からオブジェクトを作る工場
	KdGameObjectFactory m_factory;

	//(使っていない)仮のオブジェクト
	std::shared_ptr<KdGameObject> m_spDummy;

	//音
	std::shared_ptr<KdSoundInstance> m_spTitleBGM;

	//---- 設定 ----

	//タイトル画面のオブジェクトを読み込んで並べる
	void SetObjectList();

	//---- その他の処理 ----

	//シーンのイベント(毎フレーム)
	void Event() override;
};
