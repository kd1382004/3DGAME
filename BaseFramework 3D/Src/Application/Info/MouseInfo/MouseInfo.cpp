#include"MouseInfo.h"
#include"../../main.h"

MouseInfo::~MouseInfo()
{

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

void MouseInfo::Init()
{
	// ↓ウィンドウ中央座標
	m_fixMousePos.x = 640;
	m_fixMousePos.y = 360;

	POINT pos;
	pos.x = m_fixMousePos.x;
	pos.y = m_fixMousePos.y;
	if (ClientToScreen(Application::Instance().GetWindowHandle(), &pos))
	{
		SetCursorPos(pos.x, pos.y);
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


	//************************************************

	//************************************************
	//デバック用のマウスフリー切り替え
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		m_mouseFreeFlg = true;
		m_mouseType = MouseType::MouseType_Nomal;
	}

	if (GetAsyncKeyState('E') & 0x8000)
	{
		m_mouseFreeFlg = false;
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

}