#pragma once
#include"../UIBase.h"

class PotionTexInfo;

struct GetItem
{
	//入手数
	int GetNum = 0;

	//アイコン画像
	std::shared_ptr<KdTexture> IconTex = nullptr;

	//表示時間
	float showTime = 1;

	//表示が終わったか
	bool isFinished = false;

	//表示位置
	Math::Vector2 m_pos = Math::Vector2::Zero;

	//アイコン位置
	Math::Vector2 m_iconPos = Math::Vector2::Zero;

	//アルファ値
	float alpha = 1;

	bool drawFlg = false;

	int ID;
};

class ItemGetUIController :public UIBase
{
public:
	ItemGetUIController() {};
	~ItemGetUIController() {};

	void Init()override;
	void PreUpdate()override;
	void Update()override;
	void DrawSprite()override;


	void AddGetItemList(GetItem Item);
private:

	std::list<GetItem> m_getItemList;
	
	//表示のBase位置
	Math::Vector2 m_basePos;

	//アイコンの大きさ
	Math::Vector2 m_IconWH;

	std::shared_ptr<KdTexture>m_backTex;
	Math::Vector2 m__backWH;

	std::shared_ptr<KdTexture>m_back2Tex;
	float m_alpha;

	std::shared_ptr<PotionTexInfo>m_spPotionTexInfo;
};
