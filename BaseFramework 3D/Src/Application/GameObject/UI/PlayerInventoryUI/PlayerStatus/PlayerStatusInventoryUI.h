#pragma once
#include"../InventoryUIContext.h"

//////////////////////////////////////////////////////////////
// インベントリ：プレイヤーステータス画面
//////////////////////////////////////////////////////////////
class PlayerStatusInventoryUI :public InventoryTabBase
{
public:

	//---- コンストラクタ・デストラクタ ----

	//コンストラクタ
	PlayerStatusInventoryUI() {}

	//デストラクタ
	~PlayerStatusInventoryUI()override {}

	//---- 基本の処理 ----

	//初期化
	void Init()override;
	void Update()override;			// = 旧 PlayerStatusUpdate
	void PreDraw()override;			// = 旧 PlayerStatusPreDraw
	void DrawSprite()override;		// = 旧 PlayerStatusDraw

private:

	//---- 型 ----

	//ステータス画面に並べる項目1つ分(数値・名前・アイコン)の表示情報
	struct PlayerStatusInfo
	{
		//HPや攻撃力などの数値
		int m_num = 0;

		//数値の位置
		Math::Vector2 m_numPos;

		//数値の名前
		std::shared_ptr<KdTexture>m_spNameTex;

		//数値の名前の位置
		Math::Vector2 m_namePos;

		//アイコン
		std::shared_ptr<KdTexture>m_spIconTex;

		//アイコンの位置
		Math::Vector2 m_iconPos;
	};

	//ステータス画面の項目の番号(PlayerStatusInfoSize は項目の数)
	enum PlayerStatusInfoID
	{
		PlayerStatusInfo_HP,
		PlayerStatusInfo_MP,
		PlayerStatusInfo_AttackPower,
		PlayerStatusInfo_DefensePower,
		PlayerStatusInfo_Speed,
		PlayerStatusInfoSize
	};

	//---- メンバ変数 ----

	//プレイヤーを映すレンダーターゲット
	std::shared_ptr<KdRenderTargetPack> m_spRtTargetPack;

	//プレイヤーを映すカメラ
	std::shared_ptr<KdCamera> m_spStatusCamera;
	float m_statusCamAngleY = 0.0f;   // Y軸回転角度 (度)
	float m_statusCamDistance = 4.0f;   // プレイヤーからの距離 (お好みに合わせて調整)
	float m_statusCamPitch = 8.0f;   // 少し見下ろす角度 (度)
	float m_statusCamHeightOffset = 2;   // プレイヤーの注視点(胸・腰の高さ)
	float m_statusCamRotateSpeed = 45.0f;  // 回転速度 (度/秒: 45度なら約8秒で1周)

	//ステータスの表示情報
	PlayerStatusInfo m_playerStatusInfo[PlayerStatusInfoSize];

	//ステータスの背景の画像
	std::shared_ptr<KdTexture>m_spPlayerStatusInfoBackTex;
};
