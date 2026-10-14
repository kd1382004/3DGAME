#pragma once
#include"../../UIBase.h"
#include"../../../../Scene/SceneManager.h"


class ResultDisplay :public UIBase
{
public:
	ResultDisplay() {};
	~ResultDisplay() {};

	void Init()override;

	void Update()override;

	void PreDraw()override;
	void DrawSprite()override;


	void SetPath(std::string path);

	void SetDisplayFloorInfo(std::vector<DisplayFloorInfo>_displayFloorInfo)
	{
		m_displayFloorInfo = _displayFloorInfo;
	}



private:

	std::vector<DisplayFloorInfo>m_displayFloorInfo;

	int m_displayFloorInfoNum = 0;


	Math::Vector2 m_base2DPos;
	Math::Vector2 m_baseNumPos;

	std::shared_ptr<KdTexture>m_OverallResultTex;
	std::shared_ptr<KdTexture>m_HierarchyTex;
	std::shared_ptr<KdTexture>m_DisplayFloorTex;
	std::shared_ptr<KdTexture>m_enemyKillTex;
	std::shared_ptr<KdTexture>m_playTimeScTex;


	////////////////////////////////////////////////
	std::shared_ptr<KdTexture>m_ArrowTex;

	Math::Vector2 m_OverallResultPos;

	Math::Vector2 m_ArrowLPos;
	Math::Vector2 m_ArrowRPos;
	Math::Vector2 m_ArrowSiz;

	bool m_ArrowRHiFlg = false;
	bool m_ArrowLHiFlg = false;

	float m_AroowHitScale = 1.1;

	void ArrowUpdate();

	////////////////////////////////////////////////////////

};
