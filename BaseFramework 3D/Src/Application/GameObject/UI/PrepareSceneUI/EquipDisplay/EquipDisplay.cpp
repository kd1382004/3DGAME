#include "EquipDisplay.h"
#include"../../../../Info/MouseInfo/MouseInfo.h"
#include"../../../../Info/KeyInfo/KeyInfo.h"
#include"../../../../Scene/SceneManager.h"

void EquipDisplay::Init()
{}

void EquipDisplay::Update()
{
	// 開いているタブでなければ何もしない
	if (!m_activeFlg) { return; }

}

void EquipDisplay::DrawSprite()
{
	// 開いているタブでなければ何もしない
	if (!m_activeFlg) { return; }

}

bool EquipDisplay::MouseHit(Math::Vector2 _pos, float _w, float _h)
{
    return false;
}
