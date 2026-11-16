#pragma once
#include"../ResultDisplayBase.h"

#include"../../../../../Scene/SceneManager.h" 

class ResultDisplay_Score: public ResultDisplayBase
{
public:
	ResultDisplay_Score() {}
	~ResultDisplay_Score() {}

	void Init()override;

	void Update()override;

	void PreDraw()override;
	void DrawSprite()override;

	void SetDisplayFloorInfo(std::vector<DisplayFloorInfo>_displayFloorInfo);

private:

	Math::Vector2 m_base2DPos;
	Math::Vector2 m_baseNumPos;


	std::shared_ptr<KdTexture>m_backTex;

	////////////////////////////////////////////////

	std::vector<DisplayFloorInfo>m_displayFloorInfo;

	std::shared_ptr<KdTexture>m_OverallResultTex;
	std::shared_ptr<KdTexture>m_HierarchyTex;
	std::shared_ptr<KdTexture>m_DisplayFloorTex;
	std::shared_ptr<KdTexture>m_enemyKillTex;
	std::shared_ptr<KdTexture>m_playTimeScTex;
	std::shared_ptr<KdTexture>m_TimeColonTex;
	std::shared_ptr<KdTexture>m_TreasureChestsNumTex;

	Math::Vector2 m_OverallResultPos;

	////////////////////////////////////////////////////////

};

