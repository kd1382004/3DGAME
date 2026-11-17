#include "ShopDisplay.h"
#include"../../../../Info/MouseInfo/MouseInfo.h"
#include"../../../../Info/KeyInfo/KeyInfo.h"
#include"../../../../Scene/SceneManager.h"

void ShopDisplay::Init()
{}

void ShopDisplay::Update()
{
	// 開いているタブでなければ何もしない
	if (!m_activeFlg) { return; }

}

void ShopDisplay::DrawSprite()
{
	// 開いているタブでなければ何もしない
	if (!m_activeFlg) { return; }

}

bool ShopDisplay::MouseHit(Math::Vector2 _pos, float _w, float _h)
{
	return false;
}
