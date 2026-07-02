#include "ButtonBase.h"

void ButtonBase::Init()
{
	m_gameObjectClass = KdGameObject::GameObjectClass::GameObjectClass_Botton;
}

void ButtonBase::Update()
{}

void ButtonBase::DrawSprite()
{
	Math::Color color = { 1,1,1,m_alpha };
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_tex, m_pos.x, m_pos.y, m_radius.x * m_scale, m_radius.y * m_scale, nullptr, &color);
}

void ButtonBase::SetPos(const Math::Vector3& pos)
{
	m_pos.x = pos.x;
	m_pos.y = pos.y;
}

void ButtonBase::KeyChosen(bool _choseFlg)
{
	if (_choseFlg)
	{
		m_scale = m_defScale * m_hitScale;
	}
	else
	{
		m_scale = m_defScale;
	}

	m_choseFlg = _choseFlg;
}

void ButtonBase::AlphaFlashing()
{
	m_alpha += m_alphaAdd;
	if (m_alpha < m_alphaMine)
	{
		m_alpha = m_alphaMine;
		m_alphaAdd *= -1;
	}

	if (m_alpha > m_alphaMax)
	{
		m_alpha = m_alphaMax;
		m_alphaAdd *= -1;
	}

}

void ButtonBase::ImGUI()
{

	if (ImGui::TreeNode(U8("座標")))
	{
		ImGui::InputFloat(U8("X座標"), &m_pos.x, 0.1f, 1.0f, "%.2f");
		ImGui::InputFloat(U8("Y座標"), &m_pos.y, 0.1f, 1.0f, "%.2f");
		ImGui::TreePop();
	}

	if (ImGui::TreeNode(U8("サイズ")))
	{
		ImGui::InputFloat(U8("デフォルト状態"), &m_defScale, 0.1f, 1.0f, "%.2f");
		ImGui::InputFloat(U8("選択"), &m_hitScale, 0.1f, 1.0f, "%.2f");


		ImGui::Checkbox(U8("選択状態"), &m_choseFlg);

		if (m_choseFlg)
		{
			m_scale = m_defScale * m_hitScale;
		}
		else
		{
			m_scale = m_defScale;
		}


		ImGui::TreePop();
	}

}
