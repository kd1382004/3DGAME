#pragma once
#include"../UIBase.h"

class PotionTexInfo;
class PlayerBase;

class BuffUI :public UIBase
{
public:
	//コンストラクタ
	BuffUI() {};
	//デストラクタ
	~BuffUI() override{};

	//初期化
	void Init()override;

	//描画前の処理
	void PreDraw()override;
	//2Dの描画
	void DrawSprite()override;

	//ポーションの画像一覧をセット
	// _spPotionTexInfo ... ポーションの画像一覧
	void SetPotionTexInfo(std::shared_ptr<PotionTexInfo> _spPotionTexInfo) { m_wpPotionTexInfo = _spPotionTexInfo; }

	//プレイヤーをセット
	// _spPlayer ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayer) { m_wpPlayer = _spPlayer; }
private:
	//ポーションの画像一覧
	std::weak_ptr<PotionTexInfo>m_wpPotionTexInfo;
	//プレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayer;

	struct BuffUIInfo {

		//バフ(ポーション)の番号
		int m_ID;
		//残り時間の針の角度
		float m_angle = 0;
		//表示位置
		Math::Vector2 m_pos;

	};

	//アイコンの大きさ
	Math::Vector2 m_iconSiz;

	//表示するバフの一覧
	std::list<BuffUIInfo> m_potionBuffUIInfoList;

	//残り時間の針の画像
	std::shared_ptr<KdTexture>m_spBuffTex;
};
