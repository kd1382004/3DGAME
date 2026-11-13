#pragma once
#include"../ResultDisplayBase.h"


class PotionTexInfo;
struct Inventory;

class ResultDisplay_PotionInventory :public ResultDisplayBase
{
public:
	ResultDisplay_PotionInventory() {}
	~ResultDisplay_PotionInventory() {}


	void Init();

	void Update();

	void PreDraw();
	void DrawSprite();


	//あるものを入れてく関数
	void AddPotionTexInfo(std::vector<Inventory>_potions);
private:
	void PotionInfoInit();

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
		std::shared_ptr<KdTexture> m_NameTex;
	};


	std::vector<ItemIconInfo> m_itemIconInfo;
	struct { int w; int h; } m_iconDimensions;

	//選ばれてるポーションのID
	int m_selectPotionID;

	int m_num = 0;

	//ポーションselect
	std::shared_ptr<KdTexture>m_potionSelectTex;


	std::shared_ptr<KdTexture>m_potionTitleTex;
	Math::Vector2 m_potionTitleTexPos;


	std::shared_ptr<KdTexture>m_potionBackTex;
	std::shared_ptr<KdTexture>m_potionBack2Tex;
	Math::Vector2 m_potionBackTexPos;

};
