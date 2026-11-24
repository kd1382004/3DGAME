#pragma once
#include"../UIBase.h"

class PotionTexInfo;
class PlayerBase;

//バフの表示
class BuffUI :public UIBase
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	BuffUI() {};

	//デストラクタ
	~BuffUI() override{};

	//---- 基本の処理 ----

	//初期化
	void Init()override;

	//描画前の処理
	void PreDraw()override;

	//2Dの描画
	void DrawSprite()override;

	//---- 設定 ----

	//ポーションの画像一覧をセット
	// _spPotionTexInfo ... ポーションの画像一覧
	void SetPotionTexInfo(std::shared_ptr<PotionTexInfo> _spPotionTexInfo) { m_wpPotionTexInfo = _spPotionTexInfo; }

	//プレイヤーをセット
	// _spPlayer ... プレイヤー
	void SetPlayer(std::shared_ptr<PlayerBase>_spPlayer) { m_wpPlayer = _spPlayer; }

private:

	//---- 型 ----

	//表示中のバフのアイコン1つ分の情報
	struct BuffUIInfo {

		//バフ(ポーション)の番号
		int m_ID = 0;
		//残り時間の針の角度
		float m_angle = 0;
		//表示位置
		Math::Vector2 m_pos;

	};

	//---- メンバ変数 ----

	//ポーションの画像一覧
	std::weak_ptr<PotionTexInfo>m_wpPotionTexInfo;

	//プレイヤー
	std::weak_ptr<PlayerBase>m_wpPlayer;

	//アイコンの大きさ
	Math::Vector2 m_iconSize;

	//表示するバフの一覧
	std::list<BuffUIInfo> m_potionBuffUIInfoList;

	//残り時間の針の画像
	std::shared_ptr<KdTexture>m_spBuffTex;
};
