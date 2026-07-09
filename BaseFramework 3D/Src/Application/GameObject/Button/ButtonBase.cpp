#include "ButtonBase.h"

#include"../../Info/MouseInfo/MouseInfo.h"

#include"../../Info/DebugInfo/DebugInfo.h"

void ButtonBase::Init()
{
	m_gameObjectClass = KdGameObject::GameObjectClass::GameObjectClass_Botton;
	m_choseFlg = false;
}

void ButtonBase::Update()
{

	/////////////////////////////////////////////////////////
	//マウスで座標変換(エディター用)
	if (DebugInfo::Instance().GetSceneManagerImGUIFlg())
	{
		float Left = m_pos.x - (m_diameter.x / 2.0f) * m_scale;
		float Right = m_pos.x + (m_diameter.x / 2.0f) * m_scale;
		float Top = m_pos.y - (m_diameter.y / 2.0f) * m_scale;
		float Bottom = m_pos.y + (m_diameter.y / 2.0f) * m_scale;

		float mx = MouseInfo::Instance().m_windowPos.x;
		float my = MouseInfo::Instance().m_windowPos.y;

		if (Left <= mx && mx <= Right &&
			Top <= my && my <= Bottom)
		{
			MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
			m_editorChoseFlg = true;
		}

		if (m_editorChoseFlg)
		{
			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
			{
				m_pos = { mx,my };
			}
			else
			{
				m_editorChoseFlg = false;
			}
		}
	}
	/////////////////////////////////////////////////////////
}

void ButtonBase::DrawSprite()
{
	Math::Color color = { 1,1,1,m_alpha };
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_tex, m_pos.x, m_pos.y, m_diameter.x * m_scale, m_diameter.y * m_scale, nullptr, &color);
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

void ButtonBase::MouseChosen()
{

	float Left = m_pos.x - (m_diameter.x / 2.0f) * m_scale;
	float Right = m_pos.x + (m_diameter.x / 2.0f) * m_scale;
	float Top = m_pos.y - (m_diameter.y / 2.0f) * m_scale;
	float Bottom = m_pos.y + (m_diameter.y / 2.0f) * m_scale;

	float mx = MouseInfo::Instance().m_windowPos.x;
	float my = MouseInfo::Instance().m_windowPos.y;

	if (Left <= mx && mx <= Right &&
		Top <= my && my <= Bottom)
	{
		MouseInfo::Instance().SetMouseType(MouseInfo::MouseType::MouseType_HIT);
		m_choseFlg = true;
	}
	else
	{
		m_choseFlg = false;
	}

}
