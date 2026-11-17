#include "RecordsDisplay.h"
#include"../../../../Info/MouseInfo/MouseInfo.h"
#include"../../../../Info/KeyInfo/KeyInfo.h"
#include"../../../../Scene/SceneManager.h"

void RecordsDisplay::Init()
{}

void RecordsDisplay::Update()
{
	// 開いているタブでなければ何もしない
	if (!m_activeFlg) { return; }

}

void RecordsDisplay::DrawSprite()
{
	// 開いているタブでなければ何もしない
	if (!m_activeFlg) { return; }

}

bool RecordsDisplay::MouseHit(Math::Vector2 _pos, float _w, float _h)
{
    return false;
}
