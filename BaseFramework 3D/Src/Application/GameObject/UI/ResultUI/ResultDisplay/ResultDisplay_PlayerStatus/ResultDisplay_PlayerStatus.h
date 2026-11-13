#pragma once
#include"../ResultDisplayBase.h"

class PlayerBase;
class WeaponBase;

class ResultDisplay_PlayerStatus :public ResultDisplayBase
{
public:
	ResultDisplay_PlayerStatus() {}
	~ResultDisplay_PlayerStatus() {}


	void Init();

	void Update();

	void PreDraw();
	void DrawSprite();

	void SetPlayerStatusGameResultFilePath(std::string _path);

	//ゲーム終了時の武器（攻撃力の計算に使う）
	void SetSpWeaponBase(std::shared_ptr<WeaponBase> _spWeapon);

private:		 
	//プレイヤーステータス
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

	std::shared_ptr<KdModelWork> m_spPlayerModel = nullptr;
	std::shared_ptr<KdAnimator>	m_spAnimetor = nullptr;

	//リザルト用に読み込んだプレイヤーと、ゲーム終了時の武器
	std::shared_ptr<PlayerBase> m_spResultPlayer = nullptr;
	std::shared_ptr<WeaponBase> m_spWeapon = nullptr;

	//インベントリ(PlayerInventoryUI)と同じ計算で数値を更新する
	void RefreshStatusNum();

	std::shared_ptr<KdTexture>m_statusTitleTex;
	Math::Vector2 m_statusTitleTexPos;
};
