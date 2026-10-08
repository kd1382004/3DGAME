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

	std::shared_ptr<KdTexture>m_selectNameTex;
	std::shared_ptr<KdTexture>m_NotselectNameTex;
	Math::Vector2 m_selectNameTexPos;

	std::shared_ptr<KdTexture>m_selectExplanationTex;
	std::shared_ptr<KdTexture>m_NotselectExplanationTex;
	Math::Vector2 m_selectExplanationTexPos;




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
	std::shared_ptr<KdTexture>m_Changeback2Tex;
	std::shared_ptr<KdTexture>m_notSelsect;

	Math::Vector2 m_back2Tex2DPos;
	Math::Vector2 m_back2Tex2DSiz;

	std::shared_ptr<KdTexture>m_inventoryTypeTex;
	Math::Vector2 m_inventoryType2DPos;
	Math::Vector2 m_inventoryTypeSiz;

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
	void WeaponsStrengtheningUpdate();
	void PlayerStatusUpdate();

	void PotionDraw();
	void WeaponsStrengtheningDraw();
	void PlayerStatusDraw();
	void PlayerStatusPreDraw();

	/////////////////////////////////////////
	//ポーション系
	std::weak_ptr<PotionTexInfo>m_wpPotionTexInfo;

	void PotionInfoInit();

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

	void WeaponsStrengtheningInfoInit();

	//あるものを入れてく関数
	void WeaponsStrengtheningTexInfo();

	//アイコンとの当り判定
	void WeaponsStrengtheningIconHit();

	//武器強化ボタンとのあたり判定
	void WeaponsStrengtheningButtonHit();

	std::shared_ptr<KdTexture>m_weaponsStrengtheningButtonTex;
	std::shared_ptr<KdTexture>m_LVTex;
	Math::Vector2 m_weaponsStrengtheningButtonTex2DPos;
	Math::Vector2 m_LVTex2DPos;

	struct WeaponsStrengtheningInfo
	{
		//現LV
		int Lv;

		int ID;

		//次のLVにするために必要な強化アイテムの数
		int nextLvNum;

		//強化が可能かどうか
		bool strengtheningFlg = false;

		//アイコン
		std::shared_ptr<KdTexture>iconTex;

		//アイコンの位置
		Math::Vector2 iconPos;

		//アイコンのサイズ
		Math::Vector2 iconSiz;

		//当たってるか
		bool m_hit = false;

		//説明画像
		std::shared_ptr<KdTexture>m_ExplanationTex;


		//強化アイテムの数
		int num;

		//強化アイテムの画像
		std::shared_ptr<KdTexture>iconStrengtheningTex;

		//強化アイテムの位置
		Math::Vector2 iconStrengtheningPos;

		//アイコンのサイズ
		Math::Vector2 iconStrengtheningSiz;

		float timer = 0.0f;
	};

	enum WeponStrengtheningID
	{
		//攻撃力
		WeaponsStrengthening_Attck,

		//スタミナ
		WeaponsStrengthening_Stamina,

		//Chargeタイム
		WeaponsStrengthening_ChargeTime,

		//衝撃波攻撃(これはLV1がマックスでLv1になったらこれ以下の強化を許す)
		WeaponsStrengthening_ShockWave,	

		//衝撃波の飛距離
		WeaponsStrengthening_ShockWave_maxDistanceM,

		//衝撃波の速度
		WeaponsStrengthening_ShockWave_speed,

		//衝撃波の貫通力(何体まで当たっていいか)
		WeaponsStrengthening_ShockWave_hitNum,

		WeaponsStrengtheningSiz
	};

	int m_selectWeaponsStrengtheningID;
	Math::Vector2 m_selectWeaponsStrengtheningIconSiz;
	
	WeaponsStrengtheningInfo m_weaponsStrengtheningInfo[WeaponsStrengtheningSiz];

	ItemIconInfo m_weaponsStrengtheningIconInfo;

	//次のLVにするために必要な強化アイテムの数
	int m_selectWeaponsStrengtheningnextLVNum = 0;

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
