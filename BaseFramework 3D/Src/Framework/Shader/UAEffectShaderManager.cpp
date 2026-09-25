#include "UAEffectShaderManager.h"
#include "Framework/KdFramework.h"

void UAEffectShaderManager::Init()
{
	m_cb10_Effect.Create();
	m_cb10_Effect.Write();
	KdDirect3D::Instance().WorkDevContext()->VSSetConstantBuffers(10, 1, m_cb10_Effect.GetAddress());
	KdDirect3D::Instance().WorkDevContext()->PSSetConstantBuffers(10, 1, m_cb10_Effect.GetAddress());
}

void UAEffectShaderManager::ClearEffect()
{
	auto& cb = m_cb10_Effect.Work();
	cb.circleEffectNum = 0;
	cb.boxEffectNum = 0;
	m_cb10_Effect.Write();



}

void UAEffectShaderManager::AddCircleEffect(Math::Vector3 pos, float radius, Math::Vector3 color)
{
	auto& cb = m_cb10_Effect.Work();
	if (cb.circleEffectNum >= 10) return; // 上限

	cb.circleEffects[cb.circleEffectNum].colorPos = pos;
	cb.circleEffects[cb.circleEffectNum].colorRadius = radius;
	cb.circleEffects[cb.circleEffectNum].colorColor = color;
	cb.circleEffectNum++;
	cb.circleColorEnable = true;

	m_cb10_Effect.Write();
}

void UAEffectShaderManager::WriteCBCircleEffectEnable(bool enable)
{
	//データをセット
	m_cb10_Effect.Work().circleColorEnable = enable;
	//GPUに転送
	m_cb10_Effect.Write();
}

void UAEffectShaderManager::WriteCBCircleEffect(Math::Vector3 pos, float radius, Math::Vector3 color)
{
	AddCircleEffect(pos, radius, color);
}

void UAEffectShaderManager::WriteCBBoxEffectEnable(bool enable)
{
	//データをセット
	m_cb10_Effect.Work().boxColorEnable = enable;
	//GPUに転送
	m_cb10_Effect.Write();
}

void UAEffectShaderManager::WriteCBBoxEffect(Math::Vector3 pos, Math::Vector3 boxSiz, float worldAngleY, Math::Vector3 color)
{
		auto& cb = m_cb10_Effect.Work();

		// 追加できるかチェック（最大10個）
		if (cb.boxEffectNum >= 10)
		{
			// これ以上追加できない
			return;
		}

		// 書き込み先のインデックス
		int idx = cb.boxEffectNum;

		cb.boxEffects[idx].colorPos = pos;
		cb.boxEffects[idx].colorRadiusX = boxSiz.x;
		cb.boxEffects[idx].colorRadiusY = boxSiz.y;
		cb.boxEffects[idx].colorRadiusZ = boxSiz.z;
		cb.boxEffects[idx].worldAngleY = worldAngleY;
		cb.boxEffects[idx].colorColor = color;
	

		// 有効エフェクト数を増やす
		cb.boxEffectNum++;

		// GPU へ書き込み
		m_cb10_Effect.Write();
}

void UAEffectShaderManager::Release()
{
	m_cb10_Effect.Release();
}

