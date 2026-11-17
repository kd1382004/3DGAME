#pragma once
#include"../ResultDisplayBase.h"

class PlayerBase;
class WeaponBase;

class ResultDisplay_PlayerStatus :public ResultDisplayBase
{
public:
	//コンストラクタ
	ResultDisplay_PlayerStatus() {}
	//デストラクタ
	~ResultDisplay_PlayerStatus() {}


	//初期化
	void Init();

	//更新
	void Update();

	//描画前の処理
	void PreDraw();
	//2Dの描画
	void DrawSprite();

	//ゲーム終了時のステータスのファイルのパスをセット(プレイヤーを読み込む)
	// _path ... ファイルのパス
	void SetPlayerStatusGameResultFilePath(std::string _path);

	//ゲーム終了時の武器（攻撃力の計算に使う）
	// _spWeapon ... 武器
	void SetSpWeaponBase(std::shared_ptr<WeaponBase> _spWeapon);

private:		 
	//プレイヤーステータス
	std::shared_ptr<KdRenderTargetPack> m_spRtTargetPack;

	//プレイヤーを映すカメラ
	std::shared_ptr<KdCamera> m_spStatusCamera;
	float m_statusCamAngleY = 0.0f;   // Y軸回転角度 (度)
	float m_statusCamDistance = 4.0f;   // プレイヤーからの距離 (お好みに合わせて調整)
	float m_statusCamPitch = 8.0f;   // 少し見下ろす角度 (度)
	float m_statusCamHeightOffset = 2;   // プレイヤーの注視点(胸・腰の高さ)
	float m_statusCamRotateSpeed = 45.0f;  // 回転速度 (度/秒: 45度なら約8秒で1周)

	struct PlayerStatusInfo
	{
		//HPや攻撃力などの数値
		int m_num;

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

	enum PlayerStatusInfoID
	{
		PlayerStatusInfo_HP,
		PlayerStatusInfo_MP,
		PlayerStatusInfo_AttackPower,
		PlayerStatusInfo_DefensePower,
		PlayerStatusInfo_Speed,
		PlayerStatusInfoSiz
	};

	//ステータスの表示情報
	PlayerStatusInfo m_playerStatusInfo[PlayerStatusInfoSiz];

	//ステータスの背景の画像
	std::shared_ptr<KdTexture>m_spPlayerStatusInfoBackTex;

	//プレイヤーのモデル
	std::shared_ptr<KdModelWork> m_spPlayerModel = nullptr;
	//プレイヤーのアニメーション
	std::shared_ptr<KdAnimator>	m_spAnimator = nullptr;

	//リザルト用に読み込んだプレイヤーと、ゲーム終了時の武器
	std::shared_ptr<PlayerBase> m_spResultPlayer = nullptr;
	//ゲーム終了時の武器
	std::shared_ptr<WeaponBase> m_spWeapon = nullptr;

	//インベントリ(PlayerInventoryUI)と同じ計算で数値を更新する
	void RefreshStatusNum();

	//「ステータス」の画像
	std::shared_ptr<KdTexture>m_spStatusTitleTex;
	//「ステータス」の位置
	Math::Vector2 m_statusTitleTexPos;
};
