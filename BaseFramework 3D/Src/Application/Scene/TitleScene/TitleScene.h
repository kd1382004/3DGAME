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

	//BGMの音量を今の設定に合わせる(ポーズ中も呼ばれる)
	void ApplyBGMVolume() override;

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

	//---- データのリセット(ImGuiから呼ぶ) ----

	//ショップの在庫を最初の数に戻す(ShopMasterData の startStock)
	void ResetShopStock();

	//プレイヤーのデータを最初の状態に戻す
	//(ステータス・持ち物・所持金・持っている武器・武器の強化・装備中の武器)
	void ResetPlayerData();

	//歴代の記録(最新・ベスト3)を消す
	void ResetGameRecord();

	//確認つきのリセットボタン
	// _label ... ボタンの文字
	// _popupId ... 確認ウィンドウの名前(ボタンごとに別にする)
	//戻り値 ... 「はい」が押されたらtrue
	bool ResetButton(const char* _label, const char* _popupId);

	//プレイヤーの最初のステータスのファイルのパス
	const std::string m_playerDefaultStatusFilePath = "Asset/Data/ObjeData/Character/Praeyr/Status/Status.json";

	//装備中の武器のファイルのパス
	const std::string m_playerEquipFilePath = "Asset/Data/PlayerData/PlayerEquipData.json";
};
