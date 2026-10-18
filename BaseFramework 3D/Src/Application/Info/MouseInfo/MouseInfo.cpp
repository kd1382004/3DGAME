#include"MouseInfo.h"
#include"../../main.h"
#include"../KeyInfo/KeyInfo.h"

MouseInfo::~MouseInfo()
{

}

void MouseInfo::SetMouseType(MouseType _type)
{

	if (m_mouseType != MouseType::MouseType_HIT&& m_mouseOldType!= MouseType::MouseType_HIT)
	{
		if (m_spHitSE)
		{
			m_spHitSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
			m_spHitSE->Play();
		}
	}


	m_mouseType = _type;
}

void MouseInfo::SetMousePosFixMousePos()
{
	if (!m_FocusWindowFlg)
	{
		return;
	}

	POINT pos;
	pos.x = m_fixMousePos.x;
	pos.y = m_fixMousePos.y;
	if (ClientToScreen(Application::Instance().GetWindowHandle(), &pos))
	{
		SetCursorPos(pos.x, pos.y);
	}
}

POINT MouseInfo::GetWindouMousePos()
{

	POINT pos;
	pos.x = m_fixMousePos.x;
	pos.y = m_fixMousePos.y;
	ClientToScreen(Application::Instance().GetWindowHandle(), &pos);

	return pos;
}

void MouseInfo::SEMouseClickPlay()
{
	if (m_spClickSE)
	{
		m_spClickSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
		m_spClickSE->Play();
	}
}

void MouseInfo::Init()
{
	// ↓ウィンドウ中央座標
	m_fixMousePos.x = 640;
	m_fixMousePos.y = 360;


	//マウスをウィンドウの中央に設定
	POINT pos;
	pos.x = m_fixMousePos.x;
	pos.y = m_fixMousePos.y;
	if (ClientToScreen(Application::Instance().GetWindowHandle(), &pos))
	{
		SetCursorPos(pos.x, pos.y);
	}


	if (!m_mousePointer)
	{
		m_mousePointer = std::make_shared<KdTexture>();
		m_mousePointer->Load("Asset/Textures/Mouse/MousePointer.png");
	}

	if (!m_spClickSE)
	{
		m_spClickSE = KdAudioManager::Instance().Play("Asset/Sound/SE/Mouse/click.wav", false);
		m_spClickSE->Stop();
		m_spClickSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
	}
	if (!m_spHitSE)
	{
		m_spHitSE = KdAudioManager::Instance().Play("Asset/Sound/SE/Mouse/Hit.wav", false);
		m_spHitSE->Stop();
		m_spHitSE->SetVolume(VOLInfo::Instance().GetMSSEVol());
	}
}

void MouseInfo::Update()
{
	//************************************************
	//ウィンドウ単位でのフォーカスがあるかどうか
	m_FocusWindowFlg = (GetForegroundWindow() == Application::Instance().GetWindowHandle());

	POINT mousePos;
	GetCursorPos(&mousePos);

	//指定のウィンドウ基準のマウス座標に変換(実行画面の左上が(0,0))
	ScreenToClient(Application::Instance().GetWindowHandle(), &mousePos);


	if (mousePos.x > -m_fixMousePos.x && mousePos.x < m_fixMousePos.x &&
		mousePos.y > -m_fixMousePos.y && mousePos.y < m_fixMousePos.y)
	{
		if (!m_FocusWindowFlg)
		{
			return;
		}

	}
	ShowCursor(false);

	//************************************************

	//************************************************
	//デバック用のマウスフリー切り替え


	KeyInfo::Instance().SetKeyValid('Q');
	KeyInfo::Instance().SetKeyValid('E');
	if (KeyInfo::Instance().GetValidKeyPush('Q'))
	{
		SetMouseFreeFlg(true);
	}

	if (KeyInfo::Instance().GetValidKeyPush('E'))
	{
		SetMouseFreeFlg(false);
	}


	m_mouseOldType = m_mouseType;

	if (m_mouseFreeFlg)
	{
		m_mouseType = MouseType::MouseType_Nomal;

	}
	else
	{
		m_mouseType = MouseType::MouseType_NotFree;
	}

	//************************************************



	//************************************************
	//マウスがフリーならウィンドウ基準のマウス座標に変換

	if (m_mouseFreeFlg)
	{
		//ディスプレイ状のマウス座標を取得(pc画面の左上が(0,0))
		GetCursorPos(&m_windowPos);

		//指定のウィンドウ基準のマウス座標に変換(実行画面の左上が(0,0))
		ScreenToClient(Application::Instance().GetWindowHandle(), &m_windowPos);


		//マウスの座標系を実行ウィンドウの座標(中心が(0,0)に補正)
		m_windowPos.x -= m_fixMousePos.x;
		m_windowPos.y -= m_fixMousePos.y;
		m_windowPos.y *= -1;
	}
	//************************************************
}

void MouseInfo::DrawSprite()
{
	if (!m_mousePointer) { return; }
	Math::Rectangle rec;
	switch (m_mouseType)
	{
	case MouseInfo::MouseType_Nomal:
		rec = { 0,0,(long)m_mousePointer->GetWidth() / 2,(long)m_mousePointer->GetHeight() };
		break;
	case MouseInfo::MouseType_HIT:
		rec = { (long)m_mousePointer->GetWidth() / 2,0,(long)m_mousePointer->GetWidth() / 2,(long)m_mousePointer->GetHeight() };
		break;
	case MouseInfo::MouseType_Click:
		break;
	case MouseInfo::MouseType_NotFree:
		return;
		break;
	default:
		break;
	}

	Math::Vector2 piv = { 0,1 };
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_mousePointer, m_windowPos.x, m_windowPos.y, &rec, nullptr, piv);

}