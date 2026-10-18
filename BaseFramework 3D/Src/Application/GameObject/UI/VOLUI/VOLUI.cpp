#include "VOLUI.h"
#include"../../../Info/MouseInfo/MouseInfo.h"
#include"../../../Info/KeyInfo/KeyInfo.h"

void VOLUI::Init()
{
	m_VolChangeOpenFlg = false;


	if (!m_volTex)
	{
		m_volTex = std::make_shared<KdTexture>();
		m_volTex->Load("Asset/Textures/GameUI/VOL/VOLIcon.png");
		m_volSiz.x = m_volTex->GetWidth();
		m_volSiz.y = m_volTex->GetHeight();
	}

	if (!m_volBackTex)
	{
		m_volBackTex = std::make_shared<KdTexture>();
		m_volBackTex->Load("Asset/Textures/GameUI/VOL/VOLBack.png");
		m_volBackSiz.x = m_volBackTex->GetWidth();
		m_volBackSiz.y = m_volBackTex->GetHeight();
	}


	for (int i = 0;i < m_volInfoSiz;i++)
	{
		m_volInfo[i].m_volTex = std::make_shared<KdTexture>();
		m_volInfo[i].m_volValTex = std::make_shared<KdTexture>();

		switch (i)
		{
		case 0:
			m_volInfo[i].m_volTex->Load("Asset/Textures/GameUI/VOL/MAIcon.png");
			m_volInfo[i].vol = VOLInfo::Instance().GetMSVol();
			break;
		case 1:
			m_volInfo[i].m_volTex->Load("Asset/Textures/GameUI/VOL/BGMIcon.png");
			m_volInfo[i].vol = VOLInfo::Instance().GetBGMVol();
			break;
		case 2:
			m_volInfo[i].m_volTex->Load("Asset/Textures/GameUI/VOL/SEIcon.png");
			m_volInfo[i].vol = VOLInfo::Instance().GetSEVol();
			break;
		default:
			break;
		}

		m_volInfo[i].m_volValTex->Load("Asset/Textures/GameUI/VOL/VOLValue.png");



		m_volInfo[i].m_volSiz.x = m_volInfo[i].m_volTex->GetWidth();
		m_volInfo[i].m_volSiz.y = m_volInfo[i].m_volTex->GetHeight();

		m_volInfo[i].m_volVaSiz.x = m_volInfo[i].m_volValTex->GetWidth();
		m_volInfo[i].m_volVaSiz.y = m_volInfo[i].m_volValTex->GetHeight();


	}
}

void VOLUI::Update()
{
	POINT mouse = MouseInfo::Instance().m_windowPos;

	{
		float left = m_volPos.x - m_volSiz.x / 2;
		float rightt = m_volPos.x + m_volSiz.x / 2;
		float top = m_volPos.y + m_volSiz.y/2;
		float bot = m_volPos.y - m_volSiz.y / 2;

		if (left <= mouse.x && rightt >= mouse.x &&
			top >= mouse.y && bot <= mouse.y)
		{
			MouseInfo::Instance().SetMouseType(MouseInfo::MouseType_HIT);
			if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON,true))
			{
				m_VolChangeOpenFlg = !m_VolChangeOpenFlg;
			}
		}
	}

	if (!m_VolChangeOpenFlg) { return; }

	{
		float left = m_volBackPos.x - m_volBackSiz.x / 2;
		float rightt = m_volBackPos.x + m_volBackSiz.x / 2;
		float top = m_volBackPos.y + m_volBackSiz.y;
		float bot = m_volBackPos.y;

		if (left <= mouse.x && rightt >= mouse.x &&
			top >= mouse.y && bot <= mouse.y)
		{

		}
		else
		{
			if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON, true))
			{
				m_VolChangeOpenFlg = !m_VolChangeOpenFlg;
				return;
			}
		}
	}






	for (int i = 0;i < m_volInfoSiz;i++)
	{
		float left = m_volInfo[i].m_volValPos.x - m_volInfo[i].m_volVaSiz.x / 2;
		float rightt = m_volInfo[i].m_volValPos.x + m_volInfo[i].m_volVaSiz.x / 2;
		float top = m_volInfo[i].m_volValPos.y + m_volInfo[i].m_volVaSiz.y + 5;
		float bot = m_volInfo[i].m_volValPos.y - 5;

		if (left <= mouse.x && rightt >= mouse.x &&
			top >= mouse.y && bot <= mouse.y)
		{
			MouseInfo::Instance().SetMouseType(MouseInfo::MouseType_HIT);
			if (KeyInfo::Instance().GetValidKeyPush(VK_LBUTTON))
			{
				float sizY = top - bot;        // 全体の高さ
				float mouseY = top - mouse.y;  // 上からの距離

				float fromBottom = sizY - mouseY;  // 下からの距離
				float percent = fromBottom / sizY; // 下が0、上が1

				percent = std::clamp(percent, 0.0f, 1.0f);
				m_volInfo[i].vol = percent;
			}
		}


		switch (i)
		{
		case 0:
			VOLInfo::Instance().SetMSVol(m_volInfo[i].vol);
			break;
		case 1:
			VOLInfo::Instance().SetBGMVol(m_volInfo[i].vol);
			break;
		case 2:
			VOLInfo::Instance().SetSEVol(m_volInfo[i].vol);
			break;
		default:
			break;
		}
	}
}

void VOLUI::DrawSprite()
{


	if (m_volTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_volTex, m_volPos.x, m_volPos.y);
	}

	if (!m_VolChangeOpenFlg) { return; }

	if (m_volBackTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_volBackTex, m_volBackPos.x, m_volBackPos.y, nullptr, nullptr, { 0.5,0 });
	}

	for (int i = 0;i < m_volInfoSiz;i++)
	{
		if (!m_volInfo[i].m_volTex) { continue; }
		if (!m_volInfo[i].m_volValTex) { continue; }

		KdShaderManager::Instance().m_spriteShader.DrawTex(m_volInfo[i].m_volTex, m_volInfo[i].m_volPos.x, m_volInfo[i].m_volPos.y);
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_volInfo[i].m_volValTex, m_volInfo[i].m_volValPos.x, m_volInfo[i].m_volValPos.y, nullptr, nullptr, { 0.5,0 });


		Math::Color color = { 0,0.7,0,1 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_volInfo[i].m_volValTex, m_volInfo[i].m_volValPos.x, m_volInfo[i].m_volValPos.y, m_volInfo[i].m_volVaSiz.x, m_volInfo[i].m_volVaSiz.y * m_volInfo[i].vol, nullptr, &color, { 0.5,0 });
	}
}

void VOLUI::SetVolPos(Math::Vector2 _pos)
{
	m_volPos = _pos;

	m_volBackPos = _pos;
	m_volBackPos.y += m_volSiz.y / 2;


	Math::Vector2 volInfobasePos = m_volBackPos;
	volInfobasePos.y += m_volSiz.y + 20;
	volInfobasePos.x -= (m_volBackSiz.x / m_volInfoSiz);


	float siz = m_volBackSiz.x / 3;
	for (int i = 0;i < m_volInfoSiz;i++)
	{
		m_volInfo[i].m_volPos = volInfobasePos;
		m_volInfo[i].m_volValPos = m_volInfo[i].m_volPos;
		m_volInfo[i].m_volValPos.y += m_volInfo[i].m_volSiz.y / 2;


		volInfobasePos.x += siz;
	}
}
