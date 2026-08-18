#pragma once
#include"../UIBase.h"

class BuffUI :public UIBase
{
public:
	BuffUI() {};
	~BuffUI() override{};

	void Init()override;
	void Update()override;
	void DrawSprite()override;


private:


	struct BuffUIInfo {

		int m_ID;
		float m_percent = 0;
		Math::Vector2 m_pos;
	};

	Math::Vector2 m_iconSiz;

	std::list<BuffUIInfo> m_PotionBuffUIInfoList;
};
