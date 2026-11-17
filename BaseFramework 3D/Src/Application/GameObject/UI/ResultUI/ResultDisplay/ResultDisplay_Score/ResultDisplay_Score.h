#pragma once
#include"../ResultDisplayBase.h"

#include"../../../../../Scene/SceneManager.h" 

class ResultDisplay_Score: public ResultDisplayBase
{
public:
	//コンストラクタ
	ResultDisplay_Score() {}
	//デストラクタ
	~ResultDisplay_Score() {}

	//初期化
	void Init()override;

	//更新
	void Update()override;

	//描画前の処理
	void PreDraw()override;
	//2Dの描画
	void DrawSprite()override;

	//各階の結果をセット
	// _displayFloorInfo ... 各階の結果
	void SetDisplayFloorInfo(std::vector<DisplayFloorInfo>_displayFloorInfo);

private:

	//基準の位置
	Math::Vector2 m_base2DPos;
	//数字の基準の位置
	Math::Vector2 m_baseNumPos;


	//背景の画像
	std::shared_ptr<KdTexture>m_spBackTex;

	////////////////////////////////////////////////

	//各階の結果
	std::vector<DisplayFloorInfo>m_displayFloorInfo;

	//「総合結果」の画像
	std::shared_ptr<KdTexture>m_spOverallResultTex;
	//「階層」の画像
	std::shared_ptr<KdTexture>m_spHierarchyTex;
	//到達階の画像
	std::shared_ptr<KdTexture>m_spDisplayFloorTex;
	//倒した敵の数の画像
	std::shared_ptr<KdTexture>m_spEnemyKillTex;
	//プレイ時間の画像
	std::shared_ptr<KdTexture>m_spPlayTimeScTex;
	//時間の「:」の画像
	std::shared_ptr<KdTexture>m_spTimeColonTex;
	//開けた宝箱の数の画像
	std::shared_ptr<KdTexture>m_spTreasureChestsNumTex;

	//「総合結果」の位置
	Math::Vector2 m_overallResultPos;

	////////////////////////////////////////////////////////

};

