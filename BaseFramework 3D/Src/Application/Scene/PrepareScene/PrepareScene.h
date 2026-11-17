#pragma once

#include"../BaseScene/BaseScene.h"

class UIManager;
class ShopDisplay;
class EquipDisplay;
class RecordsDisplay;
class PrepareSceneDisplay;
class ShopEditor;
class GameButton;


// PlayerWeaponInfoはBaseScene.h → SceneManager.hで定義済み

//////////////////////////////////////////////////////////////
// 準備シーン
// Title → Prepare → Game
// ショップでの購入と、プレイヤーの装備変更を行う
//////////////////////////////////////////////////////////////
class PrepareScene : public BaseScene, public std::enable_shared_from_this<PrepareScene>
{
public:

	//コンストラクタ
	PrepareScene() {}
	//デストラクタ
	~PrepareScene()override;

	//ImGuiの表示
	void ImGUI()override;
	//シーンの初期化
	void Init()  override;

	// 表示中のタブ（UIから呼ぶのでpublicへ移動）
	enum class PrepareTab
	{
		None,		// 何も開いていない（メニューだけ）
		Shop,
		Equip,
		Records,
		TabSize
	};

	//表示するタブを切り替える
	// _tab ... 開くタブ
	void ChangeTab(PrepareTab _tab);
	//今開いているタブを取得
	//戻り値 ... 今のタブ
	PrepareTab GetCurrentTab() const { return m_currentTab; }

	// ショップで買った・売ったときに ShopDisplay から呼ぶ
	// ShopDisplay が持っている今の値(お金・武器)を、ほかの画面に配る
	void OnShopDataChanged();

private:

	//シーンのイベント(毎フレーム)
	void Event() override;

	// タブごとの更新
	void ShopEvent();
	//装備のタブを開いているときの処理
	void EquipEvent();

	// 決定した装備をSceneManagerへ反映(Gameシーンへ持ち越す)
	void ApplyEquip();

	// 装備変更画面で選択中の武器
	PlayerWeaponInfo m_selectWeapon;

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
};
