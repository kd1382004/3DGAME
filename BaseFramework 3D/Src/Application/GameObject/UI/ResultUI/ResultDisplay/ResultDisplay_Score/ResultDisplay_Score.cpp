#include "ResultDisplay_Score.h"
#include"../../../../../Info/NumDraw/NumDraw.h"
#include"../../../../../Scene/SceneManager.h"
void ResultDisplay_Score::Init()
{
	if (!m_spBackTex)
	{
		m_spBackTex = std::make_shared<KdTexture>();
		m_spBackTex->Load("Asset/Textures/GameUI/Result/ResultDisplay_Score/back.png");
	}


	m_overallResultPos = { 0,250 };

	m_base2DPos = { -300,m_overallResultPos.y - 200 };
	m_baseNumPos = m_base2DPos;
	m_baseNumPos.x += 600;

	if (!m_spDisplayFloorTex)
	{
		m_spDisplayFloorTex = std::make_shared<KdTexture>();
		m_spDisplayFloorTex->Load("Asset/Textures/GameUI/Result/ResultDisplay_Score/floorReached.png");
	}
	if (!m_spEnemyKillTex)
	{
		m_spEnemyKillTex = std::make_shared<KdTexture>();
		m_spEnemyKillTex->Load("Asset/Textures/GameUI/Result/ResultDisplay_Score/EnemyKill.png");
	}
	if (!m_spPlayTimeScTex)
	{
		m_spPlayTimeScTex = std::make_shared<KdTexture>();
		m_spPlayTimeScTex->Load("Asset/Textures/GameUI/Result/ResultDisplay_Score/Time.png");
	}
	if (!m_spTimeColonTex)
	{
		m_spTimeColonTex = std::make_shared<KdTexture>();
		m_spTimeColonTex->Load("Asset/Textures/GameUI/Result/ResultDisplay_Score/TimeColon.png");
	}
	if (!m_spTreasureChestsNumTex)
	{
		m_spTreasureChestsNumTex = std::make_shared<KdTexture>();
		m_spTreasureChestsNumTex->Load("Asset/Textures/GameUI/Result/ResultDisplay_Score/TreasureChestsNum.png");
	}

	if (!m_spOverallResultTex)
	{
		m_spOverallResultTex = std::make_shared<KdTexture>();
		m_spOverallResultTex->Load("Asset/Textures/GameUI/Result/ResultDisplay_Score/OverallResult.png");
	}

	if (!m_spHierarchyTex)
	{
		m_spHierarchyTex = std::make_shared<KdTexture>();
		m_spHierarchyTex->Load("Asset/Textures/GameUI/Result/ResultDisplay_Score/HierarchyTex.png");
	}
}

void ResultDisplay_Score::Update()
{

}

void ResultDisplay_Score::PreDraw()
{}

void ResultDisplay_Score::DrawSprite()
{
	// 総スコアを描画する(結果がまだ入っていなければ何もしない)
	if (m_displayFloorInfo.m_displayFloor <= 0) { return; }
	const DisplayFloorInfo& overall = m_displayFloorInfo;

	int floorNum = overall.m_displayFloor;
	int enemykill = overall.m_enemyKillCount;
	int treasureChestsNum = overall.m_treasureChestsNum;
	int timeSc = overall.m_playTimeSc;

	//タイトル（総合結果）
	if (m_spOverallResultTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spOverallResultTex, m_overallResultPos.x, m_overallResultPos.y);
	}


	int num = 0;
	Math::Vector2 pivot = { 0,0.5 };

	// 数字が超えてはいけない大きさ(項目の文字「最高階層」などの右端 〜 数字の右端)
	const Math::Vector2 kScoreNumMaxSiz = { 330.0f, 0.0f };

	if (m_spDisplayFloorTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spDisplayFloorTex, m_base2DPos.x, m_base2DPos.y, nullptr, nullptr, pivot);
		NumDraw::GetInstance().Draw(floorNum, Aligned::RAligned, m_baseNumPos, kWhiteColor, 4, true, -1, kScoreNumMaxSiz);
		num++;
	}

	if (m_spEnemyKillTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spEnemyKillTex, m_base2DPos.x, m_base2DPos.y - num * 100, nullptr, nullptr, pivot);
		NumDraw::GetInstance().Draw(enemykill, Aligned::RAligned, { m_baseNumPos.x,m_baseNumPos.y - num * 100 }, kWhiteColor, 4, true, -1, kScoreNumMaxSiz);
		num++;
	}

	if (m_spTreasureChestsNumTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTreasureChestsNumTex, m_base2DPos.x, m_base2DPos.y - num * 100, nullptr, nullptr, pivot);
		NumDraw::GetInstance().Draw(treasureChestsNum, Aligned::RAligned, { m_baseNumPos.x,m_baseNumPos.y - num * 100 }, kWhiteColor, 4, true, -1, kScoreNumMaxSiz);
		num++;
	}

	if (m_spPlayTimeScTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spPlayTimeScTex, m_base2DPos.x, m_base2DPos.y - num * 100, nullptr, nullptr, pivot);


		int sc = timeSc % 60;       // 秒
		int mine = (timeSc / 60) % 60; // 分
		int hour = timeSc / 3600;      // 時


		NumDraw::GetInstance().Draw(sc, Aligned::RAligned, { m_baseNumPos.x,m_baseNumPos.y - num * 100 }, kWhiteColor, 4, true, 2);

		if (m_spTimeColonTex)
		{
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTimeColonTex, m_baseNumPos.x - 78, m_base2DPos.y - num * 100);
			KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTimeColonTex, m_baseNumPos.x - 158, m_base2DPos.y - num * 100);
		}


		NumDraw::GetInstance().Draw(mine, Aligned::RAligned, { m_baseNumPos.x - 82,m_baseNumPos.y - num * 100 }, kWhiteColor, 4, true, 2);
		NumDraw::GetInstance().Draw(hour, Aligned::RAligned, { m_baseNumPos.x - 162,m_baseNumPos.y - num * 100 }, kWhiteColor, 4, true, 2);
		num++;
	}

	for (int i = 0; i < num; i++)
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

		KdShaderManager::Instance().m_spriteShader.DrawTex(m_spBackTex, m_base2DPos.x - padX, m_base2DPos.y - i * 100, sizX, sizY, nullptr, &color, pivot);
	}

}

void ResultDisplay_Score::SetDisplayFloorInfo(const DisplayFloorInfo& _displayFloorInfo)
{
	m_displayFloorInfo = _displayFloorInfo;
}
