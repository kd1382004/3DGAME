#pragma once
#include"../UIBase.h"

class PotionTexInfo;

struct GetItem
{
	//入手数
	int m_getNum = 0;

	//アイコン画像
	std::shared_ptr<KdTexture> m_spIconTex = nullptr;

	//名前
	std::shared_ptr<KdTexture> m_spNameTex = nullptr;

	//表示時間
	float m_showTime = 2;

	//表示が終わったか
	bool m_isFinished = false;

	//表示位置
	Math::Vector2 m_pos = Math::Vector2::Zero;

	//アイコン位置
	Math::Vector2 m_iconPos = Math::Vector2::Zero;

	//名前位置
	Math::Vector2 m_namePos = Math::Vector2::Zero;

	//アルファ値
	float m_alpha = 1;

	//表示を始めたか
	bool m_drawFlg = false;

	//アイテムの番号
	int m_ID;
};

enum ItemType
{
	Potion,
	WeaponsStrengthening
};

class ItemGetUIController :public UIBase
{
public:
	//コンストラクタ
	ItemGetUIController() {};
	//デストラクタ
	~ItemGetUIController() {};

	//初期化
	void Init()override;
	//更新前の処理
	void PreUpdate()override;
	//更新
	void Update()override;
	//2Dの描画
	void DrawSprite()override;


	//手に入れたアイテムを表示に追加
	// _item ... 手に入れたアイテム
	// _type ... アイテムの種類
	void AddGetItemList(GetItem _item, ItemType _type);
private:

	//表示するアイテムの一覧
	std::list<GetItem> m_getItemList;
	
	//表示のBase位置
	Math::Vector2 m_basePos;

	//アイコンの大きさ
	Math::Vector2 m_iconWH;

	//背景の画像
	std::shared_ptr<KdTexture>m_spBackTex;
	//背景の大きさ
	Math::Vector2 m_backWH;

	//背景2の画像
	std::shared_ptr<KdTexture>m_spBack2Tex;
	//アルファ
	float m_alpha;

	//ポーションの画像一覧
	std::shared_ptr<PotionTexInfo>m_spPotionTexInfo;
};
