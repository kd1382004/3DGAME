#pragma once
#include"../UIBase.h"

class CameraBase;

struct HitDamageDrawInfo {

	Math::Vector3 m_3DPos = Math::Vector3::Zero;
	Math::Vector2 m_2DPos = Math::Vector2::Zero;
	int m_Damage = 0;
	float m_Showtime = 3;
	float m_alpha = 1;
};

class HitDamage :public UIBase
{
public:
	HitDamage() {};
	~HitDamage()override {};


	void Init()override;

	void PreUpdate()override;
	void Update()override;

	void PreDraw()override;
	void DrawSprite()override;


	void SetDamage(int _damage, Math::Vector3 _3DPos)
	{
		HitDamageDrawInfo damageInfo;
		damageInfo.m_3DPos = _3DPos;
		damageInfo.m_Damage = _damage;
		m_hitDamageList.push_back(damageInfo);
	}

	void SetCamera(const std::shared_ptr<CameraBase>& _camera) { m_wpCamera = _camera; }
private:

	std::list<HitDamageDrawInfo>m_hitDamageList;

	std::weak_ptr<CameraBase>m_wpCamera;
};
