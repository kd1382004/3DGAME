#pragma once
#include"../WeaponBase.h"

class Dagger :public WeaponBase
{
public:
	Dagger() {};
	~Dagger()override {};

	void Init()override;
	void Update()override;
	void DrawLit()override;

private:

	//武器の長さ
	float m_weaponLength = 1.2f;
	//武器の先端のローカル座標
	Math::Vector3 tipLocalPos = {0,m_weaponLength,0};

};
