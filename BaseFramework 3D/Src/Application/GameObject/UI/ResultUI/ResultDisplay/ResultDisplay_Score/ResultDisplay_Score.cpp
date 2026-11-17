#include "ResultDisplay_Score.h"
#include"../../../../../Info/NumDraw/NumDraw.h"
#include"../../../../../Scene/SceneManager.h"
void ResultDisplay_Score::Init()
{
	if (!m_backTex)
	{
		m_backTex = std::make_shared<KdTexture>();
		m_backTex->Load("Asset/Textures/GameUI/Result/ResultDisplay_Score/back.png");
	}


	m_OverallResultPos = { 0,250 };

	m_base2DPos = { -300,m_OverallResultPos.y - 200 };
	m_baseNumPos = m_base2DPos;
	m_baseNumPos.x += 600;

	if (!m_DisplayFloorTex)
	{
		m_DisplayFloorTex = std::make_shared<KdTexture>();
		m_DisplayFloorTex->Load("Asset/Textures/GameUI/Result/ResultDisplay_Score/floorReached.png");
	}
	if (!m_enemyKillTex)
	{
		m_enemyKillTex = std::make_shared<KdTexture>();
		m_enemyKillTex->Load("Asset/Textures/GameUI/Result/ResultDisplay_Score/EnemyKill.png");
	}
	if (!m_playTimeScTex)
	{
		m_playTimeScTex = std::make_shared<KdTexture>();
		m_playTimeScTex->Load("Asset/Textures/GameUI/Result/ResultDisplay_Score/Time.png");
	}
	if (!m_TimeColonTex)
	{
		m_TimeColonTex = std::make_shared<KdTexture>();
		m_TimeColonTex->Load("Asset/Textures/GameUI/Result/ResultDisplay_Score/TimeColon.png");
	}
	if (!m_TreasureChestsNumTex)
	{
		m_TreasureChestsNumTex = std::make_shared<KdTexture>();
		m_TreasureChestsNumTex->Load("Asset/Textures/GameUI/Result/ResultDisplay_Score/TreasureChestsNum.png");
	}

	if (!m_OverallResultTex)
	{
		m_OverallResultTex = std::make_shared<KdTexture>();
		m_OverallResultTex->Load("Asset/Textures/GameUI/Result/ResultDisplay_Score/OverallResult.png");
	}

	if (!m_HierarchyTex)
	{
		m_HierarchyTex = std::make_shared<KdTexture>();
		m_HierarchyTex->Load("Asset/Textures/GameUI/Result/ResultDisplay_Score/HierarchyTex.png");
	}
}

void ResultDisplay_Score::Update()
{

}

void ResultDisplay_Score::PreDraw()
{}

void ResultDisplay_Score::DrawSprite()
{
	// 総スコア（0番目）だけを描画する（階層ごとのスコアは出さない）
	if (m_displayFloorInfo.empty()) { return; }
	const DisplayFloorInfo& overall = m_displayFloorInfo[0];

	int floorNum = overall.DisplayFloor;
	int enemykill = overall.enemyKillCount;
	int TreasureChestsNum = overall.TreasureChestsNum;
	int timeSc = overall.playTimeSc;

	//タイトル（総合結果）
	if (m_OverallResultTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_OverallResultTex, m_OverallResultPos.x, m_OverallResultPos.y);
	}


	int Num = 0;
	Math::Vector2 pivo = { 0,0.5 };

	// 数字が超えてはいけない大きさ(項目の文字「最高階層」などの右端 〜 数字の右端)
	const Math::Vector2 kScoreNumMaxSiz = { 330.0f, 0.0f };

	if (m_DisplayFloorTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_DisplayFloorTex, m_base2DPos.x, m_base2DPos.y, nullptr, nullptr, pivo);
		NumDraw::GetInstance().Drow(floorNum, Aligned::RAligned, m_baseNumPos, kWhiteColor, 4, true, -1, kScoreNumMaxSiz);
		Num++;
	}

	if (m_enemyKillTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_enemyKillTex, m_base2DPos.x, m_base2DPos.y - Num * 100, nullptr, nullptr, pivo);
		NumDraw::GetInstance().Drow(enemykill, Aligned::RAligned, { m_baseNumPos.x,m_baseNumPos.y - Num * 100 }, kWhiteColor, 4, true, -1, kScoreNumMaxSiz);
		Num++;
	}

	if (m_TreasureChestsNumTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_TreasureChestsNumTex, m_base2DPos.x, m_base2DPos.y - Num * 100, nullptr, nullptr, pivo);
		NumDraw::GetInstance().Drow(TreasureChestsNum, Aligned::RAligned, { m_baseNumPos.x,m_baseNumPos.y - Num * 100 }, kWhiteColor, 4, true, -1, kScoreNumMaxSiz);
		Num++;
	}

	if (m_playTimeScTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_playTimeScTex, m_base2DPos.x, m_base2DPos.y - Num * 100, nullptr, nullptr, pivo);


		int sc = timeSc % 60;       // 秒
		int mine = (timeSc / 60) % 60; // 分
		int hour = timeSc / 3600;      // 時


		NumDraw::GetInstance().Drow(sc, Aligned::RAligned, { m_baseNumPos.x,m_baseNumPos.y - Num * 100 }, kWhiteColor, 4, true, 2);

		if (m_TimeColonTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_TimeColonTex, m_baseNumPos.x - 78, m_base2DPos.y - Num * 100);
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_TimeColonTex, m_baseNumPos.x - 158, m_base2DPos.y - Num * 100);
		}


		NumDraw::GetInstance().Drow(mine, Aligned::RAligned, { m_baseNumPos.x - 82,m_baseNumPos.y - Num * 100 }, kWhiteColor, 4, true, 2);
		NumDraw::GetInstance().Drow(hour, Aligned::RAligned, { m_baseNumPos.x - 162,m_baseNumPos.y - Num * 100 }, kWhiteColor, 4, true, 2);
		Num++;
	}

	for (int i = 0; i < Num; i++)
	{
		// 文字（左端）と数字（右端）がしっかり入るよう、左右に余白をとって広げる
		const float padX = 40.0f;
		float sizX = m_baseNumPos.x - m_base2DPos.x + padX * 2;
		float sizY = 100;
		Math::Color color = {};
		if (i % 2 == 0)
		{
			color = { 1,1,1,0.3f };
		}
		else
		{
			color = { 0.5,0.5,0.5,0.3f };
		}

		KdShaderManager::Instance().m_spriteShader.DrawTex(m_backTex, m_base2DPos.x - padX, m_base2DPos.y - i * 100, sizX, sizY, nullptr, &color, pivo);
	}

}

void ResultDisplay_Score::SetDisplayFloorInfo(std::vector<DisplayFloorInfo> _displayFloorInfo)
{
	m_displayFloorInfo = _displayFloorInfo;
}
