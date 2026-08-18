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


	std::shared_ptr<KdTexture>m_UseTex;
	Math::Vector2 m_UseTex2DPos;


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
};
