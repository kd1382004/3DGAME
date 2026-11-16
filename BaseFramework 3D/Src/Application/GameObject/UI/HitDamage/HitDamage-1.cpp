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
		if (it->m_timer >= m_popTime + m_holdTime + m_fadeTime)
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
	float Gdt = DeltaTime::Instance().GetGameDeltaTime();
	float Rdt = DeltaTime::Instance().GetRealDeltaTime();

	std::shared_ptr<CameraBase>spCamera = m_wpCamera.lock();
	if (spCamera)
	{
		for (auto& damage : m_hitDamageList)
		{
			Math::Vector3 set3DPos = damage.m_3DPos;
			Math::Vector3 set2DPos;
			set3DPos.y += 2.5;
			spCamera->GetCamera()->ConvertWorldToScreenDetail(set3DPos, set2DPos);
			damage.m_2DPos = { set2DPos.x,set2DPos.y };

			damage.m_timer += Rdt;

			//---------- 出た直後（0 〜 m_popTime）：大きく出て縮む ----------
			if (damage.m_timer < m_popTime)
			{
				// 割合（0 → 1）
				float t = damage.m_timer / m_popTime;

				// 最初速く、最後ゆっくり（1 - (1-t)^3）
				float ease = 1.0f - powf(1.0f - t, 3.0f);

				// m_popScale → 1 に縮める
				damage.m_scale = m_popScale + (1.0f - m_popScale) * ease;
			}
			else
			{
				damage.m_scale = 1.0f;
			}

			//---------- 消える時間（m_popTime + m_holdTime 〜 + m_fadeTime）：浮きながら消える ----------
			float fadeStart = m_popTime + m_holdTime;
			if (damage.m_timer >= fadeStart)
			{
				// 割合（0 → 1）。最後は 1 で止める
				float t = (damage.m_timer - fadeStart) / m_fadeTime;
				t = std::clamp(t, 0.0f, 1.0f);

				// 上に浮く
				damage.m_floatY = m_floatDist * t;

				// だんだん透明に
				damage.m_alpha = 1.0f - t;
			}
			else
			{
				// 出た直後・見せる時間は止まって、はっきり見せる
				damage.m_floatY = 0.0f;
				damage.m_alpha = 1.0f;
			}
		}
	}
}

void HitDamage::PreDraw()
{

}

void HitDamage::DrawSprite()
{
	for (const auto& damage : m_hitDamageList)
	{
		//---------- 種類ごとの色と大きさの倍率 ----------
		Math::Color color = { 1.0f, 1.0f, 1.0f, 1.0f };	// 白
		float typeScale = 1.0f;

		switch (damage.m_type)
		{
		case HitDamageType::Normal:		// 普通：白
			color = { 1.0f, 1.0f, 1.0f, 1.0f };
			typeScale = 1.0f;
			break;
		case HitDamageType::Critical:	// 会心：オレンジ
			color = { 1.0f, 0.6f, 0.2f, 1.0f };
			typeScale = 1.3f;
			break;
		case HitDamageType::Kill:		// とどめ：金色
			color = { 1.0f, 0.85f, 0.3f, 1.0f };
			typeScale = 1.6f;
			break;
		case HitDamageType::Player:		// プレイヤーが受けた：赤
			color = { 1.0f, 0.25f, 0.25f, 1.0f };
			typeScale = 1.2f;
			break;
		}
		color.w = damage.m_alpha;

		//---------- 大きさと位置 ----------
		// 大きさ = 基本 × 種類の倍率 × 出た瞬間の弾み
		float size = m_baseSize * typeScale * damage.m_scale;

		// 浮き上がる分を足す（2D は上がプラス）
		Math::Vector2 pos = damage.m_2DPos;
		pos.y += damage.m_floatY;

		//---------- 影（先に描く） ----------
		// ずらす量は文字の 1 ドット分（大きい数字でも影が見えるように）
		float shadowOff = size;
		Math::Color shadowColor = { 0.0f, 0.0f, 0.0f, damage.m_alpha * 0.6f };
		NumDraw::GetInstance().Drow(damage.m_Damage, CAligned, { pos.x + shadowOff, pos.y - shadowOff }, shadowColor, size);

		//---------- 本体 ----------
		NumDraw::GetInstance().Drow(damage.m_Damage, CAligned, pos, color, size);
	}
}
