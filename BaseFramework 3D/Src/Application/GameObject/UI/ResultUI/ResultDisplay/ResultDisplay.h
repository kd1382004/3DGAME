#pragma once
#include"../../UIBase.h"

#include"../../../../Scene/SceneManager.h"
class ResultDisplay_Score;
class ResultDisplayBase;

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

	void SetDisplayFloorInfo(std::vector<DisplayFloorInfo>_displayFloorInfo);



private:
	std::shared_ptr<KdTexture>m_ResultBackTex;

	std::shared_ptr<ResultDisplay_Score>m_ResultDisplay_Score;

	std::vector<std::shared_ptr<ResultDisplayBase>>m_ResultDisplayList;
	int m_CurrentDisplayIndex = 0;



	std::shared_ptr<KdTexture>m_ArrowTex;
	Math::Vector2 m_ArrowLPos;
	Math::Vector2 m_ArrowRPos;
	Math::Vector2 m_ArrowSiz;

	bool m_ArrowRHiFlg = false;
	bool m_ArrowLHiFlg = false;

	float m_AroowHitScale = 1.1;

	float m_AroowAlpha = 1;
	float m_AddAroowAlpha = 1;

	void ArrowUpdate();
	void ArrowDrawSprite();

};
