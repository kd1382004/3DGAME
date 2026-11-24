#pragma once

#include"../BaseScene/BaseScene.h"
#include"../../GameObject/Terrains/Camp/CampLayoutData/CampLayoutData.h"

class UIManager;
class ShopDisplay;
class EquipDisplay;
class RecordsDisplay;
class PrepareSceneDisplay;
class ShopEditor;
class CharaStatusEditor;
class GameButton;
class CampObject;
class PrepareCamera;
class CampLight;
class PrepareLayoutEditor;
//出発演出の状態(None:していない / Walk:入口へ歩いている / Fade:暗転中)
enum class DepartState { None, Walk, Fade };
//////////////////////////////////////////////////////////////
// 準備シーン
// Title → Prepare → Game
// ショップでの購入と、プレイヤーの装備変更を行う
//////////////////////////////////////////////////////////////
class PrepareScene : public BaseScene, public std::enable_shared_from_this<PrepareScene>
{
public:

	//---- 型 ----

	// 表示中のタブ（UIから呼ぶのでpublicへ移動）
	enum class PrepareTab
	{
		None,		// 何も開いていない（メニューだけ）
		Shop,
		Equip,
		Records,
		TabSize
	};

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	PrepareScene() {}

	//デストラクタ
	~PrepareScene()override;

	//---- 基本の処理 ----

	//ImGuiの表示
	void ImGUI()override;

	//シーンの初期化
	void Init()  override;

	//---- 取得 ----

	//今開いているタブを取得
	//戻り値 ... 今のタブ
	PrepareTab GetCurrentTab() const { return m_currentTab; }

	//---- その他の処理 ----

	//表示するタブを切り替える
	// _tab ... 開くタブ
	void ChangeTab(PrepareTab _tab);

	// ショップで買った・売ったときに ShopDisplay から呼ぶ
	// ShopDisplay が持っている今の値(お金・武器)を、ほかの画面に配る
	void OnShopDataChanged();

	//出発の演出を始める(出発ボタンを押したときに呼ぶ)
	void StartDepart();

	//出発の演出を進める(プレイヤーを入口へ歩かせて、暗転したらゲームシーンへ)
	void UpdateDepart();

	//武器立ての武器の表示を更新する
	//持っている武器だけを出し、装備中の武器は(プレイヤーが持っているので)隠す
	void UpdateRackWeapons();

private:

	//---- メンバ変数 ----

	// 装備変更画面で選択中の武器
	PlayerWeaponInfo m_selectWeapon = PlayerWeaponInfo_Dagger;

	//UIの管理
	std::shared_ptr<UIManager> m_spUIManager;

	//音
	std::shared_ptr<KdSoundInstance> m_spPrepareBGM;

	//今開いているタブ
	PrepareTab m_currentTab = PrepareTab::None;

	//ショップの画面
	std::shared_ptr<ShopDisplay>    m_spShopDisplay;

	//装備の画面
	std::shared_ptr<EquipDisplay>   m_spEquipDisplay;

	//記録の画面
	std::shared_ptr<RecordsDisplay> m_spRecordsDisplay;

	//メニューの画面
	std::shared_ptr<PrepareSceneDisplay> m_spPrepareSceneDisplay;

	// 出発ボタン(→ゲームシーン)。メニューだけのとき(PrepareTab::None)だけ出す
	std::shared_ptr<GameButton> m_spGameButton;

	// ショップの値段を決めるEditor(ImGui)
	std::shared_ptr<ShopEditor> m_spShopEditor;

	//ショップのエディタを開いているか
	bool m_shopEditorOpenFlg = false;

	// キャラの初期ステータスを決めるEditor(ImGui)
	std::shared_ptr<CharaStatusEditor> m_spCharaStatusEditor;

	//キャラの初期ステータスのエディタを開いているか
	bool m_charaStatusEditorOpenFlg = false;

	// 背景(キャンプ)のカメラ・ライトを調整するEditor(ImGui)
	std::shared_ptr<PrepareLayoutEditor> m_spLayoutEditor;

	//背景とカメラのエディタを開いているか
	bool m_layoutEditorOpenFlg = false;

	//---- その他の処理 ----

	//シーンのイベント(毎フレーム)
	void Event() override;

	// タブごとの更新
	void ShopEvent();

	//装備のタブを開いているときの処理
	void EquipEvent();

	// 決定した装備をSceneManagerへ反映(Gameシーンへ持ち越す)
	void ApplyEquip();


	//---- キャンプ(背景)関係のメンバ変数 ----

	//Blenderから書き出したキャンプの配置データのパス
	const std::string m_campLayoutFilePath = "Asset/Data/ObjeData/PrepareObjeData/CampLayout.json";

	//エディタで調整した値(カメラ・ライト・出発演出)のパス
	const std::string m_campLayoutTuneFilePath = "Asset/Data/ObjeData/PrepareObjeData/CampLayoutTune.json";

	//キャンプの配置データ
	CampLayoutData m_campLayout;

	//準備シーン用カメラ
	std::shared_ptr<PrepareCamera> m_spPrepareCamera;

	//キャンプの置物
	std::vector<std::shared_ptr<CampObject>> m_campObjects;

	//武器立てに置いてある武器(武器の種類ごと。置いていない武器は空)
	std::weak_ptr<CampObject> m_wpRackWeapons[PlayerWeaponInfoSize];

	//キャンプのライト
	std::vector<std::shared_ptr<CampLight>> m_campLights;

	//キャンプで出しているエフェクト(焚き火など)
	std::vector<std::weak_ptr<KdEffekseerObject>> m_wpKdEffekseerObject;

	//入口の奥のエフェクト(エディタで位置を動かすために覚えておく)
	std::weak_ptr<KdEffekseerObject> m_wpEntranceEffect;

	// 準備画面(屋外・夕方)の霧と環境光
	// ダンジョン用の値(霧 0.3 灰色・濃さ 0.02 / 環境光 0.5)のままだと暗く濁るので、準備画面だけ変える
	// 次のシーンに移るときは SceneManager::ChangeScene がダンジョン用の値に戻すので、戻す処理は要らない
	Math::Vector3 m_campFogColor = { 0.32f, 0.36f, 0.48f };	// 夕方の青っぽい空気
	float         m_campFogDensity = 0.012f;					// 奥の崖がうっすら霞むくらい
	Math::Vector4 m_campAmbient = { 0.6f, 0.62f, 0.75f, 0.5f };	// 少し青寄り。オレンジの光と対比させる


	//キャンプに立っているプレイヤーの置物(出発の演出で歩かせる)
	std::weak_ptr<CampObject>m_wpCampPlayer;

	//---- 出発演出のメンバ変数 ----

	//出発演出の状態

	DepartState m_departState = DepartState::None;
	float m_departTimer = 0.0f;//歩き始めてからの秒数
	float m_departFadeTimer = 0.0f;//暗転を始めてからの秒数
	//歩き始めた位置
	Math::Vector3 m_departStartPos;
};
