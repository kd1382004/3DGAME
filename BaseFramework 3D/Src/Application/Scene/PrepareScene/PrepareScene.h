#pragma once

#include"../BaseScene/BaseScene.h"

class UIManager;
class ShopDisplay;
class EquipDisplay;
class RecordsDisplay;
class PrepareSceneDisplay;


// PlayerWeaponInfoはBaseScene.h → SceneManager.hで定義済み

//////////////////////////////////////////////////////////////
// 準備シーン
// Title → Prepare → Game
// ショップでの購入と、プレイヤーの装備変更を行う
//////////////////////////////////////////////////////////////
class PrepareScene : public BaseScene, public std::enable_shared_from_this<PrepareScene>
{
public:

	PrepareScene() {}
	~PrepareScene()override;

	void ImGUi()override;
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

	void ChangeTab(PrepareTab _tab);
	PrepareTab GetCurrentTab() const { return m_currentTab; }

private:

	void Event() override;

	// タブごとの更新
	void ShopEvent();
	void EquipEvent();

	// 決定した装備をSceneManagerへ反映(Gameシーンへ持ち越す)
	void ApplyEquip();

	// 装備変更画面で選択中の武器
	PlayerWeaponInfo m_selectWeapon;

	std::shared_ptr<UIManager> m_spUIManager;

	//音
	std::shared_ptr<KdSoundInstance> m_spPrepareBGM;


	PrepareTab m_currentTab = PrepareTab::None;

	std::shared_ptr<ShopDisplay>    m_spShopDisplay;
	std::shared_ptr<EquipDisplay>   m_spEquipDisplay;
	std::shared_ptr<RecordsDisplay> m_spRecordsDisplay;
	std::shared_ptr<PrepareSceneDisplay> m_spPrepareSceneDisplay;
};
