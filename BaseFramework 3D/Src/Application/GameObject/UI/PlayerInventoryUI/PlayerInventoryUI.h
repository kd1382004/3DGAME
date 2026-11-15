#pragma once
#include"../UIBase.h"
#include"InventoryUIContext.h"

class GameScene;
class PotionUseController;
class PlayerBase;
class PotionTexInfo;

class PotionInventoryUI;
class WeponInventoryUI;
class PlayerStatusInventoryUI;

class PlayerInventoryUI :public UIBase
{
public:

	PlayerInventoryUI() {};
	~PlayerInventoryUI()override {};

	void Init()override;

	void PreUpdate()override;
	void Update()override;

	void PreDraw()override;
	void DrawSprite()override;
	void SetGameScene(std::shared_ptr<GameScene> _spGameScene) { m_wpGameScene = _spGameScene; }


	void SetPotionUseController(std::shared_ptr<PotionUseController> _spPotionUseController) { m_spContext->wpPotionUseController = _spPotionUseController; }
	void SetPlayerBase(std::shared_ptr<PlayerBase> _spPlayerBase) { m_spContext->wpPlayer = _spPlayerBase; }
	void SetPotionTexInfo(std::shared_ptr<PotionTexInfo> _spPotionTexInfo) { m_spContext->wpPotionTexInfo = _spPotionTexInfo; }
private:

	// 各タブで共有する情報（選択中の名前・説明、数字、演出、プレイヤーなど）
	std::shared_ptr<InventoryUIContext> m_spContext = std::make_shared<InventoryUIContext>();

	Math::Vector2 m_selectNameTexPos;
	Math::Vector2 m_selectExplanationTexPos;

	std::weak_ptr<GameScene> m_wpGameScene;

	//インベントリが開いてるかどうか
	bool m_playerInventoryUIFlg = false;

	//インベントリの開閉を管理
	void PlayerInventoryOpen();


	std::shared_ptr<KdTexture>m_back1Tex;
	Math::Vector2 m_back1Tex2DPos;


	std::shared_ptr<KdTexture>m_back2Tex;
	std::shared_ptr<KdTexture>m_Changeback2Tex;
	std::shared_ptr<KdTexture>m_notSelsect;

	Math::Vector2 m_back2Tex2DPos;
	Math::Vector2 m_back2Tex2DSiz;

	std::shared_ptr<KdTexture>m_inventoryTypeTex;
	Math::Vector2 m_inventoryType2DPos;
	Math::Vector2 m_inventoryTypeSiz;

	/////////////////////////////
	//どのインベントリを開いてるか

	enum InventoryType
	{
		PotionInventory,
		WeponInventory,
		PlayerStatus,
		InventoryTypeSiz,
	};

	InventoryType m_nowInventoryType = PotionInventory;


	//どのInventoryにするか
	void InventoryTypeChangeUpdate();

	struct InventoryTypeChange
	{
		InventoryType m_ID;
		Math::Vector2 m_2DPos;
		bool m_hit = false;
		std::shared_ptr<KdTexture>m_IconTex;
		Math::Vector2 m_IconTexSiz;
	};

	InventoryTypeChange m_inventoryTypeChange[InventoryTypeSiz];

	/////////////////////////////
	//各タブ
	std::shared_ptr<PotionInventoryUI>			m_spPotionInventoryUI;
	std::shared_ptr<WeponInventoryUI>			m_spWeponInventoryUI;
	std::shared_ptr<PlayerStatusInventoryUI>	m_spPlayerStatusInventoryUI;

	// InventoryType に対応するタブ
	InventoryTabBase* GetTab(InventoryType _type);
};
