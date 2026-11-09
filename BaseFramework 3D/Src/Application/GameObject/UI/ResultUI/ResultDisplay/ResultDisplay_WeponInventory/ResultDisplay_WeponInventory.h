#pragma once
#include"../ResultDisplayBase.h"

class WeaponBase;
struct Inventory;

class ResultDisplay_WeponInventory :public ResultDisplayBase
{
public:
	ResultDisplay_WeponInventory() {}
	~ResultDisplay_WeponInventory() {}


	void Init();

	void Update();

	void PreDraw();
	void DrawSprite();


	//武器
	void WeaponsStrengtheningInfoInit(std::vector<Inventory> _weaponsStrengtheningInventory);


	void SetSpWeaponBase(std::shared_ptr<WeaponBase>spWeapon) { m_spWeapon = spWeapon; }
private:		 

	std::shared_ptr<WeaponBase>m_spWeapon;

	struct ItemIconInfo
	{
		int m_ItemID;
		Math::Vector2 m_2DPos;
		int m_num;
		bool m_hit = false;

		std::string m_name;
		std::shared_ptr<KdTexture>m_IconTex;
		std::shared_ptr<KdTexture>m_ExplanationTex;
		std::shared_ptr<KdTexture> m_NameTex;
	};




	//アイコンとの当り判定
	void WeaponsStrengtheningIconHit();

	std::shared_ptr<KdTexture>m_PreconditionNodeExplanationTex;
	std::shared_ptr<KdTexture>m_PreconditionNodeNameTex;
	std::shared_ptr<KdTexture>m_PreconditionNodeTex;
	std::shared_ptr<KdTexture>m_PreconditionNodeIconTex;

	std::shared_ptr<KdTexture>m_selectExplanationTex;
	std::shared_ptr<KdTexture>m_selectNameTex;

	Math::Vector2 m_weaponsStrengtheningButtonTex2DPos;

	std::shared_ptr<KdTexture>m_selectTex;
	std::shared_ptr<KdTexture>m_LVTex;
	Math::Vector2 m_LVTex2DPos;
	Math::Vector2 m_selectTex2DPos;
	float m_selectAdd = 0.1;
	float m_selectDefoAdd = 0.1;
	float m_selectAddNum = 0.05;
	float m_selectAddTotal = 0;
	float m_selectAddTotalMax = 50;

	std::shared_ptr<KdTexture>m_LvMaxButtonTex;

	bool m_PreconditionNodeFlg = false;

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

		std::shared_ptr<KdTexture> m_NameTex;

		//前提ノードID
		int PreconditionNodeID = 0;

		bool ChooseFlg;
	};

	enum WeponStrengtheningID
	{
		//攻撃力
		WeaponsStrengthening_Attck,

		//スタミナ
		WeaponsStrengthening_Stamina,

		//Chargeタイム
		WeaponsStrengthening_ChargeTime,

		//防御無視
		WeaponsStrengthening_DefenseIgnore,

		//武器特有(Lv1になったら他の特有の強化を許す)
		WeaponsStrengthening_Characteristic,

		//武器特有1
		WeaponsStrengthening_Characteristic1,

		//武器特有2
		WeaponsStrengthening_Characteristic2,

		//武器特有3
		WeaponsStrengthening_Characteristic3,

		//武器アイコン
		WeaponsStrengthening_Icon,

		WeaponsStrengtheningSiz
	};

	int m_selectWeaponsStrengtheningID;
	Math::Vector2 m_selectWeaponsStrengtheningIconSiz;

	WeaponsStrengtheningInfo m_weaponsStrengtheningInfo[WeaponsStrengtheningSiz];

	std::vector<ItemIconInfo> m_weaponsStrengtheningIconInfoList;

	//次のLVにするために必要な強化アイテムの数
	int m_selectWeaponsStrengtheningnextLVNum = 0;

	//////////////////////////////////////////////////

	std::shared_ptr<KdTexture>m_WeaponsStrengtheningBackTex;
	Math::Vector2 m_WeaponsStrengtheningTexPos;

	void WeaponChooseFlg(int _ID);
};
