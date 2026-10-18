#pragma once
#include"../UIBase.h"

class  VOLUI :public UIBase
{
public:
	VOLUI() {};
	~VOLUI() {};

	void Init()override;
	void Update()override;
	void DrawSprite()override;

	void SetVolPos(Math::Vector2 _pos);

private:

	std::shared_ptr<KdTexture>m_volTex;
	Math::Vector2 m_volPos;
	Math::Vector2 m_volSiz;


	bool m_VolChangeOpenFlg = false;

	std::shared_ptr<KdTexture>m_volBackTex;
	Math::Vector2 m_volBackPos;
	Math::Vector2 m_volBackSiz;


	struct VolInfo
	{
		std::shared_ptr<KdTexture>m_volTex;
		Math::Vector2 m_volPos;
		Math::Vector2 m_volSiz;

		std::shared_ptr<KdTexture>m_volValTex;
		Math::Vector2 m_volValPos;
		Math::Vector2 m_volVaSiz;

		float vol = 0;
	};



	static const int m_volInfoSiz = 3;
	VolInfo m_volInfo[m_volInfoSiz];

};
