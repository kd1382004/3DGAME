#pragma once
#include"../../UIBase.h"

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

	void SetNumDraw(bool _flg) { m_numDrawFlg = _flg; }
	void SetNum(int _num) { m_setNum = _num; }
private:

	std::shared_ptr<KdTexture> m_tex;

	bool m_numDrawFlg = true;

	int m_setNum = 0;
};
