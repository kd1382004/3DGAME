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
	cb.effectNum = 0;
	m_cb10_Effect.Write();
}

void UAEffectShaderManager::AddEffect(Math::Vector3 pos, float radius, Math::Vector3 color)
{
	auto& cb = m_cb10_Effect.Work();
	if (cb.effectNum >= 10) return; // 上限

	cb.effects[cb.effectNum].colorPos = pos;
	cb.effects[cb.effectNum].colorRadius = radius;
	cb.effects[cb.effectNum].colorColor = color;
	cb.effectNum++;
	cb.colorEnable = true;

	m_cb10_Effect.Write();
}

void UAEffectShaderManager::WriteCBColoerEnable(bool enable)
{
	//データをセット
	m_cb10_Effect.Work().colorEnable = enable;
	//GPUに転送
	m_cb10_Effect.Write();
}

void UAEffectShaderManager::WriteCBColoer(Math::Vector3 pos, float radius, Math::Vector3 color)
{
	AddEffect(pos, radius, color);
}

void UAEffectShaderManager::Release()
{
	m_cb10_Effect.Release();
}

