#pragma once
#include"../InventoryUIContext.h"

//////////////////////////////////////////////////////////////
// インベントリ：プレイヤーステータス画面
//////////////////////////////////////////////////////////////
class PlayerStatusInventoryUI :public InventoryTabBase
{
public:
	PlayerStatusInventoryUI() {}
	~PlayerStatusInventoryUI()override {}

	void Init()override;
	void Update()override;			// = 旧 PlayerStatusUpdate
	void PreDraw()override;			// = 旧 PlayerStatusPreDraw
	void DrawSprite()override;		// = 旧 PlayerStatusDraw

private:
	std::shared_ptr<KdRenderTargetPack> m_spRtTargetPack;

	std::shared_ptr<KdCamera> m_spStatusCamera;
	float m_statusCamAngleY = 0.0f;   // Y軸回転角度 (度)
	float m_statusCamDistance = 4.0f;   // プレイヤーからの距離 (お好みに合わせて調整)
	float m_statusCamPitch = 8.0f;   // 少し見下ろす角度 (度)
	float m_statusCamHeightOffset = 2;   // プレイヤーの注視点(胸・腰の高さ)
	float m_statusCamRotateSpeed = 45.0f;  // 回転速度 (度/秒: 45度なら約8秒で1周)

	struct PlayerStatusInfo
	{
		//HPや攻撃力などの数値
		int num;

		//数値の位置
		Math::Vector2 numPos;

		//数値の名前
		std::shared_ptr<KdTexture>nameTex;

		//数値の名前の位置
		Math::Vector2 namePos;

		//アイコン
		std::shared_ptr<KdTexture>iconTex;

		//アイコンの位置
		Math::Vector2 iconPos;
	};

	enum PlayerStatusInfoID
	{
		PlayerStatusInfo_HP,
		PlayerStatusInfo_MP,
		PlayerStatusInfo_AttackPower,
		PlayerStatusInfo_DefensePower,
		PlayerStatusInfo_Speed,
		PlayerStatusInfoSiz
	};

	PlayerStatusInfo m_playerStatusInfo[PlayerStatusInfoSiz];

	std::shared_ptr<KdTexture>m_playerStatusInfoBackTex;
};
