#include "FloorNumUI.h"
#include"../../../Info/NumDraw/NumDraw.h"
#include"../../../Scene/GameScene/GameScene.h"

void FloorNumUI::Init()
{
	//「階層」の文字
	if (!m_spFloorTex)
	{
		m_spFloorTex = std::make_shared<KdTexture>();
		m_spFloorTex->Load("Asset/Textures/GameUI/Map/FloorTex_Trim.png");
	}

	//下線
	if (!m_spUnderlineTex)
	{
		m_spUnderlineTex = std::make_shared<KdTexture>();
		m_spUnderlineTex->Load("Asset/Textures/GameUI/Map/FloorUnderline.png");
	}

	//ミニマップ(2)より上、階の切り替えの黒い画面(-1)より下
	m_layerPriority = 1;
}

void FloorNumUI::DrawSprite()
{
	std::shared_ptr<GameScene> spGameScene = m_wpGameScene.lock();
	if (!spGameScene) { return; }

	int floor = spGameScene->GetDisplayFloor();

	//数字1文字の大きさ(画面上)
	float charW = NumDraw::GetInstance().GetCharWidth() * kNumSize;
	float charH = NumDraw::GetInstance().GetCharHeight() * kNumSize;

	//数字(左端を kPos にそろえる)
	NumDraw::GetInstance().Draw(floor, LAligned, kPos, { 1,1,1,1 }, kNumSize);

	int digit = static_cast<int>(std::to_string(floor).size());

	//「階層」の文字(数字の右に、数字と同じ高さで)
	int texW = 0;
	if (m_spFloorTex)
	{
		float scale = charH / m_spFloorTex->GetHeight();
		texW = static_cast<int>(m_spFloorTex->GetWidth() * scale);
		int h = static_cast<int>(m_spFloorTex->GetHeight() * scale);
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spFloorTex, static_cast<int>(kPos.x + digit * charW + kTextGap), static_cast<int>(kPos.y), texW, h, nullptr, &kWhiteColor, { 0, 0.5f });
	}

	//---- 下線 ----
	if (m_spUnderlineTex)
	{
		//左端 = 数字の左端(kPos.x)から少し左。桁数に関係なく固定
		float left = kPos.x - kLineExtend;

		//右端 = 「階」の文字の右端から少し右
		float right = kPos.x + digit * charW + kTextGap + texW + kLineExtend;

		//高さ = 文字の下端から少し下(画面は上がプラス)
		float lineY = kPos.y - charH * 0.5f - kLineGap;

		//文字の端から端までの長さに引き伸ばして、左端をそろえて描く
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spUnderlineTex,
			static_cast<int>(left), static_cast<int>(lineY),
			static_cast<int>(right - left), m_spUnderlineTex->GetHeight(),
			nullptr, &kWhiteColor, { 0, 0.5f });
	}
}
