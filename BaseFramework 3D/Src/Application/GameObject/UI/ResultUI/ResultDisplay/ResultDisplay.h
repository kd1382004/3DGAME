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
	std::shared_ptr<ResultDisplayBase>m_ResultDisplayBase;


};
