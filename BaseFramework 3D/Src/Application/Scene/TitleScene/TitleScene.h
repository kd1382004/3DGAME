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
	// _label   ... ボタンの文字
	// _popupId ... 確認ウィンドウの名前(ボタンごとに別にする)
	// _message ... 確認ウィンドウに出す文(nullptr なら「本当にリセットしますか？」)
	//戻り値 ... 「はい」が押されたらtrue
	bool ResetButton(const char* _label, const char* _popupId, const char* _message = nullptr);

	//---- 発表用データ(ImGuiから呼ぶ) ----

	//今のプレイ用データ(PlayerData フォルダ)を、発表用データ(PresentationData フォルダ)に丸ごと写す
	//戻り値 ... 全部写せたらtrue
	bool SavePresentationData();

	//発表用データ(PresentationData フォルダ)を、プレイ用データ(PlayerData フォルダ)に丸ごと写す
	//装備中の武器は SceneManager も覚えているので、それも読み直す
	//戻り値 ... 全部写せたらtrue
	bool LoadPresentationData();

	//フォルダ _fromDir のプレイヤーデータを _toDir に写す(m_playerDataFileNames のファイルだけ)
	// _fromDir ... 写す元のフォルダ
	// _toDir   ... 写す先のフォルダ(無ければ作る)
	//戻り値 ... 全部写せたらtrue
	bool CopyPlayerDataFiles(const std::string& _fromDir, const std::string& _toDir);

	//プレイ用データのフォルダ
	const std::string m_playerDataDir = "Asset/Data/PlayerData/";

	//発表用データのフォルダ
	const std::string m_presentationDataDir = "Asset/Data/PresentationData/";

	//プレイヤーデータとして写すファイル(ステータス・持ち物とお金・武器の強化・装備・記録・ショップの在庫)
	const std::vector<std::string> m_playerDataFileNames =
	{
		"PlayerStatusData.json",
		"PlayerInventoryData.json",
		"PlayerWeaponStrengtheningData.json",
		"PlayerEquipData.json",
		"GameRecordData.json",
		"ShopStockData.json",
	};

	//プレイヤーの最初のステータスのファイルのパス
	const std::string m_playerDefaultStatusFilePath = "Asset/Data/ObjeData/Character/Praeyr/Status/Status.json";

	//装備中の武器のファイルのパス
	const std::string m_playerEquipFilePath = "Asset/Data/PlayerData/PlayerEquipData.json";
};
