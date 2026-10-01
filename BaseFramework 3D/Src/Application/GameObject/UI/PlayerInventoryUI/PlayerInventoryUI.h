#pragma once
#include"../UIBase.h"
class GameScene;
class PotionUseController;
class PlayerBase;
class PotionTexInfo;


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


	void SetPotionUseController(std::shared_ptr<PotionUseController> _spPotionUseController) { m_wpPotionUseController = _spPotionUseController; }
	void SetPlayerBase(std::shared_ptr<PlayerBase> _spPlayerBase) { m_wpPlayerBase = _spPlayerBase; }
	void SetPotionTexInfo(std::shared_ptr<PotionTexInfo> _spPotionTexInfo) { m_wpPotionTexInfo = _spPotionTexInfo; }
private:




	std::weak_ptr<GameScene> m_wpGameScene;
	std::weak_ptr<PotionUseController> m_wpPotionUseController;
	std::weak_ptr<PlayerBase> m_wpPlayerBase;

	//インベントリが開いてるかどうか
	bool m_playerInventoryUIFlg = false;

	//インベントリの開閉を管理
	void PlayerInventoryOpen();


	std::shared_ptr<KdTexture>m_back1Tex;
	Math::Vector2 m_back1Tex2DPos;


	std::shared_ptr<KdTexture>m_back2Tex;
	std::shared_ptr<KdTexture>m_notSelsect;
	Math::Vector2 m_back2Tex2DPos;
	Math::Vector2 m_back2Tex2DSiz;

	std::shared_ptr<KdTexture>m_UseTex;
	Math::Vector2 m_UseTex2DPos;

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

	//各処理用
	void PotionUpdate();
	void WeponUpdate();
	void PlayerStatusUpdate();

	void PotionDraw();
	void WeponDraw();
	void PlayerStatusDraw();
	void PlayerStatusPreDraw();

	/////////////////////////////////////////
	//ポーション系
	std::weak_ptr<PotionTexInfo>m_wpPotionTexInfo;

	//あるものを入れてく関数
	void AddPotionTexInfo();

	//アイコンとマウスが当たってるか
	void IconHit();
	
	struct ItemIconInfo
	{
		int m_ItemID;
		Math::Vector2 m_2DPos;
		int m_num;
		bool m_hit = false;

		std::string m_name;
		std::shared_ptr<KdTexture>m_IconTex;
		std::shared_ptr<KdTexture>m_ExplanationTex;
	};

	
	std::vector<ItemIconInfo> m_itemIconInfo;
	struct { int w; int h; } m_iconDimensions;

	//選ばれてるポーションのID
	int m_selectPotionID;

	//使用
	void PotionIUse();

	int m_num = 0;


	//////////////////////////////////////////////////
	//武器

	
	
	
	//////////////////////////////////////////////////
	//プレイヤーステータス
	std::shared_ptr<KdRenderTargetPack> m_spRtTargetPack;


	struct PlayerStatusInfo
	{
		//HPや攻撃力などの数値
		int num;

		//数値の位置
		Math::Vector2 numPos;

		//数値の名前
		std::shared_ptr<KdTexture>nameTex;

		//数値の名前の位置
		Math::Vector2 namePos;

		//アイコン
		std::shared_ptr<KdTexture>iconTex;

		//アイコンの位置
		Math::Vector2 iconPos;
	};

	enum PlayerStatusInfoID
	{
		PlayerStatusInfo_HP,
		PlayerStatusInfo_MP,
		PlayerStatusInfo_AttackPower,
		PlayerStatusInfo_DefensePower,
		PlayerStatusInfo_Speed,
		PlayerStatusInfoSiz
	};

	PlayerStatusInfo m_playerStatusInfo[PlayerStatusInfoSiz];

	std::shared_ptr<KdTexture>m_playerStatusInfoBackTex;
	


};
