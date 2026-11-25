#include "FloorTitleUI.h"
#include"../../../Info/DeltaTime/DeltaTime.h"
#include"../../../Info/NumDraw/NumDraw.h"
#include"../../Terrains/Map/MapManager.h"
void FloorTitleUI::Init()
{
	//黒い板(画面全体を覆う)
	if (!m_spBlackTex)
	{
		m_spBlackTex = std::make_shared<KdTexture>();
		m_spBlackTex->Load("Asset/Textures/FadeDraw/FadeDraw.png");
	}

	//「階層」の文字
	if (!m_spFloorTex)
	{
		m_spFloorTex = std::make_shared<KdTexture>();
		m_spFloorTex->Load("Asset/Textures/GameUI/Result/ResultDisplay_Score/HierarchyTex.png");
	}

	//マップ名(MapType の数だけ、名前からパスを作る)
	if (m_spMapNameTexList.empty())
	{
		for (int i = 0; i < Maptype_Siz; ++i)
		{
			std::shared_ptr<KdTexture> spTex = nullptr;

			const char* name = GetMapTypeName(static_cast<MapType>(i));
			if (name)
			{
				spTex = std::make_shared<KdTexture>();
				spTex->Load(std::string("Asset/Textures/GameUI/FloorTitle/MapName_") + name + ".png");
			}

			//名前が無くても1つ入れて、番号と MapType をずらさない
			m_spMapNameTexList.push_back(spTex);
		}
	}

	//最初は表示しない
	m_active = false;
	m_floor = 0;
	m_timer = 0.0f;
	m_alpha = 0.0f;

	m_layerPriority = -1;
}

void FloorTitleUI::Update()
{
	if (!m_active) { return; }
	m_timer += DeltaTime::Instance().GetRealDeltaTime();

	//最初の kTextFade 秒でふわっと出て、最後の kTextFade 秒でふわっと消える
	float in = m_timer / kTextFade;
	float out = (kShowTime - m_timer) / kTextFade;
	m_alpha = std::clamp(std::min(in, out), 0.0f, 1.0f);
}

void FloorTitleUI::DrawSprite()
{
	if (!m_active) { return; }
	Math::Color col = { 0,0,0,1 };
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spBlackTex, 0, 0, 1280, 720, nullptr, &col);



	//階層の表示
	Math::Color numCol = { 1,1,1,m_alpha };

	//マップ名(階層の上)
	if (m_mapType >= 0 && m_mapType < static_cast<int>(m_spMapNameTexList.size()))
	{
		const std::shared_ptr<KdTexture>& spNameTex = m_spMapNameTexList[m_mapType];
		if (spNameTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(spNameTex, 0, static_cast<int>(kMapNamePosY), nullptr, &numCol);
		}
	}
	NumDraw::GetInstance().Draw(m_floor, RAligned, { -10, 0 }, numCol, 7.0f);
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spFloorTex, -10, 0, nullptr, &numCol, { 0,0.5 });
}
