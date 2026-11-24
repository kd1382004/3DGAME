//---- 自分のヘッダー ----
#include "DrawTex.h"							//画像を1枚描くUI

//---- 便利機能 ----
#include"../../../Info/KeyInfo/KeyInfo.h"		//キー入力

//=============================================
// 基本の処理
//=============================================

void DrawTex::Update()
{

}

void DrawTex::DrawSprite()
{
	if (m_spTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTex, m_2DPos.x, m_2DPos.y);
	}
}

//=============================================
// その他の処理
//=============================================

void DrawTex::LoadTex(std::string _path)
{
	if (!m_spTex) { m_spTex = std::make_shared<KdTexture>(); }
	m_spTex->Load(_path);
}
