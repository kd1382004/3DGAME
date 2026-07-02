#include"MouseInfo.h"

MouseInfo::~MouseInfo()
{

}

void MouseInfo::Init()
{ 
	// ↓画面中央座標
	m_fixMousePos.x = 640;
	m_fixMousePos.y = 360;
	SetCursorPos(m_fixMousePos.x, m_fixMousePos.y);
}

void MouseInfo::Update()
{ 
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		m_mouseFreeFlg = true;
	}
	
	if (GetAsyncKeyState('E') & 0x8000)
	{
		m_mouseFreeFlg = false;
	}
}

void MouseInfo::DrawSprite()
{

}