#include "HitDamage.h"

#include"../../../Info/NumDraw/NumDraw.h"
#include"../../Camera/CameraBase.h"
void HitDamage::Init()
{

}

void HitDamage::PreUpdate()
{
	auto it = m_hitDamageList.begin();

	while (it != m_hitDamageList.end())
	{
		if (it->m_alpha <= 0.0f)	// IsExpired() ・・・ 無効ならtrue
		{
			// 無効なオブジェクトをリストから削除
			it = m_hitDamageList.erase(it);
		}
		else
		{
			++it;	// 次の要素へイテレータを進める
		}
	}
}

void HitDamage::Update()
{
	float dt = DeltaTime::Instance().GetGameDeltaTime();

	std::shared_ptr<CameraBase>spCamera = m_wpCamera.lock();
	if (spCamera)
	{
		for (auto& damage : m_hitDamageList)
		{
			Math::Vector3 set3DPos = damage.m_3DPos;
			Math::Vector3 set2DPos;
			damage.m_3DPos.y += dt;
			set3DPos.y += 2.5;
			spCamera->GetCamera()->ConvertWorldToScreenDetail(set3DPos, set2DPos);
			damage.m_2DPos = { set2DPos.x,set2DPos.y };
			damage.m_alpha -= dt;
		}
	}
}

void HitDamage::PreDraw()
{

}

void HitDamage::DrawSprite()
{

	for (const auto damage : m_hitDamageList)
	{
		Math::Color color = { 1,1,1,damage.m_alpha };
		NumDraw::GetInstance().Drow(damage.m_Damage, LAligned, damage.m_2DPos, color,2);
	}


}
